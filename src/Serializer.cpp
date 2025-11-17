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
    json_editor["version"] = 1; // versioning for future compatibility, not the actual app version, just the file format version
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
        json_node["i"] = node.id;
        json_node["t"] = static_cast<int>(node.type);
        json_node["n"] = node.name;
        json_node["u"] = node.n_data;
        json_node["c"] = node.computeIO;
        json_node["d"] = node.draw;
        json_node["x"] = pos.x;
        json_node["y"] = pos.y;

        std::vector<json> json_input_array;
        for (const auto &input : node.inputs)
        {
            json json_input;
            json_input["s"] = input.size;
            json_input["n"] = input.name;
            json_input["o"] = input.owner;
            json_input["i"] = input.id;
            json_input_array.push_back(json_input);
        }
        json_node["is"] = json_input_array;

        std::vector<json> json_output_array;
        for (const auto &output : node.outputs)
        {
            json json_output;
            json_output["s"] = output.size;
            json_output["n"] = output.name;
            json_output["o"] = output.owner;
            json_output["i"] = output.id;
            json_output_array.push_back(json_output);
        }
        json_node["os"] = json_output_array;
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
        json_link["l"] = link->left()->getUid();
        json_link["r"] = link->right()->getUid();
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
    if (data.contains("version"))
    {
        int version = data["version"].get<int>();
        if (version != 1)
        {
            return;
        }
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
            if (link.contains("l"))
                left = link["l"].get<size_t>();
            if (link.contains("r"))
                right = link["r"].get<size_t>();

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
        if (json_node.contains("i"))
            node.id = json_node["i"].get<size_t>();
        if (json_node.contains("t"))
            node.type = static_cast<VM::NodeType>(json_node["t"].get<int>());
        if (json_node.contains("n"))
            node.name = json_node["n"].get<std::string>();
        if (json_node.contains("u"))
            node.n_data = json_node["u"].get<size_t>();
        if (json_node.contains("c"))
            node.computeIO = json_node["c"].get<size_t>();
        if (json_node.contains("d"))
            node.draw = json_node["d"].get<size_t>();

        if (json_node.contains("is"))
        {
            for (const auto &json_input : json_node["is"])
            {
                VM::Node::IOPin input;
                if (json_node.contains("s"))
                    input.size = json_input["s"].get<size_t>();
                if (json_node.contains("n"))
                    input.name = json_input["n"].get<std::string>();
                if (json_node.contains("o"))
                    input.owner = json_input["o"].get<size_t>();
                if (json_node.contains("i"))
                    input.id = json_input["i"].get<size_t>();
                node.inputs.push_back(input);
            }
        }

        if (json_node.contains("os"))
        {
            for (const auto &json_output : json_node["os"])
            {
                VM::Node::IOPin output;
                if (json_node.contains("s"))
                    output.size = json_output["s"].get<size_t>();
                if (json_node.contains("n"))
                    output.name = json_output["n"].get<std::string>();
                if (json_node.contains("o"))
                    output.owner = json_output["o"].get<size_t>();
                if (json_node.contains("i"))
                    output.id = json_output["i"].get<size_t>();
                node.outputs.push_back(output);
            }
        }

        ImVec2 pos;
        if (json_node.contains("x"))
            pos.x = json_node["x"].get<float>();
        if (json_node.contains("y"))
            pos.y = json_node["y"].get<float>();

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
