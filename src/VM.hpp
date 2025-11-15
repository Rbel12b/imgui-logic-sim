#pragma once

#include "ImNodeFlow.h"
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

class VM
{
public:
    typedef size_t NodeId; // 0 is an invalid ID.
    typedef size_t PinId;  // 0 is an invalid ID.
    enum class NodeType
    {
        unknown,
        NOT,
        AND,
        NAND,
        OR,
        NOR,
        XOR,
        XNOR,
        CUSTOM
    };

    struct Node
    {
        using customComputeFunction = std::function<uint64_t(VM&, VM::NodeId&, ImFlow::BaseNode*, size_t)>;

        struct IOPin
        {
            uint8_t size = 1; // max 64 bits
            // uint64_t state = 0; // state, if bit count (size) is less than 64 the right n bits are used.
            // NodeId connectedNode = 0;
            // size_t connectedPinNum = 0;
            std::string name;
            NodeId owner = 0;
            PinId id = 0;
        };

        std::vector<IOPin> inputs;
        std::vector<IOPin> outputs;

        NodeId id = 0;
        NodeType type;
        customComputeFunction computeIO = nullptr;

        std::string name;

        uint64_t n_data = 0;
        void *p_data = nullptr;
    };

    NodeId getNewNodeId();
    PinId getNewPinId();
    void registerNode(NodeId id, NodeType type);
    void registerNode(const Node &_node);

    Node &getNode(NodeId id);

    uint64_t getOutput(ImFlow::BaseNode *inf_node, NodeId id, size_t outnum);

    // void registerLink(NodeId from, size_t fromNum, NodeId to, size_t toNum);

    // void updateInput(NodeId node, size_t inputNum, uint64_t state, bool allowedRecompute = false);

    // Node& getIO(NodeId id, bool allowedRecompute = false);

    // void update();

private:
    // void updatePinLink(Node::IOPin& pin, bool allowedRecompute = false);

    NodeId m_NodeIdCounter = 1; // value is always the next free ID
    PinId m_PinIdCounter = 1;   // value is always the next free ID
    std::unordered_map<NodeId, Node> m_nodeData;
};