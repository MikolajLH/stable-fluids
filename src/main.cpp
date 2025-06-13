#include <unordered_map>
#include <algorithm>
#include <random>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/BaseWindow.h"

class Demo : public wnd::BaseWindow {

public:

	Demo(size_t rs, size_t cs)
		:
		BaseWindow(800, 600, "Demo!") {

	}

	void update(float dt) override {

		wnd::clear_screen(0.5f, 0.3f, 0.6f);
	}
};


int main() {
	Demo window(200, 200);
	window.run();

	return 0;
}