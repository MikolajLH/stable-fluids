#include <unordered_map>
#include <algorithm>
#include <random>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/BaseWindow.h"
#include "graphics/Grid2d.h"
#include "graphics/primitives.h"

#include "Sim.h"
#include <asio/asio.hpp>
#include <asio/asio/serial_port.hpp>

const uint8_t viridis_colormap[256][3] = {
    {68, 1, 84},
    {68, 2, 85},
    {68, 3, 87},
    {69, 5, 88},
    {69, 6, 90},
    {69, 8, 91},
    {70, 9, 92},
    {70, 11, 94},
    {70, 12, 95},
    {70, 14, 97},
    {71, 15, 98},
    {71, 17, 99},
    {71, 18, 101},
    {71, 20, 102},
    {71, 21, 103},
    {71, 22, 105},
    {71, 24, 106},
    {72, 25, 107},
    {72, 26, 108},
    {72, 28, 110},
    {72, 29, 111},
    {72, 30, 112},
    {72, 32, 113},
    {72, 33, 114},
    {72, 34, 115},
    {72, 35, 116},
    {71, 37, 117},
    {71, 38, 118},
    {71, 39, 119},
    {71, 40, 120},
    {71, 42, 121},
    {71, 43, 122},
    {71, 44, 123},
    {70, 45, 124},
    {70, 47, 124},
    {70, 48, 125},
    {70, 49, 126},
    {69, 50, 127},
    {69, 52, 127},
    {69, 53, 128},
    {69, 54, 129},
    {68, 55, 129},
    {68, 57, 130},
    {67, 58, 131},
    {67, 59, 131},
    {67, 60, 132},
    {66, 61, 132},
    {66, 62, 133},
    {66, 64, 133},
    {65, 65, 134},
    {65, 66, 134},
    {64, 67, 135},
    {64, 68, 135},
    {63, 69, 135},
    {63, 71, 136},
    {62, 72, 136},
    {62, 73, 137},
    {61, 74, 137},
    {61, 75, 137},
    {61, 76, 137},
    {60, 77, 138},
    {60, 78, 138},
    {59, 80, 138},
    {59, 81, 138},
    {58, 82, 139},
    {58, 83, 139},
    {57, 84, 139},
    {57, 85, 139},
    {56, 86, 139},
    {56, 87, 140},
    {55, 88, 140},
    {55, 89, 140},
    {54, 90, 140},
    {54, 91, 140},
    {53, 92, 140},
    {53, 93, 140},
    {52, 94, 141},
    {52, 95, 141},
    {51, 96, 141},
    {51, 97, 141},
    {50, 98, 141},
    {50, 99, 141},
    {49, 100, 141},
    {49, 101, 141},
    {49, 102, 141},
    {48, 103, 141},
    {48, 104, 141},
    {47, 105, 141},
    {47, 106, 141},
    {46, 107, 142},
    {46, 108, 142},
    {46, 109, 142},
    {45, 110, 142},
    {45, 111, 142},
    {44, 112, 142},
    {44, 113, 142},
    {44, 114, 142},
    {43, 115, 142},
    {43, 116, 142},
    {42, 117, 142},
    {42, 118, 142},
    {42, 119, 142},
    {41, 120, 142},
    {41, 121, 142},
    {40, 122, 142},
    {40, 122, 142},
    {40, 123, 142},
    {39, 124, 142},
    {39, 125, 142},
    {39, 126, 142},
    {38, 127, 142},
    {38, 128, 142},
    {38, 129, 142},
    {37, 130, 142},
    {37, 131, 141},
    {36, 132, 141},
    {36, 133, 141},
    {36, 134, 141},
    {35, 135, 141},
    {35, 136, 141},
    {35, 137, 141},
    {34, 137, 141},
    {34, 138, 141},
    {34, 139, 141},
    {33, 140, 141},
    {33, 141, 140},
    {33, 142, 140},
    {32, 143, 140},
    {32, 144, 140},
    {32, 145, 140},
    {31, 146, 140},
    {31, 147, 139},
    {31, 148, 139},
    {31, 149, 139},
    {31, 150, 139},
    {30, 151, 138},
    {30, 152, 138},
    {30, 153, 138},
    {30, 153, 138},
    {30, 154, 137},
    {30, 155, 137},
    {30, 156, 137},
    {30, 157, 136},
    {30, 158, 136},
    {30, 159, 136},
    {30, 160, 135},
    {31, 161, 135},
    {31, 162, 134},
    {31, 163, 134},
    {32, 164, 133},
    {32, 165, 133},
    {33, 166, 133},
    {33, 167, 132},
    {34, 167, 132},
    {35, 168, 131},
    {35, 169, 130},
    {36, 170, 130},
    {37, 171, 129},
    {38, 172, 129},
    {39, 173, 128},
    {40, 174, 127},
    {41, 175, 127},
    {42, 176, 126},
    {43, 177, 125},
    {44, 177, 125},
    {46, 178, 124},
    {47, 179, 123},
    {48, 180, 122},
    {50, 181, 122},
    {51, 182, 121},
    {53, 183, 120},
    {54, 184, 119},
    {56, 185, 118},
    {57, 185, 118},
    {59, 186, 117},
    {61, 187, 116},
    {62, 188, 115},
    {64, 189, 114},
    {66, 190, 113},
    {68, 190, 112},
    {69, 191, 111},
    {71, 192, 110},
    {73, 193, 109},
    {75, 194, 108},
    {77, 194, 107},
    {79, 195, 105},
    {81, 196, 104},
    {83, 197, 103},
    {85, 198, 102},
    {87, 198, 101},
    {89, 199, 100},
    {91, 200, 98},
    {94, 201, 97},
    {96, 201, 96},
    {98, 202, 95},
    {100, 203, 93},
    {103, 204, 92},
    {105, 204, 91},
    {107, 205, 89},
    {109, 206, 88},
    {112, 206, 86},
    {114, 207, 85},
    {116, 208, 84},
    {119, 208, 82},
    {121, 209, 81},
    {124, 210, 79},
    {126, 210, 78},
    {129, 211, 76},
    {131, 211, 75},
    {134, 212, 73},
    {136, 213, 71},
    {139, 213, 70},
    {141, 214, 68},
    {144, 214, 67},
    {146, 215, 65},
    {149, 215, 63},
    {151, 216, 62},
    {154, 216, 60},
    {157, 217, 58},
    {159, 217, 56},
    {162, 218, 55},
    {165, 218, 53},
    {167, 219, 51},
    {170, 219, 50},
    {173, 220, 48},
    {175, 220, 46},
    {178, 221, 44},
    {181, 221, 43},
    {183, 221, 41},
    {186, 222, 39},
    {189, 222, 38},
    {191, 223, 36},
    {194, 223, 34},
    {197, 223, 33},
    {199, 224, 31},
    {202, 224, 30},
    {205, 224, 29},
    {207, 225, 28},
    {210, 225, 27},
    {212, 225, 26},
    {215, 226, 25},
    {218, 226, 24},
    {220, 226, 24},
    {223, 227, 24},
    {225, 227, 24},
    {228, 227, 24},
    {231, 228, 25},
    {233, 228, 25},
    {236, 228, 26},
    {238, 229, 27},
    {241, 229, 28},
    {243, 229, 30},
    {246, 230, 31},
    {248, 230, 33},
    {250, 230, 34},
    {253, 231, 36},
};

