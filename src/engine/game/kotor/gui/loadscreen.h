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

#include "../../../gui/control/label.h"
#include "../../../gui/control/progressbar.h"

#include "../../core/gui/loadscreen.h"

#include "gui.h"

namespace reone {

namespace game {

class LoadingScreen : public GameGUI, public ILoadingScreen {
public:
    LoadingScreen(
        KotOR *game,
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        Feats &feats,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        ObjectFactory &objectFactory,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Features &features,
        graphics::Fonts &fonts,
        graphics::Lips &lips,
        graphics::Materials &materials,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::PBRIBL &pbrIbl,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Walkmeshes &walkmeshes,
        graphics::Window &window,
        resource::Resources &resources,
        resource::Strings &strings,
        resource::TwoDas &twoDas);

    void load() override;

    void setImage(const std::string &resRef) override;
    void setProgress(int progress) override;

private:
    struct Binding {
        std::shared_ptr<gui::ProgressBar> pbProgress;
        std::shared_ptr<gui::Label> lblHint;
        std::shared_ptr<gui::Label> lblLogo;
        std::shared_ptr<gui::Label> lblLoading;
    } _binding;

    void bindControls();
};

} // namespace game

} // namespace reone
