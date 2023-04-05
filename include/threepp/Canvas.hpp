
#ifndef THREEPP_CANVAS_HPP
#define THREEPP_CANVAS_HPP

#include "threepp/input/KeyListener.hpp"
#include "threepp/input/MouseListener.hpp"
#include "threepp/core/Clock.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <optional>
#include <queue>
#include <iostream>


namespace threepp {

    struct WindowSize {
        int width;
        int height;

        [[nodiscard]] float getAspect() const {

            return static_cast<float>(width) / static_cast<float>(height);
        }
    };


     namespace {

        typedef std::pair<std::function<void()>, float> task;

        struct CustomComparator {
            bool operator()(const task& l, const task& r) const { return l.second > r.second; }
        };

    }// namespace


    class Canvas {

    public:
        struct Impl;
        struct Parameters;

        typedef std::variant<bool, int, WindowSize> ParameterValue;

        explicit Canvas(const Parameters& params = Parameters(), Canvas::Impl* impl = nullptr);

        explicit Canvas(const std::string& name);

        Canvas(const std::string& name, const std::unordered_map<std::string, ParameterValue>& values);

        [[nodiscard]] const WindowSize& getSize() const;

        [[nodiscard]] float getAspect() const;

        [[nodiscard]] int getFPS() const;

        void setSize(WindowSize size);

        void onWindowResize(std::function<void(WindowSize)> f);

        void addKeyListener(KeyListener* listener);

        bool removeKeyListener(const KeyListener* listener);

        void addMouseListener(MouseListener* listener);

        bool removeMouseListener(const MouseListener* listener);

        void animate(const std::function<void()>& f);

        void animate(const std::function<void(float)>& f);

        void animate(const std::function<void(float, float)>& f);

        void invokeLater(const std::function<void()>& f, float t = 0);

        [[nodiscard]] void* window_ptr() const;

        ~Canvas();


    private:
        std::unique_ptr<Impl> pimpl_;

    public:
        struct Parameters {

            Parameters();

            explicit Parameters(const std::unordered_map<std::string, ParameterValue>& values);

            Parameters& title(std::string value);

            Parameters& size(WindowSize size);

            Parameters& size(int width, int height);

            Parameters& antialiasing(int antialiasing);

            Parameters& vsync(bool flag);

        private:
            WindowSize size_{640, 480};
            int antialiasing_{0};
            std::string title_{"threpp"};
            bool vsync_{true};

            friend struct Canvas::Impl;
        };

        
        struct Impl {

            static const int KEY_ESCAPE = 1;
            static const int KEY_PRESS_ACTION = 1;
            static const int KEY_RELEASE_ACTION = 2;
            static const int KEY_REPEAT_ACTION = 3;

            static const int MOUSE_PRESS_ACTION = 1;
            static const int MOUSE_RELEASE_ACTION = 2;

            void* window;

            int fps_ = -1;

            WindowSize size_;
            Vector2 lastMousePos_;

            std::priority_queue<task, std::vector<task>, CustomComparator> tasks_;
            std::optional<std::function<void(WindowSize)>> resizeListener;
            std::vector<KeyListener*> keyListeners;
            std::vector<MouseListener*> mouseListeners;

            virtual void backend_init_window(const Canvas::Parameters& params);
            virtual bool backend_should_window_close();
            virtual void backend_window_size(WindowSize size);
            virtual double backend_get_time();
            virtual void backend_draw_complete();
            virtual void backend_window_destroy();

            explicit Impl(const Canvas::Parameters& params): size_(params.size_) {
                backend_init_window(params);
            }

            [[nodiscard]] const WindowSize& getSize() const {
                return size_;
            }

            void setSize(WindowSize size) {

                size_.height = size.height;
                backend_window_size(size);
            }

            // http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/
            inline void measureFPS(double& lastTime, int& nbFrames) {

                double currentTime = backend_get_time();

                nbFrames++;
                if (currentTime - lastTime >= 1.0) {
                    fps_ = nbFrames;
                    nbFrames = 0;
                    lastTime += 1.0;
                }
            }

            inline void handleTasks() {
                while (!tasks_.empty()) {
                    auto& task = tasks_.top();
                    double currentTime = backend_get_time();

                    if (task.second < currentTime) {
                        task.first();
                        tasks_.pop();
                    } else {
                        break;
                    }
                }
            }

