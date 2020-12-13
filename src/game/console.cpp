/*
 * Copyright (c) 2020 The reone project contributors
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

#include "console.h"

#include "glm/ext.hpp"

#include "../common/log.h"
#include "../render/font.h"
#include "../render/fonts.h"
#include "../render/mesh/quad.h"
#include "../render/shaders.h"
#include "../resource/resources.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;

namespace reone {

namespace game {

constexpr int kMaxOutputLineCount = 50;
constexpr int kVisibleLineCount = 15;

Console::Console(const GraphicsOptions &opts) : _opts(opts), _input(kTextInputConsole) {
}

void Console::load() {
    _font = Fonts::instance().get("fnt_console");
}

bool Console::handle(const SDL_Event &event) {
    if (_open && _input.handle(event)) return true;

    switch (event.type) {
        case SDL_MOUSEWHEEL:
            return handleMouseWheel(event.wheel);
        case SDL_KEYUP:
            return handleKeyUp(event.key);
        default:
            return false;
    }
}

bool Console::handleMouseWheel(const SDL_MouseWheelEvent &event) {
    bool up = event.y < 0;
    if (up) {
        if (_outputOffset > 0) {
            --_outputOffset;
        }
    } else {
        if (_outputOffset < static_cast<int>(_output.size()) - kVisibleLineCount + 1) {
            ++_outputOffset;
        }
    }
    return true;
}

bool Console::handleKeyUp(const SDL_KeyboardEvent &event) {
    if (_open) {
        switch (event.keysym.sym) {
            case SDLK_BACKQUOTE:
                _open = false;
                return true;

            case SDLK_RETURN: {
                string text(_input.text());
                if (!text.empty()) {
                    executeInputText();
                    _input.clear();
                }
                return true;
            }
            default:
                return false;
        }
    } else {
        switch (event.keysym.sym) {
            case SDLK_BACKQUOTE:
                _open = true;
                return true;

            default:
                return false;
        }
    }
}

void Console::executeInputText() {
    debug(boost::format("Console: execute \"%s\"") % _input.text());
    _output.push_front(_input.text());
    trimOutput();
}

void Console::trimOutput() {
    for (int i = static_cast<int>(_output.size()) - kMaxOutputLineCount; i > 0; --i) {
        _output.pop_back();
    }
}

void Console::render() const {
    drawBackground();
    drawLines();
}

void Console::drawBackground() const {
    float height = kVisibleLineCount * _font->height();

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(_opts.width, height, 1.0f));

    LocalUniforms locals;
    locals.general.model = move(transform);
    locals.general.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    locals.general.alpha = 0.5f;

    Shaders::instance().activate(ShaderProgram::GUIWhite, locals);

    Quad::getDefault().renderTriangles();
}

void Console::drawLines() const {
    float height = kVisibleLineCount * _font->height();

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(3.0f, height - 0.5f * _font->height(), 0.0f));

    // Input

    string text("> " + _input.text());
    _font->render(text, transform, glm::vec3(1.0f), TextGravity::Right);

    // Output

    for (int i = 0; i < kVisibleLineCount - 1 && i < static_cast<int>(_output.size()) - _outputOffset; ++i) {
        const string &line = _output[i + _outputOffset];
        transform = glm::translate(transform, glm::vec3(0.0f, -_font->height(), 0.0f));
        _font->render(line, transform, glm::vec3(1.0f), TextGravity::Right);
    }
}

bool Console::isOpen() const {
    return _open;
}

} // namespace game

} // namespace reone
