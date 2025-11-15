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

    extern VM::Node busNode1;
    void render_busNode1(VM &vm, VM::NodeId &id, CustomNode& inf_node);
    uint64_t calc_busNode1(VM &vm, VM::NodeId &id, ImFlow::BaseNode* inf_node, size_t outnum);

    extern VM::Node busNode2;
    void render_busNode2(VM &vm, VM::NodeId &id, CustomNode& inf_node);
    uint64_t calc_busNode2(VM &vm, VM::NodeId &id, ImFlow::BaseNode* inf_node, size_t outnum);
};