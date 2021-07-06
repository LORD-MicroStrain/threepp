// https://github.com/mrdoob/three.js/blob/r129/src/cameras/Camera.js

#ifndef THREEPP_CAMERA_HPP
#define THREEPP_CAMERA_HPP

#include "threepp/cameras/View.hpp"

#include "threepp/core/Object3D.hpp"
#include "threepp/math/Matrix4.hpp"

namespace threepp {

    class Camera : public Object3D {

    public:
        float zoom = 1;

        float near;
        float far;

        std::optional<View> view;

        Camera(const Camera &) = delete;

        Matrix4 matrixWorldInverse{};

        Matrix4 projectionMatrix{};
        Matrix4 projectionMatrixInverse{};

        void getWorldDirection(Vector3 &target) override {

            this->updateWorldMatrix(true, false);

            const auto &e = this->matrixWorld.elements;

            target.set(-e[8], -e[9], -e[10]).normalize();
        }

        void updateMatrixWorld(bool force = false) override {

            Object3D::updateMatrixWorld(force);

            this->matrixWorldInverse.copy(this->matrixWorld).invert();
        }

        void updateWorldMatrix(bool updateParents, bool updateChildren) override {

            Object3D::updateWorldMatrix(updateParents, updateChildren);

            this->matrixWorldInverse.copy(this->matrixWorld).invert();
        }

        std::string type() const override {

            return "Camera";
        }

        virtual void updateProjectionMatrix() = 0;

    protected:
        Camera(float near, float far) : near(near), far(far){};
    };

}// namespace threepp

#endif//THREEPP_CAMERA_HPP
