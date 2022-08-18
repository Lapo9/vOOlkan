#ifndef VULKAN_MODEL
#define VULKAN_MODEL

#include <vector>

#include "VertexInput.h"


namespace Vulkan { template<typename... Components>class Model; }


template<typename... Components>
class Vulkan::Model {
public:

	Model(std::vector<PipelineOptions::Vertex<Components...>> vertices, std::vector<uint32_t> indexes) : vertices{ vertices }, indexes{ indexes } {

	}
	
	const std::vector<PipelineOptions::Vertex<Components...>>& getVertices() const {
		return vertices;
	}

	const std::vector<uint32_t>& getIndexes() const {
		return indexes;
	}

private:
	std::vector<PipelineOptions::Vertex<Components...>> vertices;
	std::vector<uint32_t> indexes;
	std::vector<int> uniforms;
};

#endif
