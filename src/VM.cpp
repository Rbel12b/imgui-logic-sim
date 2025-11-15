#include "VM.h"
#include <stdexcept>
#include <algorithm>

VM::NodeId VM::getNewNodeId()
{
    return m_idCounter++;
}

void VM::registerNode(NodeId id, NodeType type)
{
    NodeIO io;
    io.self = id;
    io.type = type;
    switch (type)
    {
    case NodeType::NOT:
        io.inputs.push_back({1, 0, 0, 0, "", id});
        io.outputs.push_back({1, 0, 0, 0, "", id});
        break;

    case NodeType::CUSTOM:
        throw std::runtime_error("Cannot register node of type CUSTOM with registerNode(NodeId id, NodeType type) use registerNode(NodeIO io) instead");
        return;

    default:
        io.inputs.push_back({1, 0, 0, 0, "", id});
        io.inputs.push_back({1, 0, 0, 0, "", id});
        io.outputs.push_back({1, 0, 0, 0, "", id});
        break;
    }

    switch (type)
    {
    case NodeType::NOT:
        io.name = "NOT";
        break;

    case NodeType::AND:
        io.name = "AND";
        break;

    case NodeType::NAND:
        io.name = "NAND";
        break;

    case NodeType::OR:
        io.name = "OR";
        break;

    case NodeType::NOR:
        io.name = "NOR";
        break;

    case NodeType::XOR:
        io.name = "XOR";
        break;

    case NodeType::XNOR:
        io.name = "XNOR";
        break;
    }
    m_nodeData[io.self] = io;
}

void VM::registerNode(const NodeIO& _io)
{
    NodeIO& io = m_nodeData[io.self];
    io = _io;
    if (io.self == 0)
    {
        io.self = getNewNodeId();
    }

    for (size_t i = 0; i < io.inputs.size(); i++)
    {
        io.inputs[i].owner = io.self;
    }
    for (size_t i = 0; i < io.outputs.size(); i++)
    {
        io.outputs[i].owner = io.self;
    }
}

void VM::registerLink(NodeId from, size_t fromNum, NodeId to, size_t toNum)
{
    if (from == 0)
    {
        throw std::runtime_error("NodeId should not equal 0");
    }
    if (!m_nodeData.count(from))
    {
        throw std::runtime_error("NodeId not found!");
    }

    auto &fromNode = m_nodeData[from];

    if (to == 0)
    {
        throw std::runtime_error("NodeId should not equal 0");
    }
    if (!m_nodeData.count(to))
    {
        throw std::runtime_error("NodeId not found!");
    }

    auto &toNode = m_nodeData[to];

    fromNode.outputs[fromNum].connectedNode = to;
    fromNode.outputs[fromNum].connectedPinNum = toNum;

    toNode.inputs[toNum].connectedNode = from;
    toNode.inputs[toNum].connectedPinNum = fromNum;
}

void VM::updateInput(NodeId id, size_t inputNum, uint64_t state, bool allowedRecompute)
{
    if (id == 0)
    {
        throw std::runtime_error("NodeId should not equal 0");
    }
    if (!m_nodeData.count(id))
    {
        throw std::runtime_error("NodeId not found!");
    }

    if (m_nodeData[id].inputs[inputNum].state != state)
    {
        m_nodeData[id].recompute = true;
        m_nodeData[id].inputs[inputNum].state = state;
    }

    if (allowedRecompute)
    {
        getIO(id, allowedRecompute);
    }
}

VM::NodeIO &VM::getIO(NodeId id, bool allowedRecompute)
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
    if (!node.recompute || !allowedRecompute)
    {
        return node;
    }
    for (size_t i = 0; i < node.inputs.size(); i++)
    {
        updatePinLink(node.inputs[i], allowedRecompute);
    }
    switch (node.type)
    {
    case NodeType::CUSTOM:
        if (node.computeIO)
        {
            node.computeIO(node);
        }
        break;

    case NodeType::NOT:
        node.outputs[0].state = !node.inputs[0].state;
        break;

    case NodeType::AND:
        node.outputs[0].state = (node.inputs[0].state && node.inputs[1].state);
        break;

    case NodeType::NAND:
        node.outputs[0].state = !(node.inputs[0].state && node.inputs[1].state);
        break;

    case NodeType::OR:
        node.outputs[0].state = (node.inputs[0].state || node.inputs[1].state);
        break;

    case NodeType::NOR:
        node.outputs[0].state = !(node.inputs[0].state || node.inputs[1].state);
        break;

    case NodeType::XOR:
        node.outputs[0].state = (node.inputs[0].state ^ node.inputs[1].state);
        break;

    case NodeType::XNOR:
        node.outputs[0].state = !(node.inputs[0].state ^ node.inputs[1].state);
        break;

    default:
        break;
    }

    if (allowedRecompute)
    {
        for (size_t i = 0; i < node.outputs.size(); i++)
        {
            m_nodeData[node.outputs[i].connectedNode].recompute = true;
            getIO(node.outputs[i].connectedNode, allowedRecompute);
        }
    }

    node.recompute = false;

    return node;
}

void VM::update()
{
    std::for_each(m_nodeData.begin(), m_nodeData.end(), [](NodeIO &node)
                  { node.recompute = true; });
    std::for_each(m_nodeData.begin(), m_nodeData.end(), [this](NodeIO &node)
                  { getIO(node.self); });
}

void VM::updatePinLink(NodeIO::IOPin &pin, bool allowedRecompute)
{
    if (pin.connectedNode == 0)
    {
        return;
    }

    auto id = pin.connectedNode;

    if (id == 0)
    {
        throw std::runtime_error("NodeId should not equal 0");
    }
    if (!m_nodeData.count(id))
    {
        throw std::runtime_error("NodeId not found!");
    }

    auto &node = m_nodeData[id];

    if (allowedRecompute && node.recompute && id != pin.owner)
    {
        getIO(id, allowedRecompute);
    }

    if (pin.state != node.outputs[pin.connectedPinNum].state)
    {
        m_nodeData[pin.owner].recompute = true;
    }

    pin.state = node.outputs[pin.connectedPinNum].state;
}
