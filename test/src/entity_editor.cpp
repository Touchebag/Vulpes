#include <gtest/gtest.h>

#include "editor/entity_editor/components/scripting_editor.h"
#include "components/scripting/program.h"

using entity_editor::ScriptingEditor;
using scripting::Type;

namespace {

std::string printSet(std::set<std::string>& input) {
    std::string output;

    for (auto& it : input) {
        output += (it);
        output += (" ");
    }

    return output;
}

} // namespace

TEST(TestScriptingEditor, TestVoidType) {
    // Program must start with void
    auto tokens = Program::tokenizeString("");

    scripting::Type expected_type = Type::VOID;
    auto actual_type = ScriptingEditor::getExpectedType(tokens);

    EXPECT_TRUE(actual_type == expected_type);
}

TEST(TestScriptingEditor, TestInvalidInput) {
    auto tokens = Program::tokenizeString("invalid");

    EXPECT_THROW({
        try {
            ScriptingEditor::getExpectedType(tokens);
        } catch (const std::invalid_argument& e) {
            EXPECT_STREQ("Invalid token", e.what());
            throw;
        }
    }, std::invalid_argument);
}

TEST(TestScriptingEditor, TestIncompleteString) {
    auto tokens = Program::tokenizeString("if (grt 2 1) then (action");

    scripting::Type expected_type = Type::STRING;
    auto actual_type = ScriptingEditor::getExpectedType(tokens);

    EXPECT_TRUE(actual_type == expected_type);
}

TEST(TestScriptingEditor, TestPartialToken) {
    std::string input = "if (a";
    auto tokens = Program::tokenizeString(input);

    EXPECT_THROW({
        try {
            ScriptingEditor::getExpectedType(tokens);
        } catch (const std::invalid_argument& e) {
            EXPECT_STREQ("Invalid token", e.what());
            throw;
        }
    }, std::invalid_argument);

    std::set<std::string> expected_suggestions;
    expected_suggestions.insert("and");
    expected_suggestions.insert("animation_looped");

    auto actual_suggestions = ScriptingEditor::getAutoCompletionSuggestions(input);
    auto expected_suggestions_str = printSet(expected_suggestions);
    auto actual_suggestions_str = printSet(actual_suggestions);

    EXPECT_TRUE(actual_suggestions == expected_suggestions)
        << "Actual: { " << actual_suggestions_str << "}" << std::endl
        << "Expected: { " << expected_suggestions_str << "}";

}
