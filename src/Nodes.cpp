#include "Nodes.hpp"

CustomNode::CustomNode(VM &vm, VM::NodeId id, DrawFunc drawfunc)
    : m_vm(vm), m_id(id), m_drawfunc(drawfunc)
{
    auto node = m_vm.getNode(id);

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
    if (m_drawfunc)
    {
        m_drawfunc(m_vm, m_id, *this);
    }
}

NodeEditor::NodeEditor()
{
}

void NodeEditor::addNode(const VM::NodeType &type)
{
    auto id = vm.getNewNodeId();
    vm.registerNode(id, type);
    m_INF.addNode<CustomNode>(ImVec2(100, 100), vm, id);
}

void NodeEditor::addNode(VM::Node node, CustomNode::DrawFunc drawFunc)
{
    if (node.id == 0)
    {
        node.id = vm.getNewNodeId();
    }
    vm.registerNode(node);
    m_INF.addNode<CustomNode>(ImVec2(100, 100), vm, node.id, drawFunc);
}

void NodeEditor::draw()
{
    m_INF.update();
}

void NodeEditor::set_size(const ImVec2 &size)
{
    m_INF.setSize(size);
}
