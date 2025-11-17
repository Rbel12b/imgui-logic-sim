#include "VM.hpp"
#include <stdexcept>
#include <algorithm>

std::vector<VM::Node::customComputeFunc> VM::Node::computeFunctionTable = {nullptr};
std::vector<VM::Node::customDrawFunc> VM::Node::drawFunctionTable = {nullptr};

VM::NodeId VM::getNewNodeId()
{
    return m_NodeIdCounter++;
}

VM::PinId VM::getNewPinId()
{
    return m_PinIdCounter++;
}

void VM::registerNode(NodeId id, NodeType type)
{
    Node node;
    node.id = id;
    node.type = type;
    switch (type)
    {
    case NodeType::NOT:
    case NodeType::BUFFER:
        node.inputs.push_back({1, "", id, getNewPinId()});
        node.outputs.push_back({1, "", id, getNewPinId()});
        break;

    case NodeType::CUSTOM:
        throw std::runtime_error("Cannot register node of type CUSTOM with registerNode(NodeId id, NodeType type) use registerNode(Node node) instead");
        return;

    default:
        node.inputs.push_back({1, "", id, getNewPinId()});
        node.inputs.push_back({1, "", id, getNewPinId()});
        node.outputs.push_back({1, "", id, getNewPinId()});
        break;
    }

    switch (type)
    {
    case NodeType::NOT:
        node.name = "NOT";
        break;

    case NodeType::BUFFER:
        node.name = "BUFFER";
        break;

    case NodeType::AND:
        node.name = "AND";
        break;

    case NodeType::NAND:
        node.name = "NAND";
        break;

    case NodeType::OR:
        node.name = "OR";
        break;

    case NodeType::NOR:
        node.name = "NOR";
        break;

    case NodeType::XOR:
        node.name = "XOR";
        break;

    case NodeType::XNOR:
        node.name = "XNOR";
        break;
    }
    m_nodeData[node.id] = node;
}

void VM::registerNode(const Node &_node)
{
    Node &node = m_nodeData[_node.id];
    node = _node;
    if (node.id == 0)
    {
        node.id = getNewNodeId();
    }

    for (size_t i = 0; i < node.inputs.size(); i++)
    {
        node.inputs[i].owner = node.id;
        if (node.inputs[i].id == 0)
        {
            node.inputs[i].id = getNewPinId();
        }
    }
    for (size_t i = 0; i < node.outputs.size(); i++)
    {
        node.outputs[i].owner = node.id;
        if (node.outputs[i].id == 0)
        {
            node.outputs[i].id = getNewPinId();
        }
    }
}

VM::Node &VM::getNode(NodeId id)
{
    if (id == 0)
    {
        throw std::runtime_error("NodeId should not equal 0");
    }
    if (!m_nodeData.count(id))
    {
        throw std::runtime_error("NodeId not found!");
    }

    return m_nodeData[id];
}

VM::Node::IOPin &VM::getPin(PinId id)
{
    if (id == 0)
        throw std::runtime_error("PinId should not equal 0");

    for (auto &entry : m_nodeData)
    {
        Node &node = entry.second;
        for (auto &pin : node.inputs)
            if (pin.id == id)
                return pin;
        for (auto &pin : node.outputs)
            if (pin.id == id)
                return pin;
    }

    throw std::runtime_error("PinId not found!");
}

uint64_t VM::getOutput(ImFlow::BaseNode *inf_node, NodeId id, size_t outnum)
{
    if (id == 0)
    {
        throw std::runtime_error("NodeId should not equal 0");
    }
    if (!m_nodeData.count(id))
    {
        throw std::runtime_error("NodeId not found!");
    }

    auto &node = m_nodeData[id];

    uint64_t val = 0;

    switch (node.type)
    {
    case NodeType::CUSTOM:
        if (node.computeIO < Node::computeFunctionTable.size() && Node::computeFunctionTable[node.computeIO])
        {
            val = Node::computeFunctionTable[node.computeIO](*this, node.id, inf_node, outnum);
        }
        break;

    case NodeType::NOT:
        val = !inf_node->getInVal<uint64_t>(node.inputs[0].id);
        break;

    case NodeType::BUFFER:
        val = inf_node->getInVal<uint64_t>(node.inputs[0].id);
        break;

    case NodeType::AND:
        val = (inf_node->getInVal<uint64_t>(node.inputs[0].id) && inf_node->getInVal<uint64_t>(node.inputs[1].id));
        break;

    case NodeType::NAND:
        val = !(inf_node->getInVal<uint64_t>(node.inputs[0].id) && inf_node->getInVal<uint64_t>(node.inputs[1].id));
        break;

    case NodeType::OR:
        val = (inf_node->getInVal<uint64_t>(node.inputs[0].id) || inf_node->getInVal<uint64_t>(node.inputs[1].id));
        break;

    case NodeType::NOR:
        val = !(inf_node->getInVal<uint64_t>(node.inputs[0].id) || inf_node->getInVal<uint64_t>(node.inputs[1].id));
        break;

    case NodeType::XOR:
        val = (inf_node->getInVal<uint64_t>(node.inputs[0].id) ^ inf_node->getInVal<uint64_t>(node.inputs[1].id));
        break;

    case NodeType::XNOR:
        val = !(inf_node->getInVal<uint64_t>(node.inputs[0].id) ^ inf_node->getInVal<uint64_t>(node.inputs[1].id));
        break;

    default:
        break;
    }
    if (node.outputs[outnum].size == 1)
    {
        inf_node->getOuts()[outnum]->setStyle(val ? ImFlow::PinStyle::red() : ImFlow::PinStyle::blue());
    }
    else
    {
        inf_node->getOuts()[outnum]->setStyle(ImFlow::PinStyle::brown());
    }
    return val;
}