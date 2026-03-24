#pragma once

namespace ngin {
	struct Event {};
}
struct GLFWwindow;

namespace ngin {
	class Application {
	public:
		Application();
		virtual ~Application();

		virtual void on_create() = 0;
		virtual void on_destroy() = 0;

		virtual void on_update(float delta_time) = 0;
		virtual void on_event(Event& e) = 0;

		void run();

		static Application* get_instance();

	protected:
		bool m_running;
		GLFWwindow* m_window;
	};
}