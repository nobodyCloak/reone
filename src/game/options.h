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

#include "../../audio/options.h"
#include "../../common/types.h"
#include "../../graphics/options.h"

namespace reone {

namespace game {

struct GameOptions {
    boost::filesystem::path path;
    bool developer {false};
    bool neo {false};
};

struct OptionsView {
    GameOptions &game;
    graphics::GraphicsOptions &graphics;
    audio::AudioOptions &audio;

    OptionsView(
        GameOptions &game,
        graphics::GraphicsOptions &graphics,
        audio::AudioOptions &audio) :
        game(game),
        graphics(graphics),
        audio(audio) {
    }
};

} // namespace game

} // namespace reone
