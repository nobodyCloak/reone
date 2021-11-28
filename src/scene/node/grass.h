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

#include "../node.h"

namespace reone {

namespace graphics {

class Texture;

}

namespace scene {

class GrassClusterSceneNode;

class GrassSceneNode : public SceneNode {
public:
    GrassSceneNode(
        glm::vec2 quadSize,
        std::shared_ptr<graphics::Texture> texture,
        std::shared_ptr<graphics::Texture> lightmap,
        SceneGraph &sceneGraph,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders) :
        SceneNode(
            SceneNodeType::Grass,
            sceneGraph,
            context,
            meshes,
            shaders),
        _quadSize(std::move(quadSize)),
        _texture(std::move(texture)),
        _lightmap(lightmap) {
    }

    void drawElements(const std::vector<SceneNode *> &elements, int count) override;

    std::unique_ptr<GrassClusterSceneNode> newCluster();

private:
    glm::vec2 _quadSize {0.0f};
    std::shared_ptr<graphics::Texture> _texture;
    std::shared_ptr<graphics::Texture> _lightmap;
};

} // namespace scene

} // namespace reone
