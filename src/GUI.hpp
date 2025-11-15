#pragma once
#ifndef GUI_H
#define GUI_H

#include "AppState.hpp"
#include "imgui.h"

class GUI
{
public:
    GUI() = default;
    ~GUI() = default;
    void render(AppState &state);

private:
    void renderMenuBar(AppState &state);
    void renderMain(AppState &state);
    void renderInProgressPopup(AppState &state);
    void renderErrorLogPopup(AppState &state);
    void renderNewVersionPopup(AppState &state);
};

#endif // GUI_H