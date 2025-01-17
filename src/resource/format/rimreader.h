/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../id.h"

#include "binreader.h"

namespace reone {

namespace resource {

class RimReader : public BinaryResourceReader {
public:
    struct ResourceEntry {
        ResourceId resId;
        uint32_t offset {0};
        uint32_t size {0};
    };

    const std::vector<ResourceEntry> &resources() const { return _resources; }

private:
    int _id;

    int _numResources {0};
    uint32_t _offResources {0};

    std::vector<ResourceEntry> _resources;

    void onLoad() override;

    void loadResources();

    ResourceEntry readResource();
};

} // namespace resource

} // namespace reone
