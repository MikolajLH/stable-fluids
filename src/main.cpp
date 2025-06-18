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



class Demo : public wnd::BaseWindow {

public:
	Grid2d grid;
	Simulation sim;

	Demo()
		:
		BaseWindow(800, 600, "Demo!"),
		grid(Simulation::Rs, Simulation::Cs, primitives::P2::quad),
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

		sim.reset();
	}

	void draw_densities(float* d) {
		for (int i = 0; i < grid.rows; i++)
		{
			for (int j = 0; j < grid.cols; j++)
			{	
				const float ds = d[Simulation::IX(j, i)];
				grid.change_color(i, j, glm::vec3(1.f - ds, 1.f, 1.f - ds));
			}
		}
	}

	void update(float dtx) override {
		const auto fw = this->get_width();
		const auto fh = this->get_height();



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

		sim.clear_buffer();

		if (irow > 0 and irow < Simulation::Ny + 1 and icol > 0 and icol < Simulation::Nx + 1) {
		
			if (is_lmb_down()) {
				sim.d0[Simulation::IX(icol, irow)] += 100.f;
			}

			if (is_rmb_down()) {
				//Simulation::p_vy_prev[simulation::IX(icol, irow)] += 1.f;
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
		draw_densities(sim.d);

		grid.update_colors();
		grid.draw(glm::ortho(-0.5f * fw, 0.5f * fw, -0.5f * fh, 0.5f * fh));
	}
};


int main() {
	try {
		// --- 1. Configuration ---
		// !!! IMPORTANT: Replace with your ESP32's serial port name !!!
		std::string port_name = "COM5"; // For Windows
		// std::string port_name = "/dev/ttyUSB0"; // For Linux
		// std::string port_name = "/dev/tty.SLAB_USBtoUART"; // For macOS

		unsigned int baud_rate = 115200; // Must match your ESP32's baud rate

		// --- 2. Setup and Open Port ---
		asio::io_context io;
		asio::serial_port port(io);

		port.open(port_name);

		if (!port.is_open()) {
			std::cerr << "Error: Could not open serial port " << port_name << std::endl;
			return 1;
		}

		// --- 3. Configure Port Settings ---
		port.set_option(asio::serial_port_base::baud_rate(baud_rate));
		port.set_option(asio::serial_port_base::character_size(8));
		port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
		port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
		port.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
		
		std::println("Hello World!");

		// --- 4. Send Data ---
		std::string message_to_send = "Hello, ESP32!\n"; // \n is often important as a delimiter

		asio::write(port, asio::buffer(message_to_send.c_str(), message_to_send.size()));

		std::cout << "Message sent to " << port_name << ": " << message_to_send << std::endl;

		// --- 5. Close Port ---
		port.close();

	}
	catch (const std::exception& e) {
		std::cerr << "An exception occurred: " << e.what() << std::endl;
		return 1;
	}

	Demo window{};
	window.run();


	return 0;
}