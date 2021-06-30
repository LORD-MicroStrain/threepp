// https://github.com/mrdoob/three.js/blob/r129/src/renderers/webgl/WebGLUniforms.js

#ifndef THREEPP_GLUNIFORMS_HPP
#define THREEPP_GLUNIFORMS_HPP

#include <glad/glad.h>

#include <any>
#include <vector>
#include <unordered_map>

namespace threepp::gl {

    struct StructuredUniform {

        struct UniformObject {

        };

        StructuredUniform(const GLuint id): id(id){}


    private:
        GLuint id;

    private:
        std::vector<UniformObject> seq;
        std::unordered_map<std::string, UniformObject> map;

    };

    struct GLUniforms {

    };

}

#endif//THREEPP_GLUNIFORMS_HPP
