#ifndef VULKAN_SHADER
#define VULKAN_SHADER

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <string>


namespace Vulkan { class LogicalDevice; namespace PipelineOptions { class Shader; } }

/**
 * @brief A Shader is a program which runs on the GPU and performs graphics.
 */
class Vulkan::PipelineOptions::Shader {
	public:

		/**
		 * @brief Creates a shader starting from the SPIR-V code.
		 * 
		 * @param virtualGpu The LogicalDevice where the Pipeline using this shader will run on.
		 * @param spirvFileName Path to the SPIR-V code. The root of the path is the folder where the executable od the program using this class is placed.
		 * @param shaderType The type of the shader (generally a vertex, tessellation or fragment shader).
		 * @param entrypoint Name of the first function to call in the shader.
		 */
		Shader(const LogicalDevice& virtualGpu, std::string spirvFileName, VkShaderStageFlagBits shaderType, std::string entrypoint = "main");

		Shader(const Shader&) = delete;
		Shader(Shader&&) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) = delete;

		~Shader();

		/**
		 * @brief Returns the underlying VkPipelineShaderStageCreateInfo struct.
		 * 
		 * @return The underlying VkPipelineShaderStageCreateInfo struct.
		 */
		const VkPipelineShaderStageCreateInfo& operator+() const;


	private:

		//reads the SPIR-V code and outputs it as a vector of bytes
		static std::vector<char> readSpirvFile(std::string fileName);


		VkShaderModule shaderModule;
		VkPipelineShaderStageCreateInfo shaderStageInfo;
		const LogicalDevice& virtualGpu;
};

#endif
