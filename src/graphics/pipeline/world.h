/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "../framebuffer.h"
#include "../options.h"
#include "../renderbuffer.h"
#include "../texture.h"
#include "../types.h"

namespace reone {

namespace graphics {

class Context;
class IScene;
class Meshes;
class Shaders;

class WorldRenderPipeline : boost::noncopyable {
public:
    WorldRenderPipeline(
        GraphicsOptions options,
        Context &context,
        Meshes &meshes,
        Shaders &shaders) :
        _options(std::move(options)),
        _context(context),
        _meshes(meshes),
        _shaders(shaders) {
    }

    void init();

    void render();

    std::shared_ptr<Texture> screenshot() { return _screenshotColor; }

    void setScene(IScene &scene) { _scene = &scene; }
    void setTakeScreenshot(bool take) { _takeScreenshot = take; }

private:
    GraphicsOptions _options;

    IScene *_scene {nullptr};
    glm::mat4 _shadowLightSpaceMatrices[graphics::kNumCubeFaces];
    bool _takeScreenshot {false}; /**< render next frame into texture */

    // Services

    Context &_context;
    Meshes &_meshes;
    Shaders &_shaders;

    // END Services

    // Framebuffers

    std::shared_ptr<Framebuffer> _geometry1;
    std::shared_ptr<Framebuffer> _geometry2;
    std::shared_ptr<Framebuffer> _verticalBlur;
    std::shared_ptr<Framebuffer> _horizontalBlur;
    std::shared_ptr<Framebuffer> _shadows;
    std::shared_ptr<Framebuffer> _screenshot;

    // END Framebuffers

    // Framebuffer targets

    std::shared_ptr<Renderbuffer> _depthRenderbuffer;
    std::shared_ptr<Renderbuffer> _depthRenderbufferMultisample;
    std::shared_ptr<Texture> _geometry1Color1;
    std::shared_ptr<Texture> _geometry1Color2;
    std::shared_ptr<Texture> _geometry2Color1;
    std::shared_ptr<Texture> _geometry2Color2;
    std::shared_ptr<Texture> _verticalBlurColor;
    std::shared_ptr<Texture> _horizontalBlurColor;
    std::shared_ptr<Texture> _shadowsDepth;
    std::shared_ptr<Texture> _cubeShadowsDepth;
    std::shared_ptr<Texture> _screenshotColor;

    // END Framebuffers targets

    void computeLightSpaceMatrices();
    void drawShadows();
    void drawGeometry();
    void applyHorizontalBlur();
    void applyVerticalBlur();
    void drawResult();
    void saveScreenshot();
};

} // namespace graphics

} // namespace reone