#pragma once
#include <cstdint>
#include <string_view>
#include <print>
#include <cassert>
#include <chrono>
#include <ranges>


#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "gl_error.h"

namespace wnd
{
	class BaseWindow
	{
	public:
		BaseWindow(std::uint16_t width, std::uint16_t height, std::string_view title);
		virtual ~BaseWindow();

		void run();

		virtual void update(float dt);

		float get_width()const noexcept;
		float get_height()const noexcept;

		float get_cursor_x()const noexcept;
		float get_cursor_y()const noexcept;

		bool is_lmb_down()const noexcept;
		bool is_rmb_down()const noexcept;


		void set_window_size(std::uint16_t width, std::uint16_t height) noexcept;
	protected:
		std::uint16_t width;
		std::uint16_t height;

		float cursor_pos_x{};
		float cursor_pos_y{};

		bool lmb_down{};
		bool rmb_down{};

		GLFWwindow* p_wnd;
	};

	void clear_screen(float r, float g, float b, float a = 1.f);
}