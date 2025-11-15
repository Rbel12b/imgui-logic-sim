#pragma once

#include "ImNodeFlow.h"
#include "VM.hpp"
#include <functional>

class CustomNode : public ImFlow::BaseNode
{
public:
    using DrawFunc = std::function<void(VM &, VM::NodeId &, CustomNode &)>;
    CustomNode(VM &vm, VM::NodeId id, DrawFunc drawfunc = nullptr);

    void draw() override;

private:
    VM &m_vm;
    VM::NodeId m_id;
    DrawFunc m_drawfunc;
};

class NodeEditor
{
public:
    NodeEditor();
    void addNode(const VM::NodeType &type);
    void addNode(VM::Node node, CustomNode::DrawFunc drawFunc);

    void draw();

    void set_size(const ImVec2 &size);

private:
    ImFlow::ImNodeFlow m_INF;
    VM vm;
};