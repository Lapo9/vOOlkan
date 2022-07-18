#include "Shader.h"
#include "LogicalDevice.h"
#include "VulkanException.h"


Vulkan::PipelineOptions::Shader::Shader(const LogicalDevice& virtualGpu, std::string spirvFileName, VkShaderStageFlagBits shaderType, std::string entrypoint) : virtualGpu{ virtualGpu }, shaderStageInfo{}, entrypoint{ entrypoint } {
	auto code = readSpirvFile(spirvFileName); //the code of the shader in SPIR-V format

	//struct to create the shader module
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	//actually create the shader module
	if (VkResult result = vkCreateShaderModule(+virtualGpu, &createInfo, nullptr, &shaderModule); result != VK_SUCCESS) {
		throw VulkanException("Failed to create shader module for " + spirvFileName, result);
	}

	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = shaderType;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = this->entrypoint.c_str();
}


Vulkan::PipelineOptions::Shader::~Shader() {
	vkDestroyShaderModule(+virtualGpu, shaderModule, nullptr);
}


const VkPipelineShaderStageCreateInfo& Vulkan::PipelineOptions::Shader::operator+() const {
	return shaderStageInfo;
}


std::vector<char> Vulkan::PipelineOptions::Shader::readSpirvFile(std::string fileName) {
	std::ifstream file{ fileName, std::ios::ate | std::ios::binary };
	if (!file.is_open()) {
		throw VulkanException("Failed to open file " + fileName, "Check that the path is correct. The root of the path is the position of the executable.");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}