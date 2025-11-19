#include "Nodes.hpp"
#include "NodeLibrary.hpp"
#include "Serializer.hpp"
#include <fstream>

CustomNode::CustomNode(VM &vm, VM::NodeId id)
    : m_vm(vm), m_id(id)
{
    auto node = m_vm.getNode(id);
    m_drawfuncId = node.draw;

    setTitle(node.name);
    setStyle(ImFlow::NodeStyle::cyan());
    for (size_t i = 0; i < node.inputs.size(); i++)
    {
        if (node.type == VM::NodeType::CUSTOM)
        {
            ImFlow::BaseNode::addIN_uid<uint64_t>(node.inputs[i].id, node.inputs[i].name, 0, ConnectionFilter::Default(vm));
        }
        else
        {
            ImFlow::BaseNode::addIN_uid<uint64_t>(node.inputs[i].id, i ? "B" : "A", 0, ConnectionFilter::Default(vm));
        }
    }

    for (size_t i = 0; i < node.outputs.size(); i++)
    {
        if (node.type == VM::NodeType::CUSTOM)
        {
            ImFlow::BaseNode::addOUT_uid<uint64_t>(node.outputs[i].id, node.outputs[i].name)->behaviour([this, i]()
                                                                                                        { return m_vm.getOutput(this, m_id, i); });
        }
        else
        {
            ImFlow::BaseNode::addOUT_uid<uint64_t>(node.outputs[i].id, "Q")->behaviour([this, i]()
                                                                                       { return m_vm.getOutput(this, m_id, i); });
        }
    }
}

void CustomNode::draw()
{
    if (m_vm.getNode(m_id).type == VM::NodeType::CUSTOM && m_drawfuncId < VM::Node::drawFunctionTable.size() && VM::Node::drawFunctionTable[m_drawfuncId])
    {
        VM::Node::drawFunctionTable[m_drawfuncId](m_vm, m_id, this);
    }
}

NodeEditor::NodeEditor()
{
    NodeLibrary::setupLibrary();
}

VM::NodeId NodeEditor::addNode(const VM::NodeType &type, ImVec2 pos)
{
    auto id = vm.getNewNodeId();
    vm.registerNode(id, type);
    m_INF.addNode_uid<CustomNode>(id, pos, vm, id);
    return id;
}

VM::NodeId NodeEditor::addNode(VM::Node node, ImVec2 pos)
{
    if (node.id == 0)
    {
        node.id = vm.getNewNodeId();
    }
    vm.registerNode(node);
    m_INF.addNode_uid<CustomNode>(node.id, pos, vm, node.id);
    return node.id;
}

VM::NodeId NodeEditor::addNode(const VM::NodeType &type)
{
    auto id = addNode(type, {100, 100});
    return id;
}

VM::NodeId NodeEditor::addNode(VM::Node node)
{
    auto id = addNode(node, {100, 100});
    return id;
}

void NodeEditor::draw()
{
    m_INF.update();
}

void NodeEditor::set_size(const ImVec2 &size)
{
    m_INF.setSize(size);
}

void NodeEditor::save(const std::string &filepath)
{
    auto data = Serializer::editor_to_json(this);
    std::ofstream f(filepath);
    f << std::setw(0) << data << std::endl;
    f.close();
}

void NodeEditor::load(const std::string &filepath)
{
    std::ifstream f(filepath);
    json data;
    f >> data;
    f.close();
    m_INF = ImFlow::ImNodeFlow();
    Serializer::json_to_editor(this, data);
}
