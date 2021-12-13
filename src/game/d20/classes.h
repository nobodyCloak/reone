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

#include "../../common/memorycache.h"

#include "../types.h"

#include "class.h"

namespace reone {

namespace game {

class IClasses : public MemoryCache<ClassType, CreatureClass> {
public:
    IClasses(std::function<std::shared_ptr<CreatureClass>(ClassType)> compute) :
        MemoryCache(std::move(compute)) {
    }
};

} // namespace game

} // namespace reone