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
	 * @brief A DynamicSet is a Set where there is an array of objects, and each object has its bindings.
	 * @details In this implementation the binding are displaced in the buffer grouped by object.
	 *			For example if we have objects with these bindings: {A = 2bytes, B = 4bytes}, then the buffer will be like this: aabbbbaabbbb....
	 *			The right inter-binding padding is also taken into consideration (in the example this is not the case). Intra-binding padding is left as is.
	 */
	class DynamicSet : public Set {
	public:

		/**
		 * @brief Keeps the information to retrieve a specific binding in the buffer.
		 */
		struct DynamicSetBindingInfo : public Set::BindingInfo {
			DynamicSetBindingInfo(int size, const Buffers::UniformBuffer& buffer, int offset, int dynamicDistance = 0) : size{ size }, buffer{ buffer }, offset{ offset }, dynamicDistance{ dynamicDistance }{}

			/**
			 * @brief Creates the struct for this used to create the descriptor set.
			 * 
			 * @param binding .
			 * @param descriptorSet .
			 * @return .
			 */
			DescriptorSetBindingCreationInfo generateDescriptorSetBindingInfo(unsigned int binding, const VkDescriptorSet& descriptorSet) const override {
				return DescriptorSetBindingCreationInfo{ binding, descriptorSet, size, buffer, offset, dynamicDistance };
			}

			/**
			 * @brief Size in bytes of the binding.
			 */
			int size;
			/**
			 * @brief Reference to the buffer of the binding.
			 */
			const Buffers::UniformBuffer& buffer; 
			/**
			 * @brief Offset in his buffer of the binding. e.g. aa-bbbb -> a.offset = 0, b.offset = 3
			 */
			int offset; 
			/**
			 * @brief Distance between this binding in 2 different objects (e.g. aaaabbaaaabb a.dist = 6, b.dist = 6. aaaa aaaa bb bb a.dist = 4, b.dist = 2).
			 */
			int dynamicDistance;
		};


		/**
		 * @brief Creates a DynamicSet where each binding is allocated in the same buffer. The first binding will be first in the buffer, and so on.
		 * @details For example, if sizeof(int) = 2 bytes and sizeof(float) = 4 bytes:
		 * @code struct A { int a, b; float c; }; struct B { float d, e, f; };
		 *		 DynamicSet<A, B, int> foo{buf};
		 * @details The layout of buf will be: aabbccccddddeeeeffffgg where g is the int.
		 *			The example doesn't take into account inter-binding padding requirements, which are always satisfied when creating a buffer.
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
				auto tmp = std::make_unique<DynamicSetBindingInfo>(sizeof(binding.second), buffer, currentOffset);
				bindingsPerBuffer[&buffer].push_back(tmp.get());
				this->bindingsInfo.push_back(std::move(tmp));

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
			([this](T bindingInfo) {
				auto tmp = std::make_unique<DynamicSetBindingInfo>(sizeof(std::get<1>(bindingsInfo)), *std::get<2>(bindingsInfo), std::get<3>(bindingsInfo), std::get<4>(bindingsInfo));
				bindingsPerBuffer[&(tmp->buffer)].push_back(tmp.get());
				this->bindingsInfo.push_back(std::move(tmp));
				}(bindingsInfo), ...);
		
			createDescriptorSetLayout(std::pair{ std::get<0>(bindingsInfo), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC }...);
		}



		/**
		 * @brief Returns the offsets in the buffer of each binding of the i-th object.
		 * @details e.g. buffer content: aa--bbbb----aa--bbbb----aa--bbbb--
		 *				 buffer objects: 1111111111222222222222333333333333
		 *			Then getDynamicDistance(2) = {24, 24}, because the bindings 'a' of two consecutive objects are distant 12 bytes, and the same for bindings 'b'.
		 *			If we had buffer content: aa-aa-aa-...bbbb--bbbb--bbbb--...
		 *					  buffer objects: 111222333...111111222222333333...
		 *			Then getDynamicDistance(2) = {6, 12} beacause the bindings 'a' of 2 consecutive objects are distant 3 bytes, and 'b' 6 bytes.
		 *			The key point is that getDynamicDistance(x) + offset(binding) always points to the first byte of the specified binding.
		 */
		std::vector<uint32_t> getDynamicDistances(int i = 0) const {
			std::vector<uint32_t> res;

			for (const auto& binding : bindingsInfo) {
				res.push_back(static_cast<DynamicSetBindingInfo*>(binding.get())->dynamicDistance * i);
			}

			return res;
		}


		/**
		 * @brief Fills the buffer with the data in the tuples.
		 * @details The buffer must be one of the buffers present in this set, this means that at least one of the binding must use this buffer. If this pre-condition is not met, the function will throw.
		 *			The n-th element of the tuple will be inserted in the buffer at the offset specified by the n-th binding in the specified buffer contained in this set.
		 *			The m-th tuple will consider for each binding the dynamic distance * m.
		 *			e.g. If the set contains these bindings: {buffer, offset, dynamicDistance} -> {A, 0, 10}, {B, 0, 8}, {A, 4, 10}
		 *			Then fillBuffer(A, {xxx, yyyy}, {zzz, tttt}) will fill the buffer like this: xxx-yyyy--zzz-tttt----------
		 *			Basically the n-th element of the m-th tuple will be inserted in position: offset[n] + dynamicDistance[n] * m
		 *			No checks on the types in the tuple are performed (for now), so it is responsibility of the user to pass the right data to the function.
		 * 
		 * @param buffer Where to store the data.
		 * @param ...tuplesOfData Data to store.
		 */
		template<typename... E, template<typename...>class... T> requires (std::same_as<std::tuple<E...>, T<E...>> && ...)
		void fillBuffer(Buffers::UniformBuffer& buffer, const T<E...>&... tuplesOfData) const {
			fillBufferHelper(buffer, std::make_integer_sequence<int, sizeof...(E)>{}, tuplesOfData...);
		}



	private:

		//This function is used to perform a sort of constexpr for each loop on the tuple
		template<int... TI, typename... T>
		void fillBufferHelper(Buffers::UniformBuffer& buffer, std::integer_sequence<int, TI...>, const T&... tuplesOfData) const {
			try {
				auto& bindingsInfo = bindingsPerBuffer.at(&buffer); //get the info of the bindings of the buffer
			}
			catch (const std::out_of_range&) {
				throw VulkanException{ "Failed to fill the uniform buffer", "The buffer to be filled isn't used in this set" };
			}
			int counter = 0; //how many tuple we've already traversed

			//Basically the n - th element of the m - th tuple will be inserted in position : offset[n] + dynamicDistance[n] * m
			([&bindingsInfo, &counter, &buffer](const T& tuple) {
				(buffer.fillBuffer(std::get<TI>(tuple), bindingsInfo[TI]->offset + bindingsInfo[TI]->dynamicDistance * counter), ...);
				counter++;
			}(tuplesOfData), ...);
		}


		std::map<const Buffers::UniformBuffer*, std::vector<DynamicSetBindingInfo*>> bindingsPerBuffer; //this map holds the bindings relative to each buffer used in the Set

	};

}



#endif
