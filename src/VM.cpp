#include "VM.hpp"
#include <stdexcept>
#include <algorithm>

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

uint64_t VM::getOutput(ImFlow::BaseNode *inf_node, NodeId id)
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
        if (node.computeIO)
        {
            val = node.computeIO(node);
        }
        break;

    case NodeType::NOT:
        val = !inf_node->getInVal<uint64_t>(node.inputs[0].id);
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
    if (node.outputs[0].size == 1)
    {
        inf_node->getOuts()[0]->setStyle(val ? ImFlow::PinStyle::red() : ImFlow::PinStyle::blue());
    }
    else
    {
        inf_node->getOuts()[0]->setStyle(ImFlow::PinStyle::brown());
    }
    return val;
}

// void VM::registerLink(NodeId from, size_t fromNum, NodeId to, size_t toNum)
// {
//     if (from == 0)
//     {
//         throw std::runtime_error("NodeId should not equal 0");
//     }
//     if (!m_nodeData.count(from))
//     {
//         throw std::runtime_error("NodeId not found!");
//     }

//     auto &fromNode = m_nodeData[from];

//     if (to == 0)
//     {
//         throw std::runtime_error("NodeId should not equal 0");
//     }
//     if (!m_nodeData.count(to))
//     {
//         throw std::runtime_error("NodeId not found!");
//     }

//     auto &toNode = m_nodeData[to];

//     fromNode.outputs[fromNum].connectedNode = to;
//     fromNode.outputs[fromNum].connectedPinNum = toNum;

//     toNode.inputs[toNum].connectedNode = from;
//     toNode.inputs[toNum].connectedPinNum = fromNum;
// }

// void VM::updateInput(NodeId id, size_t inputNum, uint64_t state, bool allowedRecompute)
// {
//     if (id == 0)
//     {
//         throw std::runtime_error("NodeId should not equal 0");
//     }
//     if (!m_nodeData.count(id))
//     {
//         throw std::runtime_error("NodeId not found!");
//     }

//     if (m_nodeData[id].inputs[inputNum].state != state)
//     {
//         m_nodeData[id].recompute = true;
//         m_nodeData[id].inputs[inputNum].state = state;
//     }

//     if (allowedRecompute)
//     {
//         getIO(id, allowedRecompute);
//     }
// }

// VM::Node &VM::getIO(NodeId id, bool allowedRecompute)
// {
//     if (id == 0)
//     {
//         throw std::runtime_error("NodeId should not equal 0");
//     }
//     if (!m_nodeData.count(id))
//     {
//         throw std::runtime_error("NodeId not found!");
//     }

//     auto &node = m_nodeData[id];
//     if (!node.recompute || !allowedRecompute)
//     {
//         return node;
//     }
//     for (size_t i = 0; i < node.inputs.size(); i++)
//     {
//         updatePinLink(node.inputs[i], allowedRecompute);
//     }
//     switch (node.type)
//     {
//     case NodeType::CUSTOM:
//         if (node.computeIO)
//         {
//             node.computeIO(node);
//         }
//         break;

//     case NodeType::NOT:
//         node.outputs[0].state = !node.inputs[0].state;
//         break;

//     case NodeType::AND:
//         node.outputs[0].state = (node.inputs[0].state && node.inputs[1].state);
//         break;

//     case NodeType::NAND:
//         node.outputs[0].state = !(node.inputs[0].state && node.inputs[1].state);
//         break;

//     case NodeType::OR:
//         node.outputs[0].state = (node.inputs[0].state || node.inputs[1].state);
//         break;

//     case NodeType::NOR:
//         node.outputs[0].state = !(node.inputs[0].state || node.inputs[1].state);
//         break;

//     case NodeType::XOR:
//         node.outputs[0].state = (node.inputs[0].state ^ node.inputs[1].state);
//         break;

//     case NodeType::XNOR:
//         node.outputs[0].state = !(node.inputs[0].state ^ node.inputs[1].state);
//         break;

//     default:
//         break;
//     }

//     if (allowedRecompute)
//     {
//         for (size_t i = 0; i < node.outputs.size(); i++)
//         {
//             m_nodeData[node.outputs[i].connectedNode].recompute = true;
//             getIO(node.outputs[i].connectedNode, allowedRecompute);
//         }
//     }

//     node.recompute = false;

//     return node;
// }

// void VM::update()
// {
//     std::for_each(m_nodeData.begin(), m_nodeData.end(), [](Node &node)
//                   { node.recompute = true; });
//     std::for_each(m_nodeData.begin(), m_nodeData.end(), [this](Node &node)
//                   { getIO(node.self); });
// }

// void VM::updatePinLink(Node::IOPin &pin, bool allowedRecompute)
// {
//     if (pin.connectedNode == 0)
//     {
//         return;
//     }

//     auto id = pin.connectedNode;

//     if (id == 0)
//     {
//         throw std::runtime_error("NodeId should not equal 0");
//     }
//     if (!m_nodeData.count(id))
//     {
//         throw std::runtime_error("NodeId not found!");
//     }

//     auto &node = m_nodeData[id];

//     if (allowedRecompute && node.recompute && id != pin.owner)
//     {
//         getIO(id, allowedRecompute);
//     }

//     if (pin.state != node.outputs[pin.connectedPinNum].state)
//     {
//         m_nodeData[pin.owner].recompute = true;
//     }

//     pin.state = node.outputs[pin.connectedPinNum].state;
// }
