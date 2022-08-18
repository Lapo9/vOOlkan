#ifndef VULKAN_BINDING
#define VULKAN_BINDING

#include <glm/glm.hpp>
#include <tuple>
#include <concepts>


namespace Vulkan {


	template<typename... T>
	class Binding {
	public:



	private:
		std::tuple<T...> components;

	};



}



#endif