            void animate(const std::function<void()>& f) {
                double lastTime = backend_get_time();
                int nbFrames = 0;

                while (!backend_should_window_close()) {

                    measureFPS(lastTime, nbFrames);

                    handleTasks();

                    f();

                    backend_draw_complete();
                }
            }

            void animate(const std::function<void(float)>& f) {

                double lastTime = backend_get_time();
                int nbFrames = 0;
                Clock clock;

                while (!backend_should_window_close()) {

                    measureFPS(lastTime, nbFrames);

                    handleTasks();

                    f(clock.getDelta());

                    backend_draw_complete();
                }
            }

            void animate(const std::function<void(float, float)>& f) {

                double lastTime = backend_get_time();
                int nbFrames = 0;
                Clock clock;
                while (!backend_should_window_close()) {
                    double currTime = backend_get_time();
                    measureFPS(lastTime, nbFrames);

                    handleTasks();

                    f(static_cast<float>(currTime), clock.getDelta());

                    backend_draw_complete();
                }
            }

            void onWindowResize(std::function<void(WindowSize)> f) {
                this->resizeListener = std::move(f);
            }

            void addKeyListener(KeyListener* listener) {
                auto find = std::find(keyListeners.begin(), keyListeners.end(), listener);
                if (find == keyListeners.end()) {
                    keyListeners.emplace_back(listener);
                }
            }

            bool removeKeyListener(const KeyListener* listener) {
                auto find = std::find(keyListeners.begin(), keyListeners.end(), listener);
                if (find != keyListeners.end()) {
                    keyListeners.erase(find);
                    return true;
                }
                return false;
            }

            void addMouseListener(MouseListener* listener) {
                auto find = std::find(mouseListeners.begin(), mouseListeners.end(), listener);
                if (find == mouseListeners.end()) {
                    mouseListeners.emplace_back(listener);
                }
            }

            bool removeMouseListener(const MouseListener* listener) {
                auto find = std::find(mouseListeners.begin(), mouseListeners.end(), listener);
                if (find != mouseListeners.end()) {
                    mouseListeners.erase(find);
                    return true;
                }
                return false;
            }

            void invokeLater(const std::function<void()>& f, float t) {
                double currTime = backend_get_time();
                tasks_.emplace(f, static_cast<float>(currTime) + t);
            }


            void window_resize(int width, int height) {
                size_ = {width, height};
                if (resizeListener) resizeListener.value().operator()(size_);
            }

            void window_error(int error, const char* description) {
                std::cerr << "Error: " << description << std::endl;
            }

            void mouse_scroll(double xoffset, double yoffset) {
                auto listeners = mouseListeners;
                if (listeners.empty()) return;
                Vector2 delta{(float) xoffset, (float) yoffset};
                for (auto l : listeners) {
                    l->onMouseWheel(delta);
                }
            }

            void mouse_press(int button, int action, int mods) {
                auto listeners = mouseListeners;
                for (auto l : listeners) {

                    switch (action) {
                        case MOUSE_PRESS_ACTION:
                            l->onMouseDown(button, lastMousePos_);
                            break;
                        case MOUSE_RELEASE_ACTION:
                            l->onMouseUp(button, lastMousePos_);
                            break;
                        default:
                            break;
                    }
                }
            }

            void mouse_cursor(double xpos, double ypos) {
                lastMousePos_.set(static_cast<float>(xpos), static_cast<float>(ypos));
                auto listeners = mouseListeners;
                for (auto l : listeners) {
                    l->onMouseMove(lastMousePos_);
                }
            }

            void keyboard_input(int key, int scancode, int action, int mods) {
                if (key == KEY_ESCAPE && action == KEY_PRESS_ACTION) {
                    return;
                }

                if (keyListeners.empty()) return;

                KeyEvent evt{key, scancode, mods};
                auto listeners = keyListeners;
                for (auto l : listeners) {
                    switch (action) {
                        case KEY_PRESS_ACTION:
                            l->onKeyPressed(evt);
                            break;
                        case KEY_RELEASE_ACTION:
                            l->onKeyReleased(evt);
                            break;
                        case KEY_REPEAT_ACTION:
                            l->onKeyRepeat(evt);
                            break;
                        default:
                            break;
                    }
                }
            }

            ~Impl() {
                backend_window_destroy();
            }

    };
};
}// namespace threepp

#endif//THREEPP_CANVAS_HPP
