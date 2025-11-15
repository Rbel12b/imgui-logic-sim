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
            [](VM::Node &node)
            {
                return node.n_data;
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
            [](VM::Node &node)
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

} // namespace NodeLibrary