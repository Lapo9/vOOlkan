#ifndef VULKAN_DYNAMICSET
#define VULKAN_DYNAMICSET

#include <glm/glm.hpp>
#include <tuple>
#include <concepts>
#include <type_traits>

#include "UniformBuffer.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Set.h"


namespace Vulkan {

	/**
	 * @brief A DynamicSet is a set of bindings which can be read by the shader.
	 * @details A DynamicSet keeps all the information about its bindings, such as their size, their buffer and the offset in such buffer. This way it is possible to instantiate a DescriptorSet starting from a DynamicSet only.
	 */
	class DynamicSet : public Set {
	public:

		struct DynamicSetBindingInfo : public Set::BindingInfo {
			DynamicSetBindingInfo(int size, const Buffers::UniformBuffer& buffer, int offset, int dynamicDistance = 0) : size{ size }, buffer{ buffer }, offset{ offset }, dynamicDistance{ dynamicDistance }{}

			DescriptorSetBindingCreationInfo generateDescriptorSetBindingInfo(unsigned int binding, const VkDescriptorSet& descriptorSet) const override {
				return DescriptorSetBindingCreationInfo{ binding, descriptorSet, size, buffer, offset };
			}

			int size; //the size of the binding
			const Buffers::UniformBuffer& buffer; //reference to the buffer of the binding
			int offset; //offset in his buffer of the binding
			int dynamicDistance; //distance between this binding in 2 different objects (e.g. AAAABB AAAABB distA=6, distB=6. AAAA AAAA BB BB distA=4, distB=2)
		};


		/**
		 * @brief Creates a DynamicSet where each binding is allocated in the same buffer. The first binding will be first in the buffer, and so on.
		 * @details For example, if sizeof(int) = 2 bytes and sizeof(float) = 4 bytes:
		 * @code struct A { int a, b; float c; }; struct B { float d, e, f; };
		 *		 DynamicSet<A, B, int> foo{buf};
		 * @details The layout of buf will be: aabbccccddddeeeeffffgg where g is the int.
		 *			The example doesn't take into account padding requirements, which are always satisfied when creating a buffer.
		 *			The VkDescriptorSetLayout object will always be a dynamic buffer.
		 * 
		 * @param buffer Reference to the buffer where, in the future, the data will be stored.
		 */
		template<typename... Structs, template<typename, typename>class... Bindings> requires (std::same_as<Bindings<VkShaderStageFlagBits, Structs>, std::pair<VkShaderStageFlagBits, Structs>> && ...)
		DynamicSet(const PhysicalDevice& realGpu, const LogicalDevice& virtualGpu, const Buffers::UniformBuffer& buffer, Bindings<VkShaderStageFlagBits, Structs>... bindings) : Set{ virtualGpu } {
			//get minimum alignment for the GPU memory, used for padding
			int alignment = realGpu.getProperties().limits.minUniformBufferOffsetAlignment;
			int currentOffset = 0;

			([this, &currentOffset, &buffer, alignment](std::pair<VkShaderStageFlagBits, Structs> binding) {
				this->bindingsInfo.push_back(std::make_unique<DynamicSetBindingInfo>(sizeof(binding.second), buffer, currentOffset));

				currentOffset += sizeof(binding.second);
				int paddingAmount = (alignment - (currentOffset % alignment)) % alignment; //number of padding bytes
				currentOffset += paddingAmount; //padding
				}(bindings), ...);

			//the dynamic distance for bindings allocated in this way is always the same and equal to the total size of the bindings
			for (auto& bindingInfo : bindingsInfo) {
				static_cast<DynamicSetBindingInfo*>(bindingInfo.get())->dynamicDistance = currentOffset;
			}

			createDescriptorSetLayout(std::pair{ bindings.first, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC }...);
		}



		/**
		 * @brief Creates a DynamicSet where each binding is allocated in the corresponding buffer at the corrisponding offset. No checks on the offsets are carried out.
		 * @details For example, if sizeof(int) = 2 bytes and sizeof(float) = 4 bytes:
		 * @code struct A { int a, b; float c; }; struct B { float d, e; };
		 *		 DynamicSet foo{ {A(), buf1, 0}, {B(), buf2, 0}, {int g(), buf1, 14} };
		 * @details The layout of buf1 will be: aabbcccc------gg where '-' means undefined.
		 *			The layout of buf2 will be: ddddeeeeffff.
		 *			The VkDescriptorSetLayout object will always be a dynamic buffer.
		 * 
		 * @param ...bindingsInfo Shader stage in which the binding is used, type to be bounded, reference to the buffer where the data of this type will reside, offset into such buffer, dynamic distance between bindings of 2 contiguous objects.
		 */
		template<typename... Structs, typename... T> requires 
			(std::same_as<T, std::tuple<VkShaderStageFlagBits, Structs, Buffers::UniformBuffer*, int, int>> && ...)
			DynamicSet(const LogicalDevice& virtualGpu, T... bindingsInfo) : Set{ virtualGpu } {
			(this->bindingsInfo.push_back(std::make_unique<DynamicSetBindingInfo>(sizeof(std::get<1>(bindingsInfo)), *std::get<2>(bindingsInfo), std::get<3>(bindingsInfo), std::get<4>(bindingsInfo))), ...);
		
			createDescriptorSetLayout(std::pair{ std::get<0>(bindingsInfo), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC }...);
		}



		std::vector<uint32_t> getDynamicDistances(int i = 0) const {
			std::vector<uint32_t> res;

			for (const auto& binding : bindingsInfo) {
				res.push_back(static_cast<DynamicSetBindingInfo*>(binding.get())->dynamicDistance * i);
			}

			return res;
		}

	};

}



#endif
