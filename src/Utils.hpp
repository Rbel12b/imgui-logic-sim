#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <filesystem>
#include "AppState.hpp"
#include <functional>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h> // For SHGetKnownFolderPath
#include <combaseapi.h> // CoTaskMemFree
#else
#include <cstdlib>
#endif

namespace Utils
{

// Windows helper
#ifdef _WIN32
    std::string getKnownFolder(REFKNOWNFOLDERID folderId);
#endif

    std::string getVideosDir();
    std::string getMusicDir();
    std::string getDownloadsDir();
    std::filesystem::path getBundledExePath(const std::string &name);
    std::filesystem::path getBundledFilePath(const std::filesystem::path &name);

    // Returns the absolute path of the running executable
    std::filesystem::path getExecutableDir();
    std::filesystem::path getExecutable();

    /**
     * Returns a user-writable directory for app data
     * Windows -> %LOCALAPPDATA%\project-name
     * Linux   -> $HOME/.config/project-name
     */
    std::filesystem::path getUserDataDir();

    std::filesystem::path getTempDir();

    bool loadFileToString(const std::string &path, std::string &out);

    /**
     * file download using curl
     * @param url remote file URL
     * @param dest local path to save the file
     */
    bool downloadFile(const std::string &url, const std::filesystem::path &dest, AppState& state);
}

#endif // UTILS_HPP
