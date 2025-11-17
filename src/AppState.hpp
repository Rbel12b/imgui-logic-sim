#pragma once
#ifndef APP_STATE_H
#define APP_STATE_H

class AppState;

#include "Nodes.hpp"
#include "SDL2/SDL.h"
#include <filesystem>
#include <string>
#include <vector>
#include <atomic>
#include <cstdint>
#include "Renderer.hpp"
#include "Updater.hpp"
#include "imgui.h"
#include <mutex>

class AppState
{
public:
    static const std::string appName;
    Renderer *renderer;
    bool progamShouldExit = false;
    ImVec2 mainWindowSize = ImVec2(0, 0);
    std::filesystem::path logFile;
    Version *version = nullptr;
    bool readyForUpdate = false;
    std::string repoUrl = "https://github.com/Rbel12b/imgui-logic-sim";
    Updater *updater;
    bool newVersionPopup = false;
    bool downloadUpdate = false;

    struct
    {
        bool enabled = false;
        std::string text = "";
        int progress = -1;
        bool progressDisabled = true;
    } commandInProgress;

    struct
    {
        bool enabled = false;
        std::filesystem::path filePath;
        std::string msg;
        bool errorLog = false;
    } showFile;

    NodeEditor *nodeEditor = nullptr;
    std::mutex editorMutex;

    // struct
    // {
    //     std::string title;
    //     std::filesystem::path result;
    //     enum class Type
    //     {
    //         OPEN,
    //         SAVE,
    //         FOLDER
    //     };
    //     Type type;
    //     bool finished = false;
    // } filedialog;

    bool saveProject = false;
    bool loadProject = false;
};

#endif // APP_STATE_H