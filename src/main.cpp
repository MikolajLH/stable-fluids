#include <unordered_map>
#include <algorithm>
#include <random>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/BaseWindow.h"
#include "graphics/Grid2d.h"
#include "graphics/primitives.h"


namespace simulation
{
	static constexpr int Nx = 32;
	static constexpr int Ny = 64;
	static constexpr int Rs = Ny + 2;
	static constexpr int Cs = Nx + 2;
	static constexpr int SIZE = Rs * Cs;
	static constexpr int DIFFUSE_ITER = 20;
	static constexpr int PROJECT_ITER = 20;

	float total_density;

	float vx[SIZE] = { 0 };
	float vx_prev[SIZE] = { 0 };

	float vy[SIZE] = { 0 };
	float vy_prev[SIZE] = { 0 };

	float dens[SIZE] = { 0 };
	float dens_prev[SIZE] = { 0 };

	float p[SIZE] = { 0 };
	float div[SIZE] = { 0 };

	float* p_vx = vx;
	float* p_vx_prev = vx_prev;

	float* p_vy = vy;
	float* p_vy_prev = vy_prev;

	float* p_dens = dens;
	float* p_dens_prev = dens_prev;

	static constexpr int IX(int x, int y) { return Cs * y + x; }

	static void fill_with(float* arr, float v) {
		for (int i = 0; i < SIZE; ++i)
			arr[i] = v;
	}

	static void set_bnd(float* arr, int flag) {
		// velocity x
		if (flag == 1) {
			for (int i = 1; i <= Nx; i++)
			{
				arr[IX(i, 0)] = arr[IX(i, 1)];
				arr[IX(i, Ny + 1)] = arr[IX(i, Ny)];
			}

			for (int j = 1; j <= Ny; j++)
			{
				arr[IX(0, j)] = -arr[IX(1, j)];
				arr[IX(Nx + 1, j)] = -arr[IX(Nx, j)];
			}
		}
		// velocity y
		if (flag == 2) {

			for (int i = 1; i <= Nx; i++)
			{
				arr[IX(i, 0)] = -arr[IX(i, 1)];
				arr[IX(i, Ny + 1)] = -arr[IX(i, Ny)];
			}

			for (int j = 1; j <= Ny; j++)
			{
				arr[IX(0, j)] = arr[IX(1, j)];
				arr[IX(Nx + 1, j)] = arr[IX(Nx, j)];
			}
		}
		//density
		if (flag == 0) {
			for (int i = 1; i <= Nx; i++)
			{
				arr[IX(i, 0)] = arr[IX(i, 1)];
				arr[IX(i, Ny + 1)] = arr[IX(i, Ny)];
			}
			for (int j = 1; j <= Ny; j++)
			{
				arr[IX(0, j)] = arr[IX(1, j)];
				arr[IX(Nx + 1, j)] = arr[IX(Nx, j)];
			}
		}
		//corners
		arr[IX(0, 0)] = (arr[IX(0, 1)] + arr[IX(1, 0)]) / 2.f;
		arr[IX(Nx + 1, 0)] = (arr[IX(Nx, 0)] + arr[IX(Nx + 1, 1)]) / 2.f;
		arr[IX(0, Ny + 1)] = (arr[IX(0, Ny)] + arr[IX(1, Ny + 1)]) / 2.f;
		arr[IX(Nx + 1, Ny + 1)] = (arr[IX(Nx, Ny + 1)] + arr[IX(Nx + 1, Ny)]) / 2.f;
	}

	static void add_source(float* x, float* s, float dt) {
		for (int i = 0; i < SIZE; ++i)
			x[i] += dt * s[i];
	}

	static void diffuse(float* x, float* x0, float diff, float dt, int flag) {
		const float a = diff * dt; //
		for (int k = 0; k < DIFFUSE_ITER; ++k)
			for (int ix = 1; ix <= Nx; ++ix)
				for (int iy = 1; iy <= Ny; ++iy)
					x[IX(ix, iy)] = (x0[IX(ix, iy)] + a * (x[IX(ix - 1, iy)] + x[IX(ix + 1, iy)] + x[IX(ix, iy - 1)] + x[IX(ix, iy + 1)])) / (1.f + 4.f * a);
		
		set_bnd(x, flag);
	}

	static void advect(float* d, float* d0, float* vx, float* vy, float dt, int flag) {
		for (int ix = 1; ix <= Nx; ++ix)
			for (int iy = 1; iy <= Ny; ++iy) {
				float x = ix - dt * vx[IX(ix, iy)];
				float y = iy - dt * vy[IX(ix, iy)];

				if (x < 0.5f) x = 0.5f;
				if (x > Nx + 0.5f) x = Nx + 0.5f;
				int i0 = (int)x;
				int i1 = i0 + 1;

				if (y < 0.5f) y = 0.5f;
				if (y > Ny + 0.5f) y = Ny + 0.5f;
				int j0 = (int)y;
				int j1 = j0 + 1;
				float s1 = x - i0;
				float s0 = 1 - s1;
				float t1 = y - j0;
				float t0 = 1 - t1;
				d[IX(ix, iy)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) + s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);

			}

