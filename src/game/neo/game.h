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

#include "../../graphics/eventhandler.h"

#include "../services.h"
#include "../types.h"

#include "object.h"
#include "object/module.h"

namespace reone {

namespace game {

struct Options;

namespace neo {

class Game : public IObjectIdSequence, public graphics::IEventHandler, boost::noncopyable {
public:
    Game(GameID id, Options &options, ServicesView &services) :
        _id(id),
        _options(options),
        _services(services) {
    }

    void init();

    void run();

    // IObjectIdSequence

    uint32_t nextObjectId() override {
        return _objectIdCounter++;
    }

    // END IObjectIdSequence

    // IEventHandler

    bool handle(const SDL_Event &e) override;

    // END IEventHandler

private:
    GameID _id;
    Options &_options;
    ServicesView &_services;

    bool _finished {false};

    uint32_t _objectIdCounter {2}; // 0 is self, 1 is invalid
    std::unique_ptr<Module> _module;

    void handleInput();
    void update();
    void render();

    void loadModule(const std::string &name);
};

} // namespace neo

} // namespace game

} // namespace reone
