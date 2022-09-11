#ifndef VULKAN_KEYBOARDLISTENER
#define VULKAN_KEYBOARDLISTENER

#include <vector>
#include <string>

#include "Window.h"


namespace Vulkan::Utilities {

	class KeyboardObserver{
	public:
		virtual void onKeyPress(int keyPressed) = 0;
	};


	class KeyboardListener {
	public:
		template<std::derived_from<KeyboardObserver>... Listeners>
		KeyboardListener(const Window& window, Listeners&... listeners) : window{ window } {
			(this->listeners.push_back(&listeners), ...);
		}


		void checkKeyPressed() {
			for (int key = 32; key <= 96; ++key) {
				if (glfwGetKey(+window, key)) {
					notifyKeyPress(key);
				}
			}
			for (int key = 256; key <= 348; ++key) {
				if (glfwGetKey(+window, key)) {
					notifyKeyPress(key);
				}
			}
		}


	private:

		void notifyKeyPress(int keyPressed) {
			for (auto& listener : listeners) {
				listener->onKeyPress(keyPressed);
			}
		}


		std::vector<KeyboardObserver*> listeners;
		const Window& window;
	};

}


#endif