void mapDensityToRGB_Viridis(float density, uint8_t& r, uint8_t& g, uint8_t& b) {
    int index = (int)(density * 255.0f);
    index = std::max(0, std::min(255, index));

    r = viridis_colormap[index][0];
    g = viridis_colormap[index][1];
    b = viridis_colormap[index][2];
}


asio::io_context io;

class Demo : public wnd::BaseWindow {

public:
	Grid2d grid;
	Simulation sim;

	bool serial_connected;
	asio::serial_port port;

	Demo(bool serial_connected)
		:
		BaseWindow(800, 600, "Demo!"),
		grid(Simulation::Rs, Simulation::Cs, primitives::P2::quad),
		sim{},
		serial_connected{ serial_connected },
		port{ io } {

		if (serial_connected) {
			port.open("COM5");
			port.set_option(asio::serial_port_base::baud_rate(115200));
			port.set_option(asio::serial_port_base::character_size(8));
			port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
			port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
			port.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
		}

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

		sim.reset();
	}

	void draw_densities(float* d, bool mode = true) {
		for (int i = 0; i < grid.rows; i++)
		{
			for (int j = 0; j < grid.cols; j++)
			{	
				const float ds = d[Simulation::IX(j, i)];
                uint8_t rd, gr, bl;
                mapDensityToRGB_Viridis(ds, rd, gr, bl);
                if(mode)
                    grid.change_color(i, j, glm::vec3(float(rd) / 255.f, float(gr) / 255.f, float(bl) / 255.f));
                else
				    grid.change_color(i, j, glm::vec3(1.f - ds, 1.f, 1.f - ds));
			}
		}
	}

