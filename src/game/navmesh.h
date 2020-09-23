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

#include <map>
#include <memory>
#include <set>
#include <queue>

#include "paths.h"

namespace reone {

namespace game {

class NavMesh {
public:
    NavMesh() = default;

    void load(const Paths &paths);

    const std::vector<glm::vec2> findPath(const glm::vec2 &from, const glm::vec2 &to) const;

private:
    struct Edge {
        uint16_t toIndex { 0 };
        float length { 0 };

        Edge(uint16_t toIndex, float length);
    };

    struct FindPathContext {
        std::map<uint16_t, std::pair<uint16_t, float>> fromToDistance;
        std::set<uint16_t> visited;
        std::queue<uint16_t> queue;
    };

    std::vector<glm::vec2> _vertices;
    std::map<uint16_t, std::vector<Edge>> _edges;

    NavMesh(const NavMesh &) = delete;
    NavMesh &operator=(const NavMesh &) = delete;

    uint16_t getNearestVertex(const glm::vec2 &point) const;
    void visit(uint16_t index, FindPathContext &ctx) const;
};

} // namespace game

} // namespace reone
