#include "Serializer.hpp"

json Serializer::editor_to_json(NodeEditor *_editor)
{
    if (!_editor)
    {
        return json();
    }
    auto &editor = *_editor;
    json json_editor;
    json_editor["vm"] = vm_to_json(editor.getVM(), editor.getINF());
    json_editor["inf"] = inf_to_json(editor.getINF());
    return json_editor;
}

json Serializer::vm_to_json(VM *_vm, ImFlow::ImNodeFlow *_inf)
{
    if (!_vm)
    {
        return json();
    }
    auto &vm = *_vm;
    json json_vm;
    std::vector<json> json_node_array;
    for (const auto &[id, node] : vm.m_nodeData)
    {
        if (!_inf->getNodes().contains(id))
            continue; // the node has been deleted
        auto pos = _inf->getNodes()[id]->getPos();
        json json_node;
        json_node["id"] = node.id;
        json_node["type"] = static_cast<int>(node.type);
        json_node["name"] = node.name;
        json_node["n_data"] = node.n_data;
        json_node["calc"] = node.computeIO;
        json_node["draw"] = node.draw;
        json_node["pos_x"] = pos.x;
        json_node["pos_y"] = pos.y;

        std::vector<json> json_input_array;
        for (const auto &input : node.inputs)
        {
            json json_input;
            json_input["size"] = input.size;
            json_input["name"] = input.name;
            json_input["owner"] = input.owner;
            json_input["id"] = input.id;
            json_input_array.push_back(json_input);
        }
        json_node["inputs"] = json_input_array;

        std::vector<json> json_output_array;
        for (const auto &output : node.outputs)
        {
            json json_output;
            json_output["size"] = output.size;
            json_output["name"] = output.name;
            json_output["owner"] = output.owner;
            json_output["id"] = output.id;
            json_output_array.push_back(json_output);
        }
        json_node["outputs"] = json_output_array;
        json_node_array.push_back(json_node);
    }
    json_vm["nodes"] = json_node_array;
    json_vm["nodeId"] = vm.m_NodeIdCounter;
    json_vm["pinId"] = vm.m_PinIdCounter;
    return json_vm;
}

json Serializer::inf_to_json(ImFlow::ImNodeFlow *_inf)
{
    if (!_inf)
    {
        return json();
    }
    auto &inf = *_inf;
    json json_inf;
    std::vector<json> json_link_array;
    for (auto wplink : inf.getLinks())
    {
        json json_link;
        std::shared_ptr<ImFlow::Link> link = wplink.lock();
        if (!link)
        {
            continue;
        }
        json_link["left"] = link->left()->getUid();
        json_link["right"] = link->right()->getUid();
        json_link_array.push_back(json_link);
    }
    json_inf["links"] = json_link_array;
    return json_inf;
}

void Serializer::json_to_editor(NodeEditor *_editor, json &data)
{
    if (!_editor)
    {
        return;
    }
    if (data.contains("vm"))
    {
        json_to_vm(_editor->getVM(), _editor->getINF(), *_editor, data["vm"]);
    }
    if (data.contains("inf"))
    {
        auto &vm = *_editor->getVM();
        auto &inf = *_editor->getINF();
        for (auto link : data["inf"]["links"])
        {
            VM::PinId left = 0, right = 0;
            if (link.contains("left"))
                left = link["left"].get<size_t>();
            if (link.contains("right"))
                right = link["right"].get<size_t>();

            if (left == 0 || right == 0)
                continue;

            auto outNodeId = vm.getNode(vm.getPin(left).owner).id;
            auto outNodes = inf.getNodes();
            auto outNode = outNodes[outNodeId];
            if (!outNode)
                continue;
            auto &outputs = outNode->getOuts();

            auto inNodeId = vm.getNode(vm.getPin(right).owner).id;
            auto inNodes = inf.getNodes();
            auto inNode = inNodes[inNodeId];
            if (!inNode)
                continue;
            auto &inputs = inNode->getIns();

            auto outIt = std::find_if(outputs.begin(), outputs.end(), [left](std::shared_ptr<ImFlow::Pin> p)
                                      { return p->getUid() == left; });
            if (outIt == outputs.end())
                continue;

            auto inIt = std::find_if(inputs.begin(), inputs.end(), [right](std::shared_ptr<ImFlow::Pin> p)
                                     { return p->getUid() == right; });
            if (inIt == inputs.end())
                continue;

            (*outIt)->createLink(inIt->get());
        }
    }
}

void Serializer::json_to_vm(VM *_vm, ImFlow::ImNodeFlow *_inf, NodeEditor &editor, json &data)
{
    if (!_vm)
    {
        return;
    }
    auto &vm = *_vm;
    if (!data.contains("nodes"))
    {
        return;
    }

    if (!_inf)
    {
        return;
    }
    auto &inf = *_inf;
    vm.m_nodeData.clear();

    for (const auto &json_node : data["nodes"])
    {
        VM::Node node;
        if (json_node.contains("id"))
            node.id = json_node["id"].get<size_t>();
        if (json_node.contains("type"))
            node.type = static_cast<VM::NodeType>(json_node["type"].get<int>());
        if (json_node.contains("name"))
            node.name = json_node["name"].get<std::string>();
        if (json_node.contains("n_data"))
            node.n_data = json_node["n_data"].get<size_t>();
        if (json_node.contains("calc"))
            node.computeIO = json_node["calc"].get<size_t>();
        if (json_node.contains("draw"))
            node.draw = json_node["draw"].get<size_t>();

        if (json_node.contains("inputs"))
        {
            for (const auto &json_input : json_node["inputs"])
            {
                VM::Node::IOPin input;
                if (json_node.contains("size"))
                    input.size = json_input["size"].get<size_t>();
                if (json_node.contains("name"))
                    input.name = json_input["name"].get<std::string>();
                if (json_node.contains("owner"))
                    input.owner = json_input["owner"].get<size_t>();
                if (json_node.contains("id"))
                    input.id = json_input["id"].get<size_t>();
                node.inputs.push_back(input);
            }
        }

        if (json_node.contains("outputs"))
        {
            for (const auto &json_output : json_node["outputs"])
            {
                VM::Node::IOPin output;
                if (json_node.contains("size"))
                    output.size = json_output["size"].get<size_t>();
                if (json_node.contains("name"))
                    output.name = json_output["name"].get<std::string>();
                if (json_node.contains("owner"))
                    output.owner = json_output["owner"].get<size_t>();
                if (json_node.contains("id"))
                    output.id = json_output["id"].get<size_t>();
                node.outputs.push_back(output);
            }
        }

        ImVec2 pos;
        if (json_node.contains("pos_x"))
            pos.x = json_node["pos_x"].get<float>();
        if (json_node.contains("pos_y"))
            pos.y = json_node["pos_y"].get<float>();

        editor.addNode(node, pos);
    }

    if (data.contains("nodeId"))
    {
        vm.m_NodeIdCounter = data["nodeId"].get<size_t>();
    }
    if (data.contains("pinId"))
    {
        vm.m_PinIdCounter = data["pinId"].get<size_t>();
    }
}
