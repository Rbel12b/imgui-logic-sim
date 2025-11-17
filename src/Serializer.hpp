#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "VM.hpp"
#include "Nodes.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Serializer
{
    json editor_to_json(NodeEditor *_editor);
    json vm_to_json(VM *_vm, ImFlow::ImNodeFlow *_inf);
    json inf_to_json(ImFlow::ImNodeFlow *_inf);

    void json_to_editor(NodeEditor *_editor, json &data);
    void json_to_vm(VM *_vm, ImFlow::ImNodeFlow *_inf, NodeEditor &editor, json &data);
};