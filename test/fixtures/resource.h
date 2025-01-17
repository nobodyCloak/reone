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

#include "../../resource/provider.h"

namespace reone {

namespace resource {

class MockResourceProvider : public IResourceProvider {
public:
    MockResourceProvider(int id) : _id(id) {
    }

    void add(ResourceId id, std::shared_ptr<ByteArray> res) {
        _resources[id] = std::move(res);
    }

    std::shared_ptr<ByteArray> find(const ResourceId &id) override {
    }

    int id() const override {
        return _id;
    }

private:
    int _id;
    std::map<ResourceId, std::shared_ptr<ByteArray>, ResourceIdHasher> _resources;
};

} // namespace resource

} // namespace reone
