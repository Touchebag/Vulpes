#include "scripting_editor.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <stack>
#include <variant>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "components/scripting/program.h"
#include "components/component_store.h"

#include "utils/log.h"

namespace {

using entity_editor::ScriptingEditor;

int autoCompleteCallback(ImGuiInputTextCallbackData* input_data) {
    auto formatted_string = ScriptingEditor::autoCompleteProgramString(input_data->Buf);

    input_data->DeleteChars(0, input_data->BufTextLen);
    input_data->InsertChars(0, formatted_string.c_str());

    return 0;
}

// Assumes string is trimmed of special characters: ()'
std::string formatProgramStringRecursive(std::deque<std::string>& input, bool add_parens) {
    if (input.empty()) {
        return "";
    }

    std::string output;

    auto token = input.front();
    input.pop_front();

    try {
        auto type_info = scripting::string_instruction_map.at(token);

        if (!type_info.args_return_type.empty()) {
            // Don't add parentheses on outer instruction
            if (add_parens) {
                output.append("(");
            }

            output.append(token);

            for (unsigned long long i = 0; i < type_info.args_return_type.size(); i++) {
                if (input.empty()) {
                    // If string not complete, return what we have
                    return output;
                }

                output.append(" ");
                output.append(formatProgramStringRecursive(input, true));
            }

            if (add_parens) {
                output.append(")");
            }
        } else {
            output.append(token);
        }
    } catch (const std::out_of_range& e) {
        // If not instruction, try parse int
        try {
            std::unique_ptr<size_t> n = std::make_unique<size_t>();
            (void)std::stoi(token, n.get());

            // Check if entire string could be parsed as int
            if (*n != token.length()) {
                throw std::invalid_argument("Invalid input");
            }

            output.append(token);
        } catch (std::invalid_argument&) {
            // If not int, assume string

            // Re-add the correct amount of quotation marks
            output.append("'");
            output.append(token);
            output.append("'");
        }
    }

    return output;
}

} // namespace

namespace entity_editor {

void ScriptingEditor::setActive() {
    active_ = true;
}

scripting::Type ScriptingEditor::getExpectedType(std::vector<std::string> tokens) {
    std::stack<scripting::Type> type_stack;

    type_stack.push(scripting::Type::VOID);

    for (auto it = tokens.begin(); it != tokens.end(); it++) {
        auto expected_type = type_stack.top();

        if (*it == "(" || *it == ")") {
            continue;
        } else {
            try {
                auto instruction = scripting::string_instruction_map.at(*it);

                if (expected_type == instruction.return_type) {
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
                if (expected_type == scripting::Type::STRING) {
                    if (it->size() > 1 && it->front() == '\'' && it->back() == '\'') {
                        type_stack.pop();
                    } else {
                        throw std::invalid_argument("Invalid string format");
                    }
                // Check if int
                } else if (expected_type == scripting::Type::INT) {
                    try {
                        std::unique_ptr<size_t> n = std::make_unique<size_t>();
                        (void)std::stoi(*it, n.get());
                        // Check if entire string could be parsed as int
                        if (*n == it->length()) {
                            type_stack.pop();
                        }
                    } catch (std::invalid_argument&) {
                        throw std::invalid_argument("Invalid int format");
                    }
                } else {
                    throw std::invalid_argument("Invalid token");
                }
            }
        }

        if (type_stack.empty()) {
            return scripting::Type::NONE;
        }
    }

    return type_stack.top();
}

std::pair<std::string, std::set<std::string>> ScriptingEditor::getAutoCompletionSuggestions(const std::string& input) {
    // auto tokens = Program::tokenizeString(input);
    // std::string last_token;
    //
    // if (!tokens.empty()) {
    //     if (scripting::string_instruction_map.count(tokens.back()) < 1) {
    //         if (last_token == ")" || last_token == "(") {
    //             // Ignore parentheses
    //         } else {
    //             // Remove last token if partial
    //             last_token = tokens.back();
    //             tokens.pop_back();
    //         }
    //     }
    // }
    //
    // auto expected_type = getExpectedType(tokens);
    //
    // std::set<std::string> suggestions;
    //
    // for (auto it : scripting::string_instruction_map) {
    //     if (it.second.return_type == expected_type) {
    //         // Check if starts with
    //         if (it.first.rfind(last_token, 0) == 0) {
    //             suggestions.insert(it.first);
    //         }
    //     }
    // }
    //
    // std::string input_without_partial_token;
    // for (auto it = tokens.begin(); it != tokens.end(); it++) {
    //     input_without_partial_token += *it;
    //
    //     // Don't add space et end
    //     if (it != tokens.end() - 1) {
    //         input_without_partial_token += " ";
    //     }
    // }
    //
    // return {input_without_partial_token, suggestions};
    return {{}, {}};
}

std::string ScriptingEditor::formatProgramString(std::string input) {
    std::string output;

    auto trimmed_string = std::string();

    for (auto it : input) {
        if (it == '\''|| it == '('  || it == ')') {
            it++;
        } else {
            trimmed_string += it;
        }
    }
    //
    // auto tokens = Program::tokenizeString(trimmed_string);
    // auto queue = std::deque<std::string>(tokens.begin(), tokens.end());
    //
    // output = formatProgramStringRecursive(queue, false);
    //
    // // Check if output type checks
    // tokens = Program::tokenizeString(output);
    // if (getExpectedType(tokens) != scripting::Type::NONE) {
    //     // If output is partial, append space for next token
    //     output.append(" ");
    // }

    return output;
}

std::string ScriptingEditor::autoCompleteProgramString(std::string input) {
    try {
        auto [rest_of_string, suggestions] = ScriptingEditor::getAutoCompletionSuggestions(input);

        if (suggestions.size() == 1) {
            rest_of_string += " ";
            rest_of_string += *suggestions.begin();
        }

        input = ScriptingEditor::formatProgramString(rest_of_string);
    } catch (const std::invalid_argument& e) {
        // Do nothing if string doesn't type check
    }

    return input;
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
                bool text_entered = ImGui::InputText("##script_text_input", text_buf_, IMGUI_BUFFER_SIZE, ImGuiInputTextFlags_CallbackCompletion, autoCompleteCallback);
                // Save position for auto completion popup
                ImVec2 input_field_position = {ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y};

                if (text_entered) {
                    try {
                        completion_values_ = getAutoCompletionSuggestions({text_buf_}).second;
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
