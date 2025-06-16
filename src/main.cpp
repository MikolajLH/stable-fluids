#include <unordered_map>
#include <algorithm>
#include <random>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/BaseWindow.h"
#include "graphics/Grid2d.h"
#include "graphics/primitives.h"

#include "Simulation.h"

class Demo : public wnd::BaseWindow {

public:
	Grid2d grid;
	Simulation sim;

	Demo(size_t N)
		:
		BaseWindow(800, 600, "Demo!"),
		grid(N, N, primitives::P2::quad),
		sim(N) {

		grid.scale = glm::vec2(10.f, 10.f);
		grid.change_and_update_interspace(glm::vec2(0.f, 0.f));

		for (size_t r = 0; r < grid.rows; ++r)
			for (size_t c = 0; c < grid.cols; ++c) {
				const float rf = static_cast<float>((rand() % 256)) / 255.f;
				const float gf = static_cast<float>((rand() % 256)) / 255.f;
				const float bf = static_cast<float>((rand() % 256)) / 255.f;

				grid.change_color(r, c, glm::vec3(rf, gf, bf));
			}

		grid.update_colors();

	}

	void update(float dt) override {

		const auto fw = this->get_width();
		const auto fh = this->get_height();

		static float visc = 0.0f;
		static float diff = 0.0f;

		sim.vel_step(sim.N, sim.vx, sim.vy, sim.vx_prev, sim.vy_prev, visc, dt);
		sim.dens_step(sim.N, sim.dens, sim.dens_prev, sim.vx, sim.vy, diff, dt);

		float max_dens = std::ranges::max(std::span<float>(sim.dens, sim.size));

		for (size_t r = 0; r < grid.rows; ++r)
			for (size_t c = 0; c < grid.cols; ++c) {
				const float dens = sim.dens[sim.IX(c, r)];
				const float gray = max_dens == 0.f ? dens : dens / max_dens;

				grid.change_color(r, c, glm::vec3(gray, gray, gray));
			}

		grid.update_colors();


		wnd::clear_screen(0.5f, 0.3f, 0.6f);
		
		grid.draw(glm::ortho(-0.5f * fw, 0.5f * fw, -0.5f * fh, 0.5f * fh));

		ImGui::ShowDemoWindow();
		ImGui::Begin("control panel");

		ImGui::DragFloat("translation x", &grid.origin.x, 1.f);
		ImGui::DragFloat("translation y", &grid.origin.y, 1.f);

		ImGui::InputFloat("scale x", &grid.scale.x, 0.5f, 1.f);
		ImGui::InputFloat("scale y", &grid.scale.y, 0.5f, 1.f);

		ImGui::End();

	}
};


int main() {
	Demo window(100);
	window.run();

	return 0;
}