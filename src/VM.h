#pragma once

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

    struct NodeIO
    {
        using customComputeFunction = std::function<void(NodeIO&)>;

        struct IOPin
        {
            uint8_t size = 1; // max 64 bits
            uint64_t state = 0; // state, if bit count (size) is less than 64 the right n bits are used.
            NodeId connectedNode = 0;
            size_t connectedPinNum = 0;
            std::string name;
            NodeId owner = 0;
        };

        std::vector<IOPin> inputs;
        std::vector<IOPin> outputs;

        bool recompute = true;
        NodeId self = 0;
        NodeType type;
        customComputeFunction computeIO = nullptr;

        std::string name;
    };

    NodeId getNewNodeId();
    void registerNode(NodeId id, NodeType type);
    void registerNode(const NodeIO& io);
    void registerLink(NodeId from, size_t fromNum, NodeId to, size_t toNum);

    void updateInput(NodeId node, size_t inputNum, uint64_t state, bool allowedRecompute = false);

    NodeIO& getIO(NodeId id, bool allowedRecompute = false);

    void update();

private:
    void updatePinLink(NodeIO::IOPin& pin, bool allowedRecompute = false);

    NodeId m_idCounter = 1; // value is always the next free ID
    std::unordered_map<NodeId, NodeIO> m_nodeData;
};