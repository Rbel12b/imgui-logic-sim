#include "Serializer.hpp"

json Serializer::editor_to_json(NodeEditor *_editor)
{
    if (!_editor)
    {
        return json();
    }
    auto& editor = *_editor;
    json json_editor;
    json_editor["vm"] = vm_to_json(editor.getVM());
    json_editor["inf"] = inf_to_json(editor.getINF());
    return json_editor;
}

json Serializer::vm_to_json(VM *_vm)
{
    if (!_vm)
    {
        return json();
    }
    auto& vm = *_vm;
    json json_vm;
    std::vector<json> json_node_array;
    for (const auto& [id, node] : vm.m_nodeData)
    {
        json json_node;
        json_node["id"] = node.id;
        json_node["type"] = static_cast<int>(node.type);
        json_node["name"] = node.name;
        json_node["n_data"] = node.n_data;
        json_node["calcIO"] = node.computeIO;

        std::vector<json> json_input_array;
        for (const auto& input : node.inputs)
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
        for (const auto& output : node.outputs)
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
    auto& inf = *_inf;
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
