#include "NodeLibrary.hpp"

namespace NodeLibrary
{
    void render(AppState &state)
    {
        if (!ImGui::Begin("Library", nullptr, ImGuiWindowFlags_NoTitleBar))
        {
            return;
        }
        auto &nodeEditor = *state.nodeEditor;

        if (ImGui::TreeNode("Basic"))
        {
            if (ImGui::Button("Input"))
            {
                nodeEditor.addNode(inputNode, render_inputNode);
            }
            if (ImGui::Button("Output"))
            {
                nodeEditor.addNode(outputNode, render_outputNode);
            }
            if (ImGui::Button("bus 1"))
            {
                nodeEditor.addNode(busNode1, render_busNode1);
            }
            if (ImGui::Button("bus 2"))
            {
                nodeEditor.addNode(busNode2, render_busNode2);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Logic Gates"))
        {
            if (ImGui::Button("NOT"))
            {
                nodeEditor.addNode(VM::NodeType::NOT);
            }
            if (ImGui::Button("AND"))
            {
                nodeEditor.addNode(VM::NodeType::AND);
            }
            if (ImGui::Button("NAND"))
            {
                nodeEditor.addNode(VM::NodeType::NAND);
            }
            if (ImGui::Button("OR"))
            {
                nodeEditor.addNode(VM::NodeType::OR);
            }
            if (ImGui::Button("NOR"))
            {
                nodeEditor.addNode(VM::NodeType::NOR);
            }
            if (ImGui::Button("XOR"))
            {
                nodeEditor.addNode(VM::NodeType::XOR);
            }
            if (ImGui::Button("XNOR"))
            {
                nodeEditor.addNode(VM::NodeType::XNOR);
            }
            ImGui::TreePop();
        }

        ImGui::End();
    }

    VM::Node inputNode =
        {
            std::vector<VM::Node::IOPin>(),
            std::vector<VM::Node::IOPin>({{1, "Q", 0, 0}}),
            0,
            VM::NodeType::CUSTOM,
            [](VM &vm, VM::NodeId &id, ImFlow::BaseNode *inf_node, size_t outnum)
            {
                return vm.getNode(id).n_data;
            },
            "Input",
            0,
            nullptr};

    void render_inputNode(VM &vm, VM::NodeId &id, CustomNode &inf_node)
    {
        auto &node = vm.getNode(id);
        ImGui::Checkbox("Val:", (bool *)&node.n_data);
    }

    VM::Node outputNode =
        {
            std::vector<VM::Node::IOPin>({{1, "I", 0, 0}}),
            std::vector<VM::Node::IOPin>(),
            0,
            VM::NodeType::CUSTOM,
            [](VM &vm, VM::NodeId &id, ImFlow::BaseNode *inf_node, size_t outnum)
            {
                return 0;
            },
            "Output",
            0,
            nullptr};

    void render_outputNode(VM &vm, VM::NodeId &id, CustomNode &inf_node)
    {
        auto &node = vm.getNode(id);
        ImGui::Checkbox("", (bool *)&inf_node.getInVal<uint64_t>(node.inputs[0].id));
    }

    VM::Node busNode1 =
        {
            std::vector<VM::Node::IOPin>({{2, "0", 0, 0}}),
            std::vector<VM::Node::IOPin>({{1, "0", 0, 0}, {1, "1", 0, 0}}),
            0,
            VM::NodeType::CUSTOM,
            calc_busNode1,
            "",
            2,
            nullptr};

    void render_busNode1(VM &vm, VM::NodeId &id, CustomNode &inf_node)
    {
        auto &node = vm.getNode(id);
        ImGui::SetNextItemWidth(80.0f);
        if (ImGui::InputInt("", (int*)&node.n_data))
        {
            if (node.n_data > 64)
            {
                node.n_data = 64;
            }
            else if (node.n_data < 1)
            {
                node.n_data = 1;
            }
            for (size_t i = 0; i < node.outputs.size(); i++)
            {
                inf_node.dropOUT(node.outputs[i].id);
            }
            node.outputs.clear();
            for (size_t i = 0; i < node.n_data; i++)
            {
                node.outputs.push_back({1, std::to_string(i), 0, vm.getNewPinId()});
                inf_node.addOUT_uid<uint64_t>(node.outputs[i].id, std::to_string(i))->behaviour([&vm, &id, &inf_node, i](){return vm.getOutput(&inf_node, id, i);});
            }
            node.inputs[0].size = node.n_data;
        }
    }

    uint64_t calc_busNode1(VM &vm, VM::NodeId &id, ImFlow::BaseNode *inf_node, size_t outnum)
    {
        auto &node = vm.getNode(id);
        uint64_t val = inf_node->getInVal<uint64_t>(node.inputs[0].id);
        val = (val & (1 << outnum)) >> outnum;
        return val;
    }

    VM::Node busNode2 =
        {
            std::vector<VM::Node::IOPin>({{1, "0", 0, 0}, {1, "1", 0, 0}}),
            std::vector<VM::Node::IOPin>({{2, "0", 0, 0}}),
            0,
            VM::NodeType::CUSTOM,
            calc_busNode2,
            "",
            2,
            nullptr};

    void render_busNode2(VM &vm, VM::NodeId &id, CustomNode &inf_node)
    {
        auto &node = vm.getNode(id);
        ImGui::SetNextItemWidth(80.0f);
        if (ImGui::InputInt("", (int*)&node.n_data))
        {
            if (node.n_data > 64)
            {
                node.n_data = 64;
            }
            else if (node.n_data < 1)
            {
                node.n_data = 1;
            }
            for (size_t i = 0; i < node.inputs.size(); i++)
            {
                inf_node.dropIN(node.inputs[i].id);
            }
            node.inputs.clear();
            for (size_t i = 0; i < node.n_data; i++)
            {
                node.inputs.push_back({1, std::to_string(i), 0, vm.getNewPinId()});
                inf_node.addIN_uid<uint64_t>(node.inputs[i].id, std::to_string(i), 0, ImFlow::ConnectionFilter::SameType());
            }
            node.outputs[0].size = node.n_data;
        }
    }

    uint64_t calc_busNode2(VM &vm, VM::NodeId &id, ImFlow::BaseNode *inf_node, size_t outnum)
    {
        auto &node = vm.getNode(id);
        uint64_t val = 0;
        for (size_t i = 0; i < node.inputs.size(); i++)
        {
            val |= (inf_node->getInVal<uint64_t>(node.inputs[i].id) & 1) << i;
        }
        return val;
    }

} // namespace NodeLibrary