// https://github.com/mrdoob/three.js/blob/r129/src/renderers/WebGLRenderTarget.js

#ifndef THREEPPGLRENDERTARGETHPP
#define THREEPPGLRENDERTARGETHPP

#include "threepp/core/EventDispatcher.hpp"

#include "threepp/textures/Texture.hpp"

#include "threepp/math/Vector4.hpp"

#include <optional>

namespace threepp {

    class GLRenderTarget : public EventDispatcher {

        struct Options {

            int mapping;
            int wrapS;
            int wrapT;
            int magFilter;
            int minFilter = LinearFilter;
            int type;
            int anisotropy;
            int encoding;

            bool generateMipmaps = false;
            bool depthBuffer = true;
            bool stencilBuffer = false;
            std::optional<Texture> depthTexture;
        };

    public:
        unsigned int width;
        unsigned int height;
        unsigned int depth = 1;

        Vector4 scissor;
        bool scissorTest = false;

        Vector4 viewport;

        Texture texture;

        bool depthBuffer;
        bool stencilBuffer;
        std::optional<Texture> depthTexture;

        GLRenderTarget(unsigned int width, unsigned int height, const Options &options)
            : width(width), height(height),
              scissor(0.f, 0.f, (float) width, (float) height),
              viewport(0.f, 0.f, (float) width, (float) height),
              depthBuffer(options.depthBuffer), stencilBuffer(options.stencilBuffer), depthTexture(options.depthTexture),
              texture(std::nullopt, options.mapping, options.wrapS, options.wrapT, options.magFilter, options.minFilter, options.type, options.anisotropy, options.encoding) {
        }

        void setTexture(Texture &texture) {

            texture.image = Image{width, height, depth};

            this->texture = texture;
        }

        void setSize(unsigned int width, unsigned int height, unsigned int depth = 1) {

            if (this->width != width || this->height != height || this->depth != depth) {

                this->width = width;
                this->height = height;
                this->depth = depth;

                this->texture.image->width = width;
                this->texture.image->height = height;
                this->texture.image->depth = depth;

                this->dispose();
            }

            this->viewport.set(0, 0, (float) width, (float) height);
            this->scissor.set(0, 0, (float) width, (float) height);
        }

        GLRenderTarget &copy(const GLRenderTarget &source) {

            this->width = source.width;
            this->height = source.height;
            this->depth = source.depth;

            this->viewport.copy(source.viewport);

            this->texture = source.texture;
            //                this->texture.image = { ...this->texture.image }; // See #20328.

            this->depthBuffer = source.depthBuffer;
            this->stencilBuffer = source.stencilBuffer;
            this->depthTexture = source.depthTexture;

            return *this;
        }

        void dispose() {

            this->dispatchEvent("dispose");
        }
    };

}// namespace threepp

#endif//THREEPPGLRENDERTARGETHPP
