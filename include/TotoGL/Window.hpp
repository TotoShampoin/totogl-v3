#pragma once

#include "TotoGL/Misc/EventListener.hpp"
#include <GLFW/glfw3.h>
#include <functional>
#include <stdexcept>
#include <string>

namespace TotoGL {

enum class FlagEventName {
    WINDOW_CLOSE,
    WINDOW_ICONIFY,
    WINDOW_MAXIMIZE,
    WINDOW_FOCUS,
    WINDOW_REFRESH,
};
struct FlagEvent {
    bool flag = false;
};

enum class VectorEventName {
    WINDOW_SIZE,
    FRAMEBUFFER_SIZE,
    CONTENT_SCALE_SIZE,
    WINDOW_POSITION,
    CURSOR_POSITION,
    SCROLL,
};
struct VectorEvent {
    double x = 0, y = 0;
};

enum class InputEventName {
    KEY,
    CHARACTER,
    MOUSE_BUTTON,
};
struct InputEvent {
    int button = 0, scancode = 0, action = 0, mods = 0;
};

class Window {
public:
    Window(const int& width, const int& height, const std::basic_string<char>& title) {
        init();
        _glfw_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (!_glfw_window) {
            const char* error;
            glfwGetError(&error);
            throw std::runtime_error(error);
        }
        glfwMakeContextCurrent(_glfw_window);
        glfwSetWindowUserPointer(_glfw_window, this);
        bindEvents();
    }
    Window(const Window&) = delete;
    ~Window() {
        glfwDestroyWindow(_glfw_window);
    }

    void draw(const std::function<void(void)>& callback) {
        glfwMakeContextCurrent(_glfw_window);
        callback();
        glfwSwapBuffers(_glfw_window);
        glfwPollEvents();
    }

    bool shouldClose() { return glfwWindowShouldClose(_glfw_window); }

    GLFWwindow* glfwWindow() const { return _glfw_window; }

    // ? I want to reduce code redundancy here :(

    void on(const FlagEventName& name, const std::function<void(FlagEvent)>& callback) {
        _flag_event_listener.on(name, callback);
    }
    void emit(const FlagEventName& name, const FlagEvent& value) {
        _flag_event_listener.emit(name, value);
    }
    void on(const VectorEventName& name, const std::function<void(VectorEvent)>& callback) {
        _vector_event_listener.on(name, callback);
    }
    void emit(const VectorEventName& name, const VectorEvent& value) {
        _vector_event_listener.emit(name, value);
    }
    void on(const InputEventName& name, const std::function<void(InputEvent)>& callback) {
        _input_event_listener.on(name, callback);
    }
    void emit(const InputEventName& name, const InputEvent& value) {
        _input_event_listener.emit(name, value);
    }

private:
    GLFWwindow* _glfw_window;
    EventListener<FlagEventName, FlagEvent> _flag_event_listener;
    EventListener<VectorEventName, VectorEvent> _vector_event_listener;
    EventListener<InputEventName, InputEvent> _input_event_listener;

    // ? I want to reduce code redundancy here too :(

    void bindEvents() {
        glfwSetWindowCloseCallback(_glfw_window, [](GLFWwindow* glwin) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(FlagEventName::WINDOW_CLOSE, FlagEvent { true });
        });
        glfwSetWindowIconifyCallback(_glfw_window, [](GLFWwindow* glwin, int iconified) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(FlagEventName::WINDOW_ICONIFY, FlagEvent { (bool)iconified });
        });
        glfwSetWindowMaximizeCallback(_glfw_window, [](GLFWwindow* glwin, int maximized) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(FlagEventName::WINDOW_MAXIMIZE, FlagEvent { (bool)maximized });
        });
        glfwSetWindowFocusCallback(_glfw_window, [](GLFWwindow* glwin, int focused) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(FlagEventName::WINDOW_FOCUS, FlagEvent { (bool)focused });
        });
        glfwSetWindowRefreshCallback(_glfw_window, [](GLFWwindow* glwin) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(FlagEventName::WINDOW_REFRESH, FlagEvent { true });
        });

        glfwSetWindowSizeCallback(_glfw_window, [](GLFWwindow* glwin, int width, int height) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(VectorEventName::WINDOW_SIZE, VectorEvent { (double)width, (double)height });
        });
        glfwSetFramebufferSizeCallback(_glfw_window, [](GLFWwindow* glwin, int width, int height) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(VectorEventName::FRAMEBUFFER_SIZE, VectorEvent { (double)width, (double)height });
        });
        glfwSetWindowContentScaleCallback(_glfw_window, [](GLFWwindow* glwin, float xscale, float yscale) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(VectorEventName::CONTENT_SCALE_SIZE, VectorEvent { (double)xscale, (double)yscale });
        });
        glfwSetWindowPosCallback(_glfw_window, [](GLFWwindow* glwin, int xpos, int ypos) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(VectorEventName::WINDOW_POSITION, VectorEvent { (double)xpos, (double)ypos });
        });
        glfwSetCursorPosCallback(_glfw_window, [](GLFWwindow* glwin, double xpos, double ypos) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(VectorEventName::CURSOR_POSITION, VectorEvent { xpos, ypos });
        });
        glfwSetScrollCallback(_glfw_window, [](GLFWwindow* glwin, double xoffset, double yoffset) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(VectorEventName::SCROLL, VectorEvent { xoffset, yoffset });
        });

        glfwSetKeyCallback(_glfw_window, [](GLFWwindow* glwin, int key, int scancode, int action, int mods) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(InputEventName::KEY, InputEvent { key, scancode, action, mods });
        });
        glfwSetCharCallback(_glfw_window, [](GLFWwindow* glwin, unsigned int codepoint) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(InputEventName::CHARACTER, InputEvent { (int)codepoint, 0, 0, 0 });
        });
        glfwSetMouseButtonCallback(_glfw_window, [](GLFWwindow* glwin, int button, int action, int mods) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glwin));
            window->emit(InputEventName::MOUSE_BUTTON, InputEvent { button, 0, action, mods });
        });
    }

    static void init() {
        static bool is_glfw_init = false;
        if (is_glfw_init)
            return;
        if (glfwInit())
            return;
        const char* error;
        glfwGetError(&error);
        throw std::runtime_error(error);
    }
};

} // namespace TotoGL
