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

#include "../../common/singleton.h"

#include "mesh.h"

namespace reone {

namespace render {

class AABBMesh : public Mesh, public Singleton {
public:
    static AABBMesh &instance();
    void render(const AABB &aabb, const glm::mat4 &transform) const;

private:
    AABBMesh();
};

} // namespace render

} // namespace reone
