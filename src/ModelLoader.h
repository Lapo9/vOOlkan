#ifndef VULKAN_MODELLOADER
#define VULKAN_MODELLOADER

#include <vector>

#include "VertexInput.h"


namespace Vulkan {


	template<PipelineOptions::IsVertex Vertex>
	class ModelLoader;



	template<>
	class ModelLoader < PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>> {
	
	private:
		using Vertex = PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>;
	
	public:
		static void loadModel(std::string pathToModel, std::vector<Vertex>& vertices, std::vector<uint32_t>& indexes);

	};

}



#endif
