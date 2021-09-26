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

#include "../../script/scripts.h"

namespace reone {

namespace di {

class ResourceServices;

class ScriptServices : boost::noncopyable {
public:
    ScriptServices(ResourceServices &resource) : _resource(resource) {
    }

    void init();

    script::Scripts &scripts() { return *_scripts; }

private:
    ResourceServices &_resource;

    std::unique_ptr<script::Scripts> _scripts;
};

} // namespace di

} // namespace reone