
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

#include <cstdint>
#include <functional>
#include <map>
#include <set>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

#include "SDL2/SDL_events.h"

#include "../audio/files.h"
#include "../audio/player.h"
#include "../audio/soundhandle.h"
#include "../graphics/eventhandler.h"
#include "../graphics/fonts.h"
#include "../graphics/lip/lips.h"
#include "../graphics/mesh/meshes.h"
#include "../graphics/model/models.h"
#include "../graphics/pbribl.h"
#include "../graphics/shader/shaders.h"
#include "../graphics/texture/textures.h"
#include "../graphics/walkmesh/walkmeshes.h"
#include "../graphics/window.h"
#include "../resource/resources.h"
#include "../resource/strings.h"
#include "../scene/pipeline/world.h"
#include "../scene/scenegraph.h"
#include "../script/scripts.h"
#include "../video/video.h"

#include "console.h"
#include "combat/combat.h"
#include "cursors.h"
#include "d20/classes.h"
#include "d20/feats.h"
#include "d20/spells.h"
#include "footstepsounds.h"
#include "gui/chargen/chargen.h"
#include "gui/container.h"
#include "gui/computer.h"
#include "gui/dialog.h"
#include "gui/hud.h"
#include "gui/ingame/ingame.h"
#include "gui/loadscreen.h"
#include "gui/mainmenu.h"
#include "gui/partyselect.h"
#include "gui/profileoverlay.h"
#include "gui/saveload.h"
#include "gui/sounds.h"
#include "object/module.h"
#include "object/objectfactory.h"
#include "object/spatial.h"
#include "options.h"
#include "party.h"
#include "portraits.h"
#include "reputes.h"
#include "savedgame.h"
#include "script/routines.h"
#include "script/runner.h"
#include "soundsets.h"
#include "surfaces.h"
#include "types.h"

namespace reone {

namespace game {

constexpr char kKeyFilename[] = "chitin.key";
constexpr char kTexturePackDirectoryName[] = "texturepacks";
constexpr char kGUITexturePackFilename[] = "swpc_tex_gui.erf";
constexpr char kTexturePackFilename[] = "swpc_tex_tpa.erf";
constexpr char kMusicDirectoryName[] = "streammusic";
constexpr char kSoundsDirectoryName[] = "streamsounds";
constexpr char kLipsDirectoryName[] = "lips";
constexpr char kOverrideDirectoryName[] = "override";

/**
 * Entry point for the game logic: contains the main game loop and delegates
 * work to the instances of Module and GUI. Serves as a Service Locator.
 *
 * @see game::Module
 * @see gui::GUI
 */
class Game : public graphics::IEventHandler, boost::noncopyable {
public:
    Game(const boost::filesystem::path &path, const Options &opts);

    /**
     * Initialize the engine, run the main game loop and clean up on exit.
     *
     * @return the exit code
     */
    int run();

    /**
     * Request termination of the main game loop.
     */
    void quit();

    void playVideo(const std::string &name);

    bool isLoadFromSaveGame() const;
    bool isPaused() const { return _paused; }
    bool isInConversation() const { return _screen == GameScreen::Conversation; }

    Camera *getActiveCamera() const;
    std::shared_ptr<Object> getObjectById(uint32_t id) const;

    GameID gameId() const { return _gameId; }
    const Options &options() const { return _options; }
    std::shared_ptr<Module> module() const { return _module; }
    HUD &hud() const { return *_hud; }
    CharacterGeneration &characterGeneration() { return *_charGen; }
    CameraType cameraType() const { return _cameraType; }
    Conversation &conversation() { return *_conversation; }
    const std::set<std::string> &moduleNames() const { return _moduleNames; }

    void setCursorType(CursorType type);
    void setLoadFromSaveGame(bool load);
    void setPaused(bool paused);
    void setRelativeMouseMode(bool relative);

    // Services