		set_bnd(d, flag);
	}

	static void project(float* vx, float* vy, float* p, float* div) {
		for (int i = 1; i <= Nx; i++)
		{
			for (int j = 1; j <= Ny; j++)
			{
				div[IX(i, j)] = 0.5f * (vx[IX(i + 1, j)] - vx[IX(i - 1, j)] + vy[IX(i, j + 1)] - vy[IX(i, j - 1)]);
				p[IX(i, j)] = 0.0f;;
			}
		}
		set_bnd(div, 0);
		set_bnd(p, 0);
		for (int k = 0; k < PROJECT_ITER; k++) {
			for (int i = 1; i <= Nx; i++) {
				for (int j = 1; j <= Ny; j++) {
					p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] + p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4.f;
				}
			}
			set_bnd(p, 0);
		}

		for (int i = 1; i <= Nx; i++) {
			for (int j = 1; j <= Ny; j++) {
				vx[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]);
				vy[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]);
			}
		}

		set_bnd(vx, 1);
		set_bnd(vy, 2);
	}

	static float sum_arr(float* d) {
		float acc = 0.f;
		for (int i = 0; i < SIZE; ++i)
			acc += d[i];

		return acc;
	}

	static void conserve_density(float* d, float total_density) {
		float current_total_density = 0.f;
		for (int i = 0; i < SIZE; ++i)
			current_total_density += d[i];

		if (current_total_density > 0.f) {
			float correction_factor = total_density / current_total_density;
			for (int i = 0; i < SIZE; ++i)
				d[i] *= correction_factor;
		}
	}
}

class Demo : public wnd::BaseWindow {

public:
	Grid2d grid;

	Demo()
		:
		BaseWindow(800, 600, "Demo!"),
		grid(simulation::Rs, simulation::Cs, primitives::P2::quad) {

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
	}

	void draw_densities(float* d) {
		for (int i = 0; i < grid.rows; i++)
		{
			for (int j = 0; j < grid.cols; j++)
			{	
				const float ds = d[simulation::IX(j, i)];
				grid.change_color(i, j, glm::vec3(1.f - ds, 1.f, 1.f - ds));
			}
		}
	}

	void update(float dtx) override {
		const auto fw = this->get_width();
		const auto fh = this->get_height();

		simulation::fill_with(simulation::p_vx_prev, 0.f);
		simulation::fill_with(simulation::p_vy_prev, 0.f);
		simulation::fill_with(simulation::p_dens_prev, 0.f);

		//GUI
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

		

		if (irow > 0 and irow < simulation::Ny + 1 and icol > 0 and icol < simulation::Nx + 1) {
		
			if (is_lmb_down()) {
				simulation::p_dens_prev[simulation::IX(icol, irow)] += 100.f;
			}

			if (is_rmb_down()) {
				simulation::p_vy_prev[simulation::IX(icol, irow)] += 1.f;
			}
		}

		for (int i = 1; i <= simulation::Nx; ++i)
			for (int j = 1; j <= simulation::Ny; ++j)
				if (simulation::p_dens[simulation::IX(i, j)] > 0.01f)
					simulation::p_vy_prev[simulation::IX(i, j)] += 10.f;

		float visc = 0.f;
		float diff = 0.f;
		float dt = 0.016f;

		simulation::add_source(simulation::p_vx, simulation::p_vx_prev, dt);
		simulation::add_source(simulation::p_vy, simulation::p_vy_prev, dt);

		//std::swap(simulation::p_vx, simulation::p_vx_prev);
		//std::swap(simulation::p_vy, simulation::p_vy_prev);
		simulation::diffuse(simulation::p_vx, simulation::p_vx_prev, visc, dt, 1);
		simulation::diffuse(simulation::p_vy, simulation::p_vy_prev, visc, dt, 2);
		simulation::project(simulation::p_vx, simulation::p_vy, simulation::p, simulation::div);
		std::swap(simulation::p_vx, simulation::p_vx_prev);
		std::swap(simulation::p_vy, simulation::p_vy_prev);
		simulation::advect(simulation::p_vx, simulation::p_vx_prev, simulation::p_vx_prev, simulation::p_vy_prev, dt, 1);
		simulation::advect(simulation::p_vy, simulation::p_vy_prev, simulation::p_vx_prev, simulation::p_vy_prev, dt, 2);
		simulation::project(simulation::p_vx, simulation::p_vy, simulation::p, simulation::div);



		simulation::add_source(simulation::p_dens, simulation::p_dens_prev, dt);

		//std::swap(simulation::p_dens, simulation::p_dens_prev);
		//simulation::diffuse(simulation::p_dens, simulation::p_dens_prev, diff, dt, 0);

		std::swap(simulation::p_dens, simulation::p_dens_prev);
		simulation::advect(simulation::p_dens, simulation::p_dens_prev, simulation::p_vx_prev, simulation::p_vy_prev, dt, 0);

		// Draw
		wnd::clear_screen(0.5f, 0.3f, 0.6f);
		draw_densities(simulation::p_dens);

		grid.update_colors();
		grid.draw(glm::ortho(-0.5f * fw, 0.5f * fw, -0.5f * fh, 0.5f * fh));
	}
};


int main() {
	Demo window{};
	window.run();


	return 0;
}