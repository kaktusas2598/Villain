#include "ScriptEditor.hpp"

#include "FileUtils.hpp"
#include "imgui/imgui.h"

namespace Villain {

    void ScriptEditor::setScript(const std::string& fileName) {
        filePath = fileName;
        text = FileUtils::loadFile(fileName);
        bufferSize = text.size() + 2000;
        shouldSetFocus = true;
    }

    void ScriptEditor::render() {
        if (shouldSetFocus) {
            ImGui::SetNextWindowFocus();
            shouldSetFocus = false;
        }
        ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

        float textBoxHeight = ImGui::GetContentRegionAvail().y - ImGui::GetTextLineHeightWithSpacing();
        static char buffer[100000];
        strncpy(buffer, text.c_str(), sizeof(buffer));
        // Text area for editing the script
        // FIXME: Loading fine, but not saving, either issue with this widget or FileUtils::saveFile()
        //ImGui::InputTextMultiline("##ScriptArea", &text[0], 100000, ImVec2(-1.0f, textBoxHeight), ImGuiInputTextFlags_AllowTabInput);
        //ImGui::InputTextMultiline("##ScriptArea", text.data(), bufferSize, ImVec2(-1.0f, textBoxHeight), ImGuiInputTextFlags_AllowTabInput);
        if (ImGui::InputTextMultiline("##ScriptArea", buffer, sizeof(buffer), ImVec2(-1.0f, textBoxHeight), ImGuiInputTextFlags_AllowTabInput)) {
            text = buffer;
        }

        // Save button
        if (ImGui::Button("Save")) {
            // TODO: Dispatch some kind of event so that ScriptComponent can subscribe and hot reload script
            FileUtils::saveFile(filePath, text);
        }

        ImGui::End();
    }
}
