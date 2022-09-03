#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "ModelLoader.h"
#include "VertexInput.h"
#include "VulkanException.h"


void Vulkan::ModelLoader<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>::loadModel(std::string pathToModel, std::vector<Vertex>& vertices, std::vector<uint32_t>& indexes) {
    //load the model into the tiny object loader library structures
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, pathToModel.c_str())) {
        throw VulkanException{ "Failed to load the model", warn + err };
    }


    //now fill our sturctures
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            glm::vec3 position{
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            }; 
            
            glm::vec3 normals{
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
            }; 
            
            glm::vec2 texture{
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertices.emplace_back(position, normals, texture);
            indexes.push_back(indexes.size());
        }
    }

}
