#ifndef VULKAN_SET
#define VULKAN_SET

#include <glm/glm.hpp>
#include <tuple>
#include <concepts>
#include <type_traits>

#include "UniformBuffer.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"


namespace Vulkan {

	class Set {
	public:

		/**
		 * @brief Creates a Set where each binding is allocated in the same buffer. The first binding will be first in the buffer, and so on.
		 * @details For example, if sizeof(int) = 2 bytes and sizeof(float) = 4 bytes:
		 * @code struct A { int a, b; float c; }; struct B { float d, e, f; };
		 *		 Set<A, B, int> foo{buf};
		 * @details The layout of buf will be: aabbccccddddeeeeffffgg where g is the int.
		 *			The example doesn't take into account padding requirements, which are always satisfied when creating a buffer.
		 *			The VkDescriptorSetLayout object will always be a dynamic buffer.
		 * 
		 * @param buffer Reference to the buffer where, in the future, the data will be stored.
		 */
		template<typename Struct, template<std::same_as<Struct>, std::same_as<VkShaderStageFlagBits>>class... Bindings> requires (std::same_as<Bindings<Struct, VkShaderStageFlagBits>, std::pair<Struct, VkShaderStageFlagBits>> && ...)
		Set(const PhysicalDevice& realGpu, const LogicalDevice& virtualGpu, const Buffers::UniformBuffer& buffer, Bindings<Struct, VkShaderStageFlagBits>... bindings) : virtualGpu{ virtualGpu } {
			//get minimum alignment for the GPU memory, used for padding
			int alignment = realGpu.getProperties().limits.minUniformBufferOffsetAlignment;
			int currentOffset = 0;

			([this, &currentOffset, &buffer, alignment](Bindings binding) {
				this->bindingsInfo.emplace_back(sizeof(binding.first), buffer, currentOffset);

				currentOffset += sizeof(binding.first);
				int paddingAmount = (alignment - (currentOffset % alignment)) % alignment; //number of padding bytes
				currentOffset += paddingAmount; //padding
				}(bindings), ...);

			createDescriptorSetLayout(bindings.second...);
		}



		/**
		 * @brief Creates a Set where each binding is allocated in the corresponding buffer at the corrisponding offset. No checks on the offsets are carried out.
		 * @details For example, if sizeof(int) = 2 bytes and sizeof(float) = 4 bytes:
		 * @code struct A { int a, b; float c; }; struct B { float d, e; };
		 *		 Set foo{ {A(), buf1, 0}, {B(), buf2, 0}, {int g(), buf1, 14} };
		 * @details The layout of buf1 will be: aabbcccc------gg where '-' means undefined.
		 *			The layout of buf2 will be: ddddeeeeffff.
		 *			The VkDescriptorSetLayout object will always be a dynamic buffer.
		 * 
		 * @param ...bindingsInfo Type to be bounded, reference to the buffer where the data of this type will reside, offset into such buffer.
		 */
		template<typename Struct, template<std::same_as<Struct>, std::same_as<Buffers::UniformBuffer*>, std::same_as<int>, std::same_as<VkShaderStageFlagBits>>class... T> requires (std::same_as<T<Struct, Buffers::UniformBuffer*, int, VkShaderStageFlagBits>, std::tuple<Struct, Buffers::UniformBuffer*, int, VkShaderStageFlagBits>> && ...)
			Set(const LogicalDevice& virtualGpu, T<Struct, Buffers::UniformBuffer*, int, VkShaderStageFlagBits>... bindingsInfo) : virtualGpu{ virtualGpu } {
			(this->bindingsInfo.emplace_back(sizeof(std::get<0>(bindingsInfo)), *std::get<1>(bindingsInfo), std::get<2>(bindingsInfo)), ...);
		
			createDescriptorSetLayout(std::get<3>(bindingsInfo)...);
		}


		Set(const Set&) = delete;
		Set& operator=(const Set&) = delete;
		Set(Set&&) = delete;
		Set& operator=(Set&&) = delete;

		~Set() {
			vkDestroyDescriptorSetLayout(+virtualGpu, descriptorSetLayout, nullptr);
		}


		struct BindingInfo {
			int bindingSize; //the size of the binding
			Buffers::UniformBuffer& buffer; //reference to the buffer of the binding
			int offset; //offset in his buffer of the binding
		};

	private:

		template<std::same_as<VkShaderStageFlagBits>... ShaderStages>
		void createDescriptorSetLayout(ShaderStages... shaderStages) {
			//create info structs for each binding
			std::vector<VkDescriptorSetLayoutBinding> bindings; //vector containing the binding info structures for this set
			int counter = 0;
			([&bindings, &counter](VkShaderStageFlagBits shaderStage) {
				VkDescriptorSetLayoutBinding binding{};
				binding.binding = counter;
				binding.descriptorCount = 1; //no arrays for now
				binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				binding.pImmutableSamplers = nullptr;
				binding.stageFlags = shaderStage;
				bindings.push_back(binding);
				counter++;
			}(shaderStages), ...);

			//put toghether the info structs for each binding and create the final layout object
			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = bindings.size();
			layoutInfo.pBindings = bindings.data();

			if (VkResult result = vkCreateDescriptorSetLayout(+virtualGpu, &layoutInfo, nullptr, &descriptorSetLayout); result != VK_SUCCESS) {
				throw VulkanException("Failed to create descriptor set layout!", result);
			}
		}



		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<BindingInfo> bindingsInfo;
		const LogicalDevice& virtualGpu;

	};

}



#endif
