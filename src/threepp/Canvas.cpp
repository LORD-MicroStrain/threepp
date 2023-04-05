
#include "threepp/Canvas.hpp"
#include "threepp/loaders/ImageLoader.hpp"


#ifndef CUSTOM_BACKEND
  #define GLFW_INCLUDE_NONE
  #include <GLFW/glfw3.h>
  #include <glad/glad.h>
#endif


using namespace threepp;


#ifndef CUSTOM_BACKEND

    void Canvas::Impl::backend_init_window() 
    {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit()) {
            exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (params.antialiasing_ > 0) {
            glfwWindowHint(GLFW_SAMPLES, params.antialiasing_);
        }

        window = (void*)glfwCreateWindow(params.size_.width, params.size_.height, params.title_.c_str(), nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        {

            ImageLoader imageLoader;
            auto favicon = imageLoader.load("favicon.png", 4);
            if (favicon) {
                GLFWimage images[1];
                images[0] = {static_cast<int>(favicon->width),
                             static_cast<int>(favicon->height),
                             favicon->getData()};
                glfwSetWindowIcon((GLFWwindow*)window, 1, images);
            }
        }

        glfwSetWindowUserPointer((GLFWwindow*) window, this);

        glfwSetKeyCallback((GLFWwindow*) window, key_callback);
        glfwSetMouseButtonCallback((GLFWwindow*) window, mouse_callback);
        glfwSetCursorPosCallback((GLFWwindow*) window, cursor_callback);
        glfwSetScrollCallback((GLFWwindow*) window, scroll_callback);
        glfwSetWindowSizeCallback((GLFWwindow*) window, window_size_callback);

        glfwMakeContextCurrent((GLFWwindow*) window);
        gladLoadGL();
        glfwSwapInterval(params.vsync_ ? 1 : 0);

        if (params.antialiasing_ > 0) {
            glEnable(GL_MULTISAMPLE);
        }

        glEnable(GL_PROGRAM_POINT_SIZE);
    }

    bool Canvas::Impl::backend_should_window_close() 
    {
        return glfwWindowShouldClose((GLFWwindow*) window);
    }
    
    void Canvas::Impl::backend_window_size(WindowSize size) 
    {
        glfwSetWindowSize((GLFWwindow*) window, size.width, size.height);
    }
    double Canvas::Impl::backend_get_time() 
    {
        return glfwGetTime();
    }

    void Canvas::Impl::backend_draw_complete() 
    {
        glfwSwapBuffers((GLFWwindow*) window);
        glfwPollEvents();
    }

    void Canvas::Impl::backend_window_destroy() 
    {
        glfwDestroyWindow((GLFWwindow*) window);
        glfwTerminate();
    }


    static void window_size_callback(GLFWwindow* w, int width, int height) {
        auto p = static_cast<Canvas::Impl*>(glfwGetWindowUserPointer(w));
        p->window_resize(width, height);
    }

    static void error_callback(int error, const char* description) {
        window_error(error, description);
    }

    static void scroll_callback(GLFWwindow* w, double xoffset, double yoffset) {
        auto p = static_cast<Canvas::Impl*>(glfwGetWindowUserPointer(w));

        p->mouse_scroll(xoffset, yoffset);
    }

    static void mouse_callback(GLFWwindow* w, int button, int action, int mods) {
        auto p = static_cast<Canvas::Impl*>(glfwGetWindowUserPointer(w));

        //Remap action
        if (action == GLFW_PRESS)
            action = MOUSE_PRESS_ACTION;
        else if (action == GLFW_RELEASE)
            action = MOUSE_RELEASE_ACTION;

        p->mouse_press(button, action, mods);
     }

    static void cursor_callback(GLFWwindow* w, double xpos, double ypos) {
        auto p = static_cast<Canvas::Impl*>(glfwGetWindowUserPointer(w));
        p->mouse_cursor(xpos, ypos);
    }

    static void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(w, GLFW_TRUE);
            return;
        }

        auto p = static_cast<Canvas::Impl*>(glfwGetWindowUserPointer(w));

           switch (action) {
            case GLFW_PRESS:
                action = KEY_PRESS_ACTION;
                break;
            case GLFW_RELEASE:
                action = KEY_RELEASE_ACTION;
                break;
            case GLFW_REPEAT:
                action = KEY_REPEAT_ACTION;
                break;
            default:
                break;
        }

        p->keyboard_input(key, scancode, action, mods);
    }

#else

void Canvas::Impl::backend_init_window() {}
bool Canvas::Impl::backend_should_window_close() { return false; }
void Canvas::Impl::backend_window_size(WindowSize size) {}
double Canvas::Impl::backend_get_time() { return 0; }
void Canvas::Impl::backend_draw_complete() {}
void Canvas::Impl::backend_window_destroy() {}
#endif

Canvas::Canvas(const Canvas::Parameters& params, Canvas::Impl* imp) {

    if (imp == nullptr)
        imp = new Impl(params);
   
    pimpl_ = std::unique_ptr<Impl>(new Impl(params));
}


int threepp::Canvas::getFPS() const {
    return pimpl_->fps_;
}

void Canvas::animate(const std::function<void()>& f) {

    pimpl_->animate(f);
}

void Canvas::animate(const std::function<void(float)>& f) {

    pimpl_->animate(f);
}

void Canvas::animate(const std::function<void(float, float)>& f) {

    pimpl_->animate(f);
}

const WindowSize& Canvas::getSize() const {

    return pimpl_->getSize();
}

float Canvas::getAspect() const {

    return getSize().getAspect();
}

void Canvas::setSize(WindowSize size) {

    pimpl_->setSize(size);
}

void Canvas::onWindowResize(std::function<void(WindowSize)> f) {

    pimpl_->onWindowResize(std::move(f));
}

void Canvas::addKeyListener(KeyListener* listener) {

    pimpl_->addKeyListener(listener);
}

bool Canvas::removeKeyListener(const KeyListener* listener) {

    return pimpl_->removeKeyListener(listener);
}

void Canvas::addMouseListener(MouseListener* listener) {

    pimpl_->addMouseListener(listener);
}

bool Canvas::removeMouseListener(const MouseListener* listener) {

    return pimpl_->removeMouseListener(listener);
}

void Canvas::invokeLater(const std::function<void()>& f, float t) {
    pimpl_->invokeLater(f, t);
}

void* Canvas::window_ptr() const {

    return pimpl_->window;
}

Canvas::~Canvas() = default;


Canvas::Parameters& Canvas::Parameters::title(std::string value) {

    this->title_ = std::move(value);

    return *this;
}

Canvas::Parameters& Canvas::Parameters::size(WindowSize size) {

    this->size_ = size;

    return *this;
}

Canvas::Parameters& Canvas::Parameters::size(int width, int height) {

    return this->size({width, height});
}

Canvas::Parameters& Canvas::Parameters::antialiasing(int antialiasing) {

    this->antialiasing_ = antialiasing;

    return *this;
}

Canvas::Parameters& Canvas::Parameters::vsync(bool flag) {

    this->vsync_ = flag;

    return *this;
}
