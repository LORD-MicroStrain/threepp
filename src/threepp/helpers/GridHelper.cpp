
#include "threepp/helpers/GridHelper.hpp"

#include "threepp/materials/LineBasicMaterial.hpp"

using namespace threepp;


GridHelper::GridHelper(unsigned int size, unsigned int divisions, const Color& color1, const Color& color2)
        : LineSegments(nullptr, nullptr) {

    const auto center = divisions / 2;
    const auto step = static_cast<float>(size) / static_cast<float>(divisions);
    const auto halfSize = static_cast<float>(size) / 2;

    std::vector<float> vertices;
    std::vector<float> colors((divisions + 1) * 12);

    int j = 0;
    float k = -halfSize;
    for (int i = 0; i <= divisions; i++) {

        vertices.insert(vertices.end(), {-halfSize, 0, k, halfSize, 0, k});
        vertices.insert(vertices.end(), {k, 0, -halfSize, k, 0, halfSize});

        auto& color = (i == center ? color1 : color2);

        color.toArray(colors, j);
        j += 3;
        color.toArray(colors, j);
        j += 3;
        color.toArray(colors, j);
        j += 3;
        color.toArray(colors, j);
        j += 3;

        k += step;
    }

    auto geometry = BufferGeometry::create();
    geometry->setAttribute("position", FloatBufferAttribute::create(vertices, 3));
    geometry->setAttribute("color", FloatBufferAttribute::create(colors, 3));

    auto material = LineBasicMaterial::create();
    material->vertexColors = true;
    material->toneMapped = false;

    material_ = material;
    geometry_ = geometry;
}

std::shared_ptr<GridHelper> GridHelper::create(unsigned int size, unsigned int divisions, const Color& color1, const Color& color2) {

    return std::shared_ptr<GridHelper>(new GridHelper(size, divisions, color1, color2));
}

GridHelper::~GridHelper() {

    this->geometry_->dispose();
    this->material_->dispose();
}
