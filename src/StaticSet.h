#ifndef VULKAN_STATICSET
#define VULKAN_STATICSET

#include <glm/glm.hpp>
#include <tuple>
#include <concepts>
#include <type_traits>

#include "UniformBuffer.h"
#include "TextureImage.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Set.h"


namespace Vulkan {

	/**
	 * @brief In a static set there isn't the notion of 'object' (like in DynamicSet). Each binding is "single". It can also contain images.
	 */
	class StaticSet : public Set {
	public:
		//TODO comment
		struct StaticBufferBindingInfo : public Set::BindingInfo {
			StaticBufferBindingInfo(int size, const Buffers::UniformBuffer& buffer, int offset) : size{ size }, buffer{ buffer }, offset{ offset } {}

			DescriptorSetBindingCreationInfo generateDescriptorSetBindingInfo(const VkDescriptorSet& descriptorSet) const override {
				return DescriptorSetBindingCreationInfo{ binding, descriptorSet, size, buffer, offset };
			}

			int size; //the size of the binding
			const Buffers::UniformBuffer& buffer; //reference to the buffer of the binding
			int offset; //offset in his buffer of the binding
		};


		struct ImageBindingInfo : public Set::BindingInfo {
			ImageBindingInfo(const TextureImage& texture) : texture{ texture } {}

			DescriptorSetBindingCreationInfo generateDescriptorSetBindingInfo(const VkDescriptorSet& descriptorSet) const override {
				return DescriptorSetBindingCreationInfo{ binding, descriptorSet, texture };
			}

			const TextureImage& texture;
		};



		/**
		 * @brief Creates a StaticSet where each binding is allocated in the corresponding buffer at the corrisponding offset. No checks on the offsets are carried out. Image binding are treated differently.
		 * @details For example, if sizeof(int) = 2 bytes and sizeof(float) = 4 bytes:
		 * @code struct A { int a, b; float c; }; struct B { float d, e; };
		 *		 DynamicSet foo{ {A(), buf1, 0}, {B(), buf2, 0}, {int g(), buf1, 14} };
		 * @details The layout of buf1 will be: aabbcccc------gg where '-' means undefined.
		 *			The layout of buf2 will be: ddddeeeeffff.
		 *			The VkDescriptorSetLayout object will always be a dynamic buffer.
		 *
		 * @param ...bindingsInfo
		 */
		template<typename... Structs, typename... T> requires
			((std::same_as<T, std::tuple<VkShaderStageFlagBits, Structs, Buffers::UniformBuffer*, int>> || std::same_as<T, std::tuple<VkShaderStageFlagBits, TextureImage*>>) && ...)
			StaticSet(const LogicalDevice& virtualGpu, const T&... bindingsInfo) : Set{ virtualGpu } {
			(this->bindingsInfo.push_back(createBindingInfo(bindingsInfo)), ...);

			createDescriptorSetLayout(std::pair{ std::get<0>(bindingsInfo), std::same_as<T, std::tuple<VkShaderStageFlagBits, TextureImage*>> ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }...);
		}



		/**
		 * @brief Fills the buffer by placing the passed data into the corresponding position in the buffer.
		 * @details The buffer must be one of the buffers present in this set, this means that at least one of the binding must use this buffer. If this pre-condition is not met, the function will throw.
		 *			The n-th element will be inserted in the buffer at the offset specified by the n-th binding in the specified buffer contained in this set.
		 *			e.g. If the set contains these bindings: {buffer, offset} -> {A, 0}, {B, 0}, {A, 4}
		 *			Then fillBuffer(A, xxx, yyyy) will fill the buffer like this: xxx-yyyy
		 *			Basically the n-th binding will be inserted in position: offset[n]
		 *			No checks on the types of the elements are performed (for now), so it is responsibility of the user to pass the right data to the function.
		 * 
		 * @param buffer Where to store the data.
		 * @param ...bindings Bindings to store.
		 */
		template<typename... Bindings>
		void fillBuffer(Buffers::UniformBuffer& buffer, const Bindings&... bindings) const {
			std::vector<StaticBufferBindingInfo*> bindingsInfo;
			try {
				bindingsInfo = bindingsPerBuffer.at(&buffer); //get the info of the bindings of the buffer
			}
			catch (const std::out_of_range&) {
				throw VulkanException{ "Failed to fill the uniform buffer", "The buffer to be filled isn't used in this set" };
			}

			//fill the buffer by placing the n-th binding at the position pointed by the n-th bindingInfo (for the buffer) offset
			int counter = 0;
			([&bindingsInfo, &counter, &buffer](Bindings binding) {
				buffer.fillBuffer(binding, bindingsInfo[counter]->offset);
				counter++;
				}(bindings), ...);

		}



	private:

		template<typename Struct>
		std::unique_ptr<StaticBufferBindingInfo> createBindingInfo(const std::tuple<VkShaderStageFlagBits, Struct, Buffers::UniformBuffer*, int>& info) {
			auto tmp = std::make_unique<StaticBufferBindingInfo>(sizeof(std::get<1>(info)), *std::get<2>(info), std::get<3>(info));
			tmp->binding = bindingsInfo.size();
			bindingsPerBuffer[std::get<2>(info)].push_back(tmp.get());
			return tmp;
		}


		std::unique_ptr<ImageBindingInfo> createBindingInfo(const std::tuple<VkShaderStageFlagBits, TextureImage*>& info) {
			auto tmp = std::make_unique<ImageBindingInfo>(*std::get<1>(info));
			tmp->binding = bindingsInfo.size();
			return tmp;
		}


		std::map<const Buffers::UniformBuffer*, std::vector<StaticBufferBindingInfo*>> bindingsPerBuffer; //this map holds the bindings relative to each buffer used in the Set


	};

}



#endif
