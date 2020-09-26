/*
 * Copyright � 2020 Vsevolod Kremianskii
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

#include "mesh.h"

namespace reone {

namespace render {

class GUIQuad : public Mesh {
public:
    static GUIQuad &getDefault();
    static GUIQuad &getXFlipped();
    static GUIQuad &getYFlipped();
    static GUIQuad &getXYFlipped();

private:
    GUIQuad(std::vector<float> &&vertices);
};

#define DefaultGuiQuad render::GUIQuad::getDefault()
#define FlipXGuiQuad render::GUIQuad::getXFlipped()
#define FlipYGuiQuad render::GUIQuad::getYFlipped()
#define FlipXYGuiQuad render::GUIQuad::getXYFlipped()

} // namespace render

} // namespace reone