    audio::AudioFiles &audioFiles() { return _audioFiles; }
    audio::AudioPlayer &audioPlayer() { return _audioPlayer; }
    Classes &classes() { return _classes; }
    Combat &combat() { return _combat; }
    Console &console() { return _console; }
    Cursors &cursors() { return _cursors; }
    Feats &feats() { return _feats; }
    FootstepSounds &footstepSounds() { return _footstepSounds; }
    graphics::Fonts &fonts() { return _fonts; }
    graphics::Lips &lips() { return _lips; }
    graphics::Materials &materials() { return _materials; }
    graphics::Meshes &meshes() { return _meshes; }
    graphics::Models &models() { return _models; }
    graphics::PBRIBL &pbrIbl() { return _pbrIbl; }
    graphics::Shaders &shaders() { return _shaders; }
    graphics::Textures &textures() { return _textures; }
    graphics::Walkmeshes &walkmeshes() { return _walkmeshes; }
    graphics::Window &window() { return _window; }
    GUISounds &guiSounds() { return _guiSounds; }
    ObjectFactory &objectFactory() { return _objectFactory; }
    Party &party() { return _party; }
    Portraits &portraits() { return _portraits; }
    ProfileOverlay &profileOverlay() { return _profileOverlay; }
    Reputes &reputes() { return _reputes; }
    resource::Resources &resources() { return _resources; }
    resource::Strings &strings() { return _strings; }
    Routines &routines() { return _routines; }
    scene::SceneGraph &sceneGraph() { return _sceneGraph; }
    scene::WorldRenderPipeline &worldPipeline() { return _worldPipeline; }
    script::Scripts &scripts() { return _scripts; }
    ScriptRunner &scriptRunner() { return _scriptRunner; }
    SoundSets &soundSets() { return _soundSets; }
    Spells &spells() { return _spells; }
    Surfaces &surfaces() { return _surfaces; }

    // END Services

    // Module loading

    /**
     * Load a module with the specified name and entry point.
     *
     * @param name name of the module to load
     * @param entry tag of the waypoint to spawn at, or empty string to use the default entry point
     */
    void loadModule(const std::string &name, std::string entry = "");

    /**
     * Schedule transition to the specified module with the specified entry point.
     *
     * @param name name of the module to load
     * @param entry tag of the waypoint to spawn at
     */
    void scheduleModuleTransition(const std::string &moduleName, const std::string &entry);

    // END Module loading

    // Game screens

    void openMainMenu();
    void openSaveLoad(SaveLoad::Mode mode);
    void openInGame();
    void openInGameMenu(InGameMenu::Tab tab);
    void openContainer(const std::shared_ptr<SpatialObject> &container);
    void openPartySelection(const PartySelection::Context &ctx);
    void openLevelUp();

    void startCharacterGeneration();
    void startDialog(const std::shared_ptr<SpatialObject> &owner, const std::string &resRef);

    // END Game screens

    // Globals/locals

    bool getGlobalBoolean(const std::string &name) const;
    int getGlobalNumber(const std::string &name) const;
    std::shared_ptr<Location> getGlobalLocation(const std::string &name) const;
    std::string getGlobalString(const std::string &name) const;

    void setGlobalBoolean(const std::string &name, bool value);
    void setGlobalLocation(const std::string &name, const std::shared_ptr<Location> &location);
    void setGlobalNumber(const std::string &name, int value);
    void setGlobalString(const std::string &name, const std::string &value);

    // END Globals/locals

    // Saved games

    void saveToFile(const boost::filesystem::path &path);
    void loadFromFile(const boost::filesystem::path &path);

    // END Saved games

    // IEventHandler

    bool handle(const SDL_Event &event) override;

    // END IEventHandler

private:
    enum class GameScreen {
        None,
        MainMenu,
        Loading,
        CharacterGeneration,
        InGame,
        InGameMenu,
        Conversation,
        Container,
        PartySelection,
        SaveLoad
    };

    boost::filesystem::path _path;
    Options _options;

    GameID _gameId { GameID::KotOR };
    GameScreen _screen { GameScreen::MainMenu };
    uint32_t _ticks { 0 };
    bool _quit { false };
    std::shared_ptr<video::Video> _video;
    CursorType _cursorType { CursorType::None };
    float _gameSpeed { 1.0f };
    bool _loadFromSaveGame { false };
    CameraType _cameraType { CameraType::ThirdPerson };
    bool _paused { false };
    Conversation *_conversation { nullptr }; /**< pointer to either DialogGUI or ComputerGUI  */
    std::set<std::string> _moduleNames;

