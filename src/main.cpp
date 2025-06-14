#include <unordered_map>
#include <algorithm>
#include <random>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/BaseWindow.h"
#include "graphics/Grid2d.h"
#include "graphics/primitives.h"

class Demo : public wnd::BaseWindow {

public:
	Grid2d grid;

	Demo(size_t rs, size_t cs)
		:
		BaseWindow(800, 600, "Demo!"),
		grid(rs, cs, primitives::P2::quad) {

		grid.scale = glm::vec2(10.f, 10.f);
		grid.change_and_update_interspace(glm::vec2(0.f, 0.f));
	}

	void update(float dt) override {

		const auto fw = this->get_width();
		const auto fh = this->get_height();


		wnd::clear_screen(0.5f, 0.3f, 0.6f);

		for (size_t r = 0; r < grid.rows; ++r)
			for (size_t c = 0; c < grid.cols; ++c) {
				const float rf = static_cast<float>((rand() % 256)) / 255.f;
				const float gf = static_cast<float>((rand() % 256)) / 255.f;
				const float bf = static_cast<float>((rand() % 256)) / 255.f;

				grid.change_color(r, c, glm::vec3(rf, gf, bf));
			}

		grid.update_colors();
				
		
		grid.draw(glm::ortho(-0.5f * fw, 0.5f * fw, -0.5f * fh, 0.5f * fh));

	}
};


int main() {
	Demo window(200, 200);
	window.run();

	return 0;
}