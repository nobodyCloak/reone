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

#include "../object.h"

#include "area.h"
#include "creature.h"

namespace reone {

namespace resource {

class GffStruct;

}

namespace game {

struct ServicesView;

namespace neo {

class Module : public Object {
public:
    class Builder : public Object::Builder<Module, Builder> {
    public:
        Builder &area(std::shared_ptr<Area> area) {
            _area = std::move(area);
            return *this;
        }

        Builder &pc(std::shared_ptr<Creature> pc) {
            _pc = std::move(pc);
            return *this;
        }

        std::unique_ptr<Module> build() override {
            return std::make_unique<Module>(_id, _tag, _area, _pc);
        }

    private:
        std::shared_ptr<Area> _area;
        std::shared_ptr<Creature> _pc;
    };

    class Loader : boost::noncopyable {
    public:
        Loader(IObjectIdSequence &idSeq, ServicesView &services) :
            _idSeq(idSeq),
            _services(services) {
        }

        std::unique_ptr<Module> load(const std::string &name);

    private:
        IObjectIdSequence &_idSeq;
        ServicesView &_services;
    };

    Module(
        uint32_t id,
        std::string tag,
        std::shared_ptr<Area> area,
        std::shared_ptr<Creature> pc) :
        Object(
            id,
            ObjectType::Module,
            std::move(tag),
            nullptr),
        _area(std::move(area)),
        _pc(std::move(pc)) {
    }

    Area &area() const {
        return *_area;
    }

    Creature &pc() const {
        return *_pc;
    }

private:
    std::shared_ptr<Area> _area;
    std::shared_ptr<Creature> _pc;
};

} // namespace neo

} // namespace game

} // namespace reone