	void update(float dtx) override {
		const auto fw = this->get_width();
		const auto fh = this->get_height();

		//GUI
		ImGui::Begin("control panel");

		ImGui::DragFloat("translation x", &grid.origin.x, 1.f);
		ImGui::DragFloat("translation y", &grid.origin.y, 1.f);

        if (ImGui::InputFloat("scale x", &grid.scale.x, 0.5f, 1.f)) {
            grid.change_and_update_interspace(glm::vec2(0.f, 0.f));
        }
        if (ImGui::InputFloat("scale y", &grid.scale.y, 0.5f, 1.f)) {
            grid.change_and_update_interspace(glm::vec2(0.f, 0.f));
        }

		if (ImGui::Button("reset")) {
			sim.reset();

			auto msg = std::format("{} {} 0\n", -1, -1);
			if (serial_connected)asio::write(port, asio::buffer(msg));
		}
		static float visc = 0.f;
		ImGui::InputFloat("visc", &visc, 0.5f, 1.f);
		if (ImGui::Button("change visc")) {
			sim.visc = visc;
			auto msg = std::format("{} {} {}\n", -2, -1, visc);
			if(serial_connected)asio::write(port, asio::buffer(msg));
		}

		static float diff = 0.f;
		ImGui::InputFloat("diff", &diff, 0.5f, 1.f);
		if (ImGui::Button("change diff")) {
			sim.diff = diff;;
			auto msg = std::format("{} {} {}\n", -3, -1, diff);
			if (serial_connected)asio::write(port, asio::buffer(msg));
		}

		static float vorticity = 0.f;
		ImGui::InputFloat("vorticity", &vorticity, 0.5f, 1.f);
		if (ImGui::Button("change vorticity")) {
			sim.vorticity = vorticity;;
			auto msg = std::format("{} {} {}\n", -4, -1, vorticity);
			if (serial_connected)asio::write(port, asio::buffer(msg));
		}

        static bool d_mode = false;
        if (ImGui::Button("change drawing mode")) {
            d_mode = not d_mode;
        }

		ImGui::End();

		float mx = get_cursor_x() - fw * 0.5f - grid.origin.x;
		float my = get_cursor_y() - fh * 0.5f + grid.origin.y;

		int icol = int(mx / grid.scale.x);
		int irow = int(my / grid.scale.y);

		sim.clear_buffer();

		if (irow > 0 and irow < Simulation::Ny + 1 and icol > 0 and icol < Simulation::Nx + 1) {
		
			if (is_lmb_down()) {
				sim.d0[Simulation::IX(icol, irow)] += 100.f;
				auto msg = std::format("{} {} 0\n", icol - 1, irow - 1);
				
				if (serial_connected)asio::write(port, asio::buffer(msg));
			}
		}

		for (int i = 1; i <= Simulation::Nx; ++i)
			for (int j = 1; j <= Simulation::Ny; ++j)
				if (sim.d[Simulation::IX(i, j)] > 0.01f)
					sim.vy0[Simulation::IX(i, j)] += 10.f;
			
		sim.add_source();
		sim.vort_confinement();
		sim.vel_step();
		sim.den_step();


		// Draw
		wnd::clear_screen(0.5f, 0.3f, 0.6f);
		draw_densities(sim.d, d_mode);

		grid.update_colors();
		grid.draw(glm::ortho(-0.5f * fw, 0.5f * fw, -0.5f * fh, 0.5f * fh));
	}
};


int main() {
	try {
		Demo window{false};
		window.run();
	}
	catch (std::exception e) {
		std::println("{}", e.what());
	}


	return 0;
}