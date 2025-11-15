#include "App.hpp"
#include "imgui.h"
#include <thread>
#include <chrono>
#include "Utils.hpp"
#include <filesystem>
#include <iostream>
#include <atomic>

#include "version.def"

App::App()
{
    std::string versionStr(reinterpret_cast<const char *>(____version),
                           static_cast<size_t>(____version_len));
    state.version = new Version(versionStr);
    state.updater = new Updater();
    state.nodeEditor = new NodeEditor();
}

App::~App()
{
    if (state.version)
    {
        delete state.version;
        state.version = nullptr;
    }

    if (state.updater)
    {
        delete state.updater;
        state.updater = nullptr;
    }

    if (state.nodeEditor)
    {
        delete state.nodeEditor;
        state.nodeEditor = nullptr;
    }
}

void App::init()
{
    state.nodeEditor->addNode(VM::NodeType::NOT);
    state.nodeEditor->addNode(VM::NodeType::AND);
    state.nodeEditor->addNode(VM::NodeType::OR);
    state.nodeEditor->set_size(ImVec2(400, 640));
}

void App::render()
{
    state.mainWindowSize = renderer.getWindowSize();
    gui.render(state);
}

void App::keyCallback(const SDL_KeyboardEvent &keyEvent)
{
    if (keyEvent.type == SDL_KEYDOWN)
    {
        if (keyEvent.keysym.sym == SDLK_F4 && (keyEvent.keysym.mod & KMOD_ALT))
        {
            state.progamShouldExit = true;
        }
    }
}

int App::run(int argc, char **argv, std::filesystem::path logFile)
{
    state.logFile = logFile;
    state.renderer = &renderer;
    init();
    std::filesystem::path exeDir = Utils::getExecutableDir();
    try
    {
        std::filesystem::current_path(exeDir);
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        // If changing directory fails, log the error but continue
        std::cerr << "Warning: Could not change working directory to executable directory: " << e.what() << std::endl;
    }

    renderer.setRenderFunction(std::bind(&App::render, this));
    renderer.setKeyCallback(std::bind(&App::keyCallback, this, std::placeholders::_1));

    if (renderer.startRenderLoop(&state) != 0)
        return -1; // Failed to start render loop

    if (state.updater && state.updater->checkUpdate(state))
    {
        state.newVersionPopup = true;
    }

    while (renderer.isRunning())
    {
        if (state.progamShouldExit)
            break;

        if (state.downloadUpdate)
        {
            state.downloadUpdate = false;
            state.commandInProgress.progress = 0;
            state.commandInProgress.progressDisabled = false;
            state.commandInProgress.text = "Downloading update...";
            state.commandInProgress.enabled = true;
            if (state.updater && !state.updater->downloadUpdate(state))
            {
                state.readyForUpdate = true;
                state.progamShouldExit = true;
            }
            else
            {
                state.showFile.errorLog = true;
                state.showFile.enabled = true;
            }
            state.commandInProgress.enabled = false;
            state.commandInProgress.progress = -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout.flush();
    }

    renderer.join();

    if (state.readyForUpdate)
    {
        std::cout << "updating...\n";
        state.updater->update(state);
    }

    return 0;
}