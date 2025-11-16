#pragma once

#include "ImNodeFlow.h"
#include "VM.hpp"
#include <functional>

class ConnectionFilter
{
public:
    static std::function<bool(ImFlow::Pin*, ImFlow::Pin*)> Default(VM& vm) { return [&vm](ImFlow::Pin* out, ImFlow::Pin* in) { return vm.getPin(out->getUid()).size == vm.getPin(in->getUid()).size; }; }
};


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

    void save(const std::string &filepath);
    void load(const std::string &filepath);

    VM* getVM()
    {
        return &vm;
    }

    ImFlow::ImNodeFlow* getINF()
    {
        return &m_INF;
    }

private:
    ImFlow::ImNodeFlow m_INF;
    VM vm;
};