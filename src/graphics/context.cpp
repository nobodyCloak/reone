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

#include "context.h"

#include "framebuffer.h"
#include "renderbuffer.h"
#include "texture.h"

using namespace std;

namespace reone {

namespace graphics {

void Context::init() {
    glGetIntegerv(GL_VIEWPORT, &_viewport[0]);
    setBlendMode(BlendMode::Default);
}

void Context::deinit() {
    if (_boundFramebuffer != 0) {
        unbindFramebuffer();
        _boundFramebuffer = 0;
    }
    if (_boundRenderbuffer != 0) {
        unbindRenderbuffer();
        _boundRenderbuffer = 0;
    }
    for (size_t i = 0; i < _boundTextures.size(); ++i) {
        unbindTexture(static_cast<int>(i));
    }
    _boundTextures.clear();
}

void Context::clear(int mask) {
    int glMask = 0;
    if (mask & ClearBuffers::color) {
        glMask |= GL_COLOR_BUFFER_BIT;
    }
    if (mask & ClearBuffers::depth) {
        glMask |= GL_DEPTH_BUFFER_BIT;
    }
    if (mask & ClearBuffers::stencil) {
        glMask |= GL_STENCIL_BUFFER_BIT;
    }
    glClear(glMask);
}

void Context::setViewport(glm::ivec4 viewport) {
    if (_viewport == viewport)
        return;

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    _viewport = move(viewport);
}

void Context::setDepthTestEnabled(bool enabled) {
    if (_depthTest == enabled)
        return;

    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    _depthTest = enabled;
}

void Context::setBackFaceCullingEnabled(bool enabled) {
    if (_backFaceCulling == enabled)
        return;

    if (enabled) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
    _backFaceCulling = enabled;
}

static uint32_t getPolygonModeGL(PolygonMode mode) {
    switch (mode) {
    case PolygonMode::Line:
        return GL_LINE;
    case PolygonMode::Fill:
    default:
        return GL_FILL;
    }
}

void Context::setPolygonMode(PolygonMode mode) {
    if (_polygonMode == mode) {
        return;
    }
    glPolygonMode(GL_FRONT_AND_BACK, getPolygonModeGL(mode));
}

void Context::setBlendMode(BlendMode mode) {
    if (_blendMode == mode)
        return;

    switch (mode) {
    case BlendMode::None:
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
        break;
    case BlendMode::Add:
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
        break;
    case BlendMode::Lighten:
        glBlendEquationSeparate(GL_MAX, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);
        break;
    case BlendMode::Default:
    default:
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);
        break;
    }

    _blendMode = mode;
}

void Context::withScissorTest(const glm::ivec4 &bounds, const function<void()> &block) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(bounds[0], bounds[1], bounds[2], bounds[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    block();

    glDisable(GL_SCISSOR_TEST);
}

void Context::bindFramebuffer(shared_ptr<Framebuffer> framebuffer) {
    if (_boundFramebuffer == framebuffer) {
        return;
    }
    framebuffer->bind();
    _boundFramebuffer = move(framebuffer);
}

void Context::bindRenderbuffer(shared_ptr<Renderbuffer> renderbuffer) {
    if (_boundRenderbuffer == renderbuffer) {
        return;
    }
    renderbuffer->bind();
    _boundRenderbuffer = move(renderbuffer);
}

void Context::bindTexture(int unit, shared_ptr<Texture> texture) {
    size_t numUnits = _boundTextures.size();
    if (numUnits <= unit) {
        _boundTextures.resize(unit + 1);
    }
    if (_boundTextures[unit] == texture) {
        return;
    }
    setActiveTextureUnit(unit);
    texture->bind();
    _boundTextures[unit] = move(texture);
}

void Context::unbindFramebuffer() {
    if (!_boundFramebuffer) {
        return;
    }
    _boundFramebuffer->unbind();
    _boundFramebuffer = 0;
}

void Context::unbindRenderbuffer() {
    if (!_boundRenderbuffer) {
        return;
    }
    _boundRenderbuffer->unbind();
    _boundRenderbuffer = 0;
}

void Context::unbindTexture(int unit) {
    if (!_boundTextures[unit]) {
        return;
    }
    setActiveTextureUnit(unit);
    _boundTextures[unit]->unbind();
    _boundTextures[unit].reset();
}

void Context::setActiveTextureUnit(int unit) {
    if (_textureUnit == unit) {
        return;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    _textureUnit = unit;
}

} // namespace graphics

} // namespace reone