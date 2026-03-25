#pragma once

struct GLFWwindow;

namespace ngin {
	class Application {
	public:
		Application();
		virtual ~Application();

		virtual void on_create() = 0;
		virtual void on_destroy() = 0;

		virtual void on_update(float delta_time) = 0;

		void run();

		static Application* get_instance();
		GLFWwindow* get_native_window_handle() const { return m_native_window_handle; }

	protected:
		void set_native_window_handle(GLFWwindow* handle) { m_native_window_handle = handle; }
		bool m_running;
		GLFWwindow* m_native_window_handle;
	};
}