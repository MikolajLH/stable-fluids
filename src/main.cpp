#include <unordered_map>
#include <algorithm>
#include <random>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/BaseWindow.h"
#include "graphics/Grid2d.h"
#include "graphics/primitives.h"

#include "Simulation.h"

class Demo : public wnd::BaseWindow {

public:
	Grid2d grid;
	StableSolver sim;

	Demo(size_t simX, size_t simY)
		:
		BaseWindow(800, 600, "Demo!"),
		grid(128, 128, primitives::P2::quad),
		sim{} {

		grid.scale = glm::vec2(5.f, 5.f);
		grid.change_and_update_interspace(glm::vec2(0.f, 0.f));

		for (size_t r = 0; r < grid.rows; ++r)
			for (size_t c = 0; c < grid.cols; ++c) {
				const float rf = static_cast<float>((rand() % 256)) / 255.f;
				const float gf = static_cast<float>((rand() % 256)) / 255.f;
				const float bf = static_cast<float>((rand() % 256)) / 255.f;

				grid.change_color(r, c, glm::vec3(rf, gf, bf));
			}

		grid.update_colors();
		sim.init();
		sim.reset();

	}

	void draw_densities() {
		for (int i = 1; i < sim.getRowSize() - 1; i++)
		{
			for (int j = 1; j < sim.getColSize() - 1; j++)
			{	
				const float d = sim.getDens(i, j);
				grid.change_color(j, i, glm::vec3(1.f - d, 1.f, 1.f - d));
			}
		}
	}

	void update(float dt) override {
		const auto fw = this->get_width();
		const auto fh = this->get_height();

		//GUI
		sim.cleanBuffer();

		ImGui::ShowDemoWindow();
		ImGui::Begin("control panel");

		ImGui::DragFloat("translation x", &grid.origin.x, 1.f);
		ImGui::DragFloat("translation y", &grid.origin.y, 1.f);

		ImGui::InputFloat("scale x", &grid.scale.x, 0.5f, 1.f);
		ImGui::InputFloat("scale y", &grid.scale.y, 0.5f, 1.f);

		ImGui::End();

		float mx = get_cursor_x() - fw * 0.5f - grid.origin.x;
		float my = get_cursor_y() - fh * 0.5f + grid.origin.y;

		int icol = int(mx / grid.scale.x);
		int irow = int(my / grid.scale.y);

		const size_t row = static_cast<size_t>(irow);
		const size_t col = static_cast<size_t>(icol);

		if (irow > 0 and row < sim.getColSize() - 1 and icol > 0 and col < sim.getRowSize() - 1) {
		
			if (is_lmb_down()) {
				std::println("row: {} col: {}", row, col);
				sim.setD0(icol, irow, 100.f);


			}

			if (is_rmb_down()) {
				//sim.reset();
				//sim.cleanBuffer();
				sim.setVY0(icol, irow, 1.f);
			}
		}

		for (size_t i = 1; i <= sim.getColSize() - 2; i++)
		{
			for (size_t j = 1; j <= sim.getRowSize() - 2; j++)
			{
				if(sim.getDens(j, i) > 0.01f)
					sim.setVY0(j, i, 10.f);
			}
		}

		sim.addSource();
		sim.vortConfinement();
		sim.animVel();
		sim.animDen();

		// Draw
		wnd::clear_screen(0.5f, 0.3f, 0.6f);
		draw_densities();

		grid.update_colors();
		grid.draw(glm::ortho(-0.5f * fw, 0.5f * fw, -0.5f * fh, 0.5f * fh));
	}
};


int main() {
	Demo window(10, 15);
	window.run();


	return 0;
}