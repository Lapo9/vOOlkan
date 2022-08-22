#ifndef VULKAN_SET
#define VULKAN_SET

#include <glm/glm.hpp>
#include <tuple>
#include <concepts>
#include <type_traits>

#include "UniformBuffer.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "DescriptorSetBindingCreationInfo.h"


namespace Vulkan {

	/**
	 * @brief A Set is a set of bindings which can be read by the shader.
	 * @details A Set keeps all the information about its bindings, such as their size, their buffer and the offset in such buffer. This way it is possible to instantiate a DescriptorSet starting from a Set only.
	 */
	class Set {
	public:

		/**
		 * @brief Returns the underlying VkDescriptorSetLayout object.
		 * 
		 * @return The underlying VkDescriptorSetLayout object.
		 */
		const VkDescriptorSetLayout& operator+() const {
			return descriptorSetLayout;
		}


		/**
		 * @brief A struct containing all of the information of a specific binding. This class must be the base of other classes.
		 */
		struct BindingInfo {

			/**
			 * @brief Creates the struct for this binding used to create the descriptor set.
			 *
			 * @param descriptorSet The descriptor set which is being filled with the returned struct..
			 * @return Tthe struct for this binding used to create the descriptor set.
			 */
			virtual DescriptorSetBindingCreationInfo generateDescriptorSetBindingInfo(const VkDescriptorSet& descriptorSet) const = 0;
			
			/**
			 * @brief The binding index.
			 */
			unsigned int binding;
		};


		/**
		 * @brief Returns the number of bindings in this set.
		 *
		 * @return The number of bindings in this set..
		 */
		int getAmountOfBindings() const {
			return bindingsInfo.size();
		}


		/**
		 * @brief Returns the size, offset and buffer of each binding in this set.
		 */
		const std::vector<std::unique_ptr<BindingInfo>>& getBindingsInfo() const {
			return bindingsInfo;
		}


		/**
		 * @brief Returns the size, offset and buffer of the binding number i of this set.
		 */
		const BindingInfo& getBindingInfo(int i) const {
			return *bindingsInfo[i];
		}


	protected:


		Set(const LogicalDevice& virtualGpu) : virtualGpu{ virtualGpu } {}

		Set(const Set&) = delete;
		Set& operator=(const Set&) = delete;
		Set(Set&&) = delete;
		Set& operator=(Set&&) = delete;

		~Set() {
			vkDestroyDescriptorSetLayout(+virtualGpu, descriptorSetLayout, nullptr);
		}


		//Creates the VkDescriptorSetLayout Vulkan object for this set
		template<std::same_as<std::pair<VkShaderStageFlagBits, VkDescriptorType>>... ShaderStages>
		void createDescriptorSetLayout(ShaderStages... shaderStages) {
			//create info structs for each binding
			std::vector<VkDescriptorSetLayoutBinding> bindings; //vector containing the binding info structures for this set
			int counter = 0;
			([&bindings, &counter](std::pair<VkShaderStageFlagBits, VkDescriptorType> shaderStage) {
				VkDescriptorSetLayoutBinding binding{};
				binding.binding = counter;
				binding.descriptorCount = 1; //no arrays for now
				binding.descriptorType = shaderStage.second;
				binding.pImmutableSamplers = nullptr;
				binding.stageFlags = shaderStage.first;
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
		std::vector<std::unique_ptr<BindingInfo>> bindingsInfo;
		const LogicalDevice& virtualGpu;

	};

}



#endif