    // Services

    audio::AudioFiles _audioFiles;
    audio::AudioPlayer _audioPlayer;
    Combat _combat;
    Classes _classes;
    Console _console;
    Cursors _cursors;
    Feats _feats;
    FootstepSounds _footstepSounds;
    graphics::Fonts _fonts;
    graphics::Lips _lips;
    graphics::Materials _materials;
    graphics::Meshes _meshes;
    graphics::Models _models;
    graphics::PBRIBL _pbrIbl;
    graphics::Shaders _shaders;
    graphics::Textures _textures;
    graphics::Walkmeshes _walkmeshes;
    graphics::Window _window;
    GUISounds _guiSounds;
    ObjectFactory _objectFactory;
    Party _party;
    Portraits _portraits;
    ProfileOverlay _profileOverlay;
    Reputes _reputes;
    resource::Resources _resources;
    resource::Strings _strings;
    Routines _routines;
    scene::SceneGraph _sceneGraph;
    scene::WorldRenderPipeline _worldPipeline;
    script::Scripts _scripts;
    ScriptRunner _scriptRunner;
    SoundSets _soundSets;
    Spells _spells;
    Surfaces _surfaces;

    // END Services

    // Modules

    std::string _nextModule;
    std::string _nextEntry;
    std::shared_ptr<Module> _module;
    std::map<std::string, std::shared_ptr<Module>> _loadedModules;

    // END Modules

    // GUI

    std::unique_ptr<MainMenu> _mainMenu;
    std::unique_ptr<LoadingScreen> _loadScreen;
    std::unique_ptr<CharacterGeneration> _charGen;
    std::unique_ptr<HUD> _hud;
    std::unique_ptr<InGameMenu> _inGame;
    std::unique_ptr<DialogGUI> _dialog;
    std::unique_ptr<ComputerGUI> _computer;
    std::unique_ptr<Container> _container;
    std::unique_ptr<PartySelection> _partySelect;
    std::unique_ptr<SaveLoad> _saveLoad;

    // END GUI

    // Audio

    std::string _musicResRef;
    std::shared_ptr<audio::SoundHandle> _music;
    std::shared_ptr<audio::SoundHandle> _movieAudio;

    // END Audio

    // Globals/locals

    std::map<std::string, std::string> _globalStrings;
    std::map<std::string, bool> _globalBooleans;
    std::map<std::string, int> _globalNumbers;
    std::map<std::string, std::shared_ptr<Location>> _globalLocations;

    // END Globals/locals

    void initSubsystems();
    void deinitSubsystems();

    void update();

    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleKeyDown(const SDL_KeyboardEvent &event);
    void loadNextModule();
    float measureFrameTime();
    void playMusic(const std::string &resRef);
    void runMainLoop();
    void toggleInGameCameraType();
    void updateCamera(float dt);
    void stopMovement();
    void changeScreen(GameScreen screen);
    void updateVideo(float dt);
    void updateMusic();
    void updateSceneGraph(float dt);

    std::string getMainMenuMusic() const;
    std::string getCharacterGenerationMusic() const;
    gui::GUI *getScreenGUI() const;

    // Resource management

    void initResourceProviders();
    void initResourceProvidersForKotOR();
    void initResourceProvidersForTSL();

    void loadModuleNames();
    void loadModuleResources(const std::string &moduleName);

    // END Resource management

    // Loading

    void loadCharacterGeneration();
    void loadContainer();
    void loadDialog();
    void loadComputer();
    void loadHUD();
    void loadInGame();
    void loadLoadingScreen();
    void loadMainMenu();
    void loadPartySelection();
    void loadSaveLoad();

    // END Loading

    // Rendering

    void drawAll();
    void drawWorld();
    void drawGUI();

    // END Rendering

    // Helper methods

    void withLoadingScreen(const std::string &imageResRef, const std::function<void()> &block);

    // END Helper methods
};

} // namespace game

} // namespace reone
