#include "scripting_editor.h"

#include <memory>
#include <stdexcept>
#include <stack>
#include <variant>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "components/scripting/program.h"

#include "utils/log.h"

namespace entity_editor {

void ScriptingEditor::setActive() {
    active_ = true;
}

scripting::Type ScriptingEditor::getExpectedType(std::vector<std::string> tokens) {
    std::stack<scripting::Type> type_stack;

    type_stack.push(scripting::Type::VOID);

    for (auto it : tokens) {
        auto expected_type = type_stack.top();

        if (it == "(" || it == ")") {
            continue;
        } else {
            auto type = expected_type;

            try {
                auto instruction = scripting::string_instruction_map.at(it);

                if (type == instruction.return_type) {
                    // Remove return type and replace with argument types
                    type_stack.pop();

                    for (auto arg_type = instruction.args_return_type.rbegin();
                         arg_type != instruction.args_return_type.rend();
                         arg_type++) {
                        type_stack.push(*arg_type);
                    }
                } else {
                    throw std::invalid_argument("Invalid type");
                }
            } catch (const std::out_of_range& e) {
                // Check if string
                if (type == scripting::Type::STRING) {
                    if (it.size() > 1 && it.front() == '\'' && it.back() == '\'') {
                        type_stack.pop();
                    }
                // Check if int
                } else if (type == scripting::Type::INT) {
                    try {
                        std::unique_ptr<size_t> n = std::make_unique<size_t>();
                        (void)std::stoi(it, n.get());
                        // Check if entire string could be parsed as int
                        if (*n == it.length()) {
                            type_stack.pop();
                        }
                    } catch (std::invalid_argument&) {
                        throw std::invalid_argument("Invalid int format");
                    }
                } else {
                    // TODO Match half words
                    throw std::invalid_argument("Invalid token");
                }
            }
        }
    }

    if (type_stack.empty()) {
        throw std::invalid_argument("Type stack empty");
    } else {
        return type_stack.top();
    }
}

std::set<std::string> ScriptingEditor::getAutoCompletionSuggestions(const std::string& input) {
    auto tokens = Program::tokenizeString(input);
    std::string last_token;

    if (!tokens.empty()) {
        if (scripting::string_instruction_map.count(tokens.back()) < 1) {
            // Remove last token if partial
            last_token = tokens.back();
            tokens.pop_back();

            // Ignore parentheses
            if (last_token == ")" || last_token == "(") {
                last_token.clear();
            }
        }
    }

    auto expected_type = getExpectedType(tokens);

    std::set<std::string> suggestions;

    for (auto it : scripting::string_instruction_map) {
        if (it.second.return_type == expected_type) {
            // Check if starts with
            if (it.first.rfind(last_token, 0) == 0) {
                suggestions.insert(it.first);
            }
        }
    }

    return suggestions;
}

void ScriptingEditor::drawMenu(std::vector<std::string>& script) {
    if (active_) {
        ImGui::Begin("Scripting", nullptr, 0
            | ImGuiWindowFlags_AlwaysAutoResize
            );

        for (int i = 0; i < static_cast<int>(script.size()); i++) {
            ImGui::PushID(std::to_string(i).c_str());
            if (current_edit_ == i) {
                if (ImGui::Button("Done")) {
                    script[i] = text_buf_;
                    current_edit_ = - 1;
                }

                ImGui::SameLine();
                bool text_entered = ImGui::InputText("##script_text_input", text_buf_, IMGUI_BUFFER_SIZE);
                // Save position for auto completion popup
                ImVec2 input_field_position = {ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y};

                if (text_entered) {
                    try {
                        completion_values_ = getAutoCompletionSuggestions({text_buf_});
                    } catch (const std::invalid_argument&) {
                        // Do nothing if failed to parse
                    }
                }

                bool tooltip_open = !completion_values_.empty();
                if (tooltip_open) {

                    ImGui::SetNextWindowPos(input_field_position);

                    ImGui::BeginTooltip();
                    for (auto it : completion_values_) {
                        ImGui::Text(it.c_str());
                    }
                    ImGui::EndTooltip();
                }

                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    // Keep old value on cancel
                    current_edit_ = - 1;
                }
            } else {
                if (ImGui::Button("Edit")) {
                    strncpy(text_buf_, script[i].c_str(), IMGUI_BUFFER_SIZE - 1);
                    current_edit_ = i;
                }

                ImGui::SameLine();
                ImGui::Text("%s", script[i].c_str());
            }
            ImGui::PopID();
        }

        if (ImGui::Button("Close")) {
            active_ = false;
        }

        ImGui::End();
    }
}

} // entity_editor
