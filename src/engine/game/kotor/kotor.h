
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

#include "../core/game.h"

#include "gui/chargen/chargen.h"
#include "gui/computer.h"
#include "gui/container.h"
#include "gui/conversation.h"
#include "gui/dialog.h"
#include "gui/hud.h"
#include "gui/ingame/ingame.h"
#include "gui/loadscreen.h"
#include "gui/mainmenu.h"
#include "gui/partyselect.h"
#include "gui/saveload.h"

namespace reone {

namespace game {

class KotOR : public Game {
public:
    KotOR(
        boost::filesystem::path path,
        Options options,
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        Cursors &cursors,
        EffectFactory &effectFactory,
        Feats &feats,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        ObjectFactory &objectFactory,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        Skills &skills,
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
        scene::SceneGraph &sceneGraph,
        scene::WorldRenderPipeline &worldRenderPipeline,
        script::Scripts &scripts,
        resource::Resources &resources,
        resource::Strings &strings);

    void initResourceProviders() override;

    void loadModule(const std::string &name, std::string entry = "") override;

    void openMainMenu() override;
    void openSaveLoad(SaveLoadMode mode) override;
    void openInGame();
    void openInGameMenu(InGameMenuTab tab);
    void openContainer(const std::shared_ptr<SpatialObject> &container) override;
    void openPartySelection(const PartySelectionContext &ctx) override;
    void openLevelUp();

    void startCharacterGeneration();
    void startDialog(const std::shared_ptr<SpatialObject> &owner, const std::string &resRef) override;

    void resumeConversation() override;
    void pauseConversation() override;

    void setBarkBubbleText(std::string text, float durartion);

protected:
    void onModuleSelected(const std::string &name) override;
    void drawHUD() override;
    CameraType getConversationCamera(int &cameraId) const override;

    // GUI

    std::unique_ptr<MainMenu> _mainMenu;
    std::unique_ptr<LoadingScreen> _loadScreen;
    std::unique_ptr<CharacterGeneration> _charGen;
    std::unique_ptr<HUD> _hud;
    std::unique_ptr<InGameMenu> _inGame;
    std::unique_ptr<DialogGUI> _dialog;
    std::unique_ptr<ComputerGUI> _computer;
    std::unique_ptr<ContainerGUI> _container;
    std::unique_ptr<PartySelection> _partySelect;
    std::unique_ptr<SaveLoad> _saveLoad;

    Conversation *_conversation {nullptr}; /**< pointer to either DialogGUI or ComputerGUI  */

    void loadMainMenu();
    void loadLoadingScreen();
    void loadCharacterGeneration();
    void loadHUD();
    void loadInGame();
    void loadDialog();
    void loadComputer();
    void loadContainer();
    void loadPartySelection();
    void loadSaveLoad();

    void changeScreen(GameScreen screen);

    gui::GUI *getScreenGUI() const override;

    void withLoadingScreen(const std::string &imageResRef, const std::function<void()> &block);

    // END GUI
};

} // namespace game

} // namespace reone