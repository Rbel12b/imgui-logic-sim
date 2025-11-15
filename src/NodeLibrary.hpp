#pragma once

#include "AppState.hpp"
#include "imgui.h"

namespace NodeLibrary
{
    void render(AppState &state);

    extern VM::Node inputNode;
    void render_inputNode(VM &vm, VM::NodeId &id, CustomNode& inf_node);

    extern VM::Node outputNode;
    void render_outputNode(VM &vm, VM::NodeId &id, CustomNode& inf_node);
};