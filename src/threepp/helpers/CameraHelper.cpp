
#include "threepp/helpers/CameraHelper.hpp"

#include "threepp/cameras/PerspectiveCamera.hpp"
#include "threepp/materials/LineBasicMaterial.hpp"

using namespace threepp;


CameraHelper::CameraHelper(const std::shared_ptr<Camera>& camera)
    : LineSegments(BufferGeometry::create(), LineBasicMaterial::create()), camera(camera) {

    auto m = material_->as<LineBasicMaterial>();
    m->toneMapped = false;
    m->vertexColors = true;
    m->color = 0xffffff;

    std::vector<float> vertices;
    std::vector<float> colors;

    // colors

    const Color colorFrustum(0xffaa00);
    const Color colorCone(0xff0000);
    const Color colorUp(0x00aaff);
    const Color colorTarget(0xffffff);
    const Color colorCross(0x333333);

    auto addPoint = [&](const std::string& id, const Color& color) {
        vertices.insert(vertices.end(), {0, 0, 0});
        colors.insert(colors.end(), {color.r, color.g, color.b});

        pointMap[id].emplace_back(vertices.size() / 3 - 1);
    };

    auto addLine = [&](const std::string& a, const std::string& b, const Color& color) {
        addPoint(a, color);
        addPoint(b, color);
    };


    // near

    addLine("n1", "n2", colorFrustum);
    addLine("n2", "n4", colorFrustum);
    addLine("n4", "n3", colorFrustum);
    addLine("n3", "n1", colorFrustum);

    // far

    addLine("f1", "f2", colorFrustum);
    addLine("f2", "f4", colorFrustum);
    addLine("f4", "f3", colorFrustum);
    addLine("f3", "f1", colorFrustum);

    // sides

    addLine("n1", "f1", colorFrustum);
    addLine("n2", "f2", colorFrustum);
    addLine("n3", "f3", colorFrustum);
    addLine("n4", "f4", colorFrustum);

    // cone

    addLine("p", "n1", colorCone);
    addLine("p", "n2", colorCone);
    addLine("p", "n3", colorCone);
    addLine("p", "n4", colorCone);

    // up

    addLine("u1", "u2", colorUp);
    addLine("u2", "u3", colorUp);
    addLine("u3", "u1", colorUp);

    // target

    addLine("c", "t", colorTarget);
    addLine("p", "c", colorCross);

    // cross

    addLine("cn1", "cn2", colorCross);
    addLine("cn3", "cn4", colorCross);

    addLine("cf1", "cf2", colorCross);
    addLine("cf3", "cf4", colorCross);

    geometry_->setAttribute("position", FloatBufferAttribute::create(vertices, 3));
    geometry_->setAttribute("color", FloatBufferAttribute::create(colors, 3));

    camera->updateProjectionMatrix();

    this->matrix = camera->matrixWorld;
    this->matrixAutoUpdate = false;

    update();
}

std::shared_ptr<CameraHelper> CameraHelper::create(const std::shared_ptr<Camera>& camera) {

    return std::shared_ptr<CameraHelper>(new CameraHelper(camera));
}

void CameraHelper::update() {

    float w = 1, h = 1;

    // we need just camera projection matrix inverse
    // world matrix must be identity

    _camera.projectionMatrixInverse.copy(this->camera->projectionMatrixInverse);

    // center / target

    setPoint("c", 0, 0, -1);
    setPoint("t", 0, 0, 1);

    // near

    setPoint("n1", -w, -h, -1);
    setPoint("n2", w, -h, -1);
    setPoint("n3", -w, h, -1);
    setPoint("n4", w, h, -1);

    // far

    setPoint("f1", -w, -h, 1);
    setPoint("f2", w, -h, 1);
    setPoint("f3", -w, h, 1);
    setPoint("f4", w, h, 1);

    // up

    setPoint("u1", w * 0.7f, h * 1.1f, -1);
    setPoint("u2", -w * 0.7f, h * 1.1f, -1);
    setPoint("u3", 0, h * 2, -1);

    // cross

    setPoint("cf1", -w, 0, 1);
    setPoint("cf2", w, 0, 1);
    setPoint("cf3", 0, -h, 1);
    setPoint("cf4", 0, h, 1);

    setPoint("cn1", -w, 0, -1);
    setPoint("cn2", w, 0, -1);
    setPoint("cn3", 0, -h, -1);
    setPoint("cn4", 0, h, -1);

    geometry()->getAttribute<float>("position")->needsUpdate();
}

void CameraHelper::setPoint(const std::string& point, float x, float y, float z) {

    Vector3 _vector;
    _vector.set(x, y, z).unproject(_camera);

    if (pointMap.count(point)) {
        auto& points = pointMap.at(point);
        auto position = geometry()->getAttribute<float>("position");

        for (unsigned p : points) {

            position->setXYZ(p, _vector.x, _vector.y, _vector.z);
        }
    }
}
