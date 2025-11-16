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
    json editor_to_json(NodeEditor* _editor);
    json vm_to_json(VM* _vm);
    json inf_to_json(ImFlow::ImNodeFlow* _inf);
};