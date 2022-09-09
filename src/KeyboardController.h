#ifndef VULKAN_KEYBOARDCONTROLLER
#define VULKAN_KEYBOARDCONTROLLER

#include <vector>
#include <string>

#include "Window.h"


namespace Vulkan::Utilities {

	class KeyboardListener{
	public:
		virtual void notifyKeyPress(int keyPressed) = 0;
	};


	class KeyboardController {
	public:
		template<std::derived_from<KeyboardListener>... Listeners>
		KeyboardController(const Window& window, Listeners&... listeners) : window{ window } {
			(this->listeners.push_back(&listeners), ...);
		}


		void checkKeyPressed() {
			for (int key = 32; key <= 96; ++key) {
				if (glfwGetKey(+window, key)) {
					publishKeyPress(key);
				}
			}
			for (int key = 256; key <= 348; ++key) {
				if (glfwGetKey(+window, key)) {
					publishKeyPress(key);
				}
			}
		}


	private:

		void publishKeyPress(int keyPressed) {
			for (auto& listener : listeners) {
				listener->notifyKeyPress(keyPressed);
			}
		}


		std::vector<KeyboardListener*> listeners;
		const Window& window;
	};

}


#endif
