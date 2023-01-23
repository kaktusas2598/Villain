#include "DebugConsole.hpp"

#include "Engine.hpp"
//#include "Level.hpp"
#include "ScriptEngine.hpp"

namespace Villain {

    bool DebugConsole::layerVisibility = false;
    bool DebugConsole::colliderVisibility = false;
    bool DebugConsole::showDemoWindow = false;

    DebugConsole* DebugConsole::s_pInstance = nullptr;

    DebugConsole::DebugConsole() {
        clearLog();
        memset(inputBuf, 0, sizeof(inputBuf));
        historyPos = -1;

        // TODO: is there a way to register lua binders more cleanly
        commands.push_back("print");
        commands.push_back("playSound");
        commands.push_back("playMusic");
        commands.push_back("changeState");
        commands.push_back("quit()");
        commands.push_back("Entity:create()");
        commands.push_back(":addSprite");
        commands.push_back("addParticleEmitter");
        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        //Commands.push_back("HELP");
        //Commands.push_back("HISTORY");
        commands.push_back("CLEAR");
        //Commands.push_back("CLASSIFY");
        autoScroll = true;
        scrollToBottom = false;
        addLog("Vigilant Debug Console");
    }
    DebugConsole::~DebugConsole() {
        clearLog();
        for (int i = 0; i < history.Size; i++)
            free(history[i]);
    }

    void DebugConsole::addLog(const char* fmt, ...) { // IM_FMTARGS(2) ???
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
        //items.push_back(Strdup(buf));
        items.push_back(strdup(buf));

        //char tmp[strlen(fmt) + 1];
        //strcpy(tmp, fmt);
        //items.push_back(strdup(tmp));
    }

    void DebugConsole::clearLog() {
        for (int i = 0; i < items.Size; i++)
            free(items[i]);
        items.clear();
    }

    void DebugConsole::exec(const char* command) {
        addLog("# %s\n", command);

        // add to history, find a match and delete it so it can be pushed to the back
        // this is not smart or optional
        historyPos = -1;
        for (int i = history.Size - 1; i >= 0; i--) {
            // Stricmp would be better?
            if(strcmp(history[i], command) == 0) {
                free(history[i]);
                history.erase(history.begin() + i);
                break;
            }
        }
        history.push_back(strdup(command));

        // Process all non Lua console commands
        if (strcmp(command, "CLEAR") == 0) {
            clearLog();
            return;
        }

        // run luachunk
        std::string output = ScriptEngine::Instance()->runChunk(command);
        if (!output.empty())
            addLog(output.c_str());

        scrollToBottom = true;
    }

    void DebugConsole::render() {
        ImGui::Begin("Debug Console");

        //ImGui::Checkbox("Show IMGui Demo Window", &showDemoWindow);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //ImGui::Text("Active entities: %d", EntityManager::livingCount);
        ImGui::Checkbox("Show collision layers", &layerVisibility);
        ImGui::Checkbox("Show colliders", &colliderVisibility);
        ImGui::Separator();

        //Level* level = TheEngine::Instance()->getLevel();
        //if (level != nullptr) {
            //int i = 0;
            //for (auto it = level->getCollisionLayers()->begin(); it != level->getCollisionLayers()->end(); ++it) {
                //(*it)->setVisible(layerVisibility);
                //i++;
            //}
        //}

        // Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //ImGui::ShowDemoWindow(&showDemoWindow);

        // Lua Console
        //----------------------------------
        ImGui::Separator();
        if (ImGui::SmallButton("Clear")) {
            clearLog();
        }
        ImGui::SameLine();
        bool copyToClipboard = ImGui::SmallButton("Copy");

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar)) {
            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::Selectable("Clear")) clearLog();
                ImGui::EndPopup();
            }

            // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
            if (copyToClipboard)
                ImGui::LogToClipboard();
            for (int i = 0; i < items.Size; i++) {
                const char* item = items[i];
                if (!filter.PassFilter(item))
                    continue;

                // Normally you would store more information in your item than just a string.
                // (e.g. make Items[] an array of structure, store color/type etc.)
                ImVec4 color;
                bool has_color = false;
                if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
                else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
                else if (strstr(item, "[lua]") == 0) { color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f); has_color = true; }
                else if (strstr(item, "[info]") == 0) { color = ImVec4(0.44f, 0.7f, 0.09f, 1.0f); has_color = true; }
                if (has_color)
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(item);
                if (has_color)
                    ImGui::PopStyleColor();
            }
            if (copyToClipboard)
                ImGui::LogFinish();

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);
            scrollToBottom = false;

            ImGui::PopStyleVar();

        }
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool reclaimFocus = false;
        ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        // FIXME: how to ignore all input in engine while typing comand in console
        if (ImGui::InputText("Lua Input", inputBuf, IM_ARRAYSIZE(inputBuf), inputTextFlags, &textEditCallbackStub, (void*)this)) {
            char* s = inputBuf;
            //Strtrim(s);
            if (s[0])
                exec(s);
            strcpy(s, "");
            reclaimFocus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaimFocus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
        //----------------------------------


        ImGui::End();
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    int DebugConsole::textEditCallbackStub(ImGuiInputTextCallbackData* data) {
        return DebugConsole::Instance()->textEditCallback(data);
    }

    int DebugConsole::textEditCallback(ImGuiInputTextCallbackData* data) {
        switch (data->EventFlag) {
            // Autocompletion logic
            case ImGuiInputTextFlags_CallbackCompletion:
                {
                    // Locate beginning of current word
                    const char* wordEnd = data->Buf + data->CursorPos;
                    const char* wordStart = wordEnd;
                    while (wordStart > data->Buf) {
                        const char c = wordStart[-1];
                        if (c == ' ' || c == '\t' || c == ',' || c == ';')
                            break;
                        wordStart--;
                    }

                    //build a list of candidate commands
                    ImVector<const char*> candidates;
                    for (int i = 0; i < commands.Size; i++) {
                        // TODO: ignore case
                        if (strncmp(commands[i], wordStart, (int)(wordEnd - wordStart)) == 0)
                            candidates.push_back(commands[i]);
                    }

                    if (candidates.Size == 0) {
                        // No match
                        addLog("No match for \"%.*s\"!\n", (int)(wordEnd - wordStart), wordStart);
                    } else if (candidates.Size == 1) {
                        // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                        data->DeleteChars((int)(wordStart - data->Buf), (int)(wordEnd - wordStart));
                        data->InsertChars(data->CursorPos, candidates[0]);
                        data->InsertChars(data->CursorPos, " ");
                    } else {
                        // Multiple matches, complete as much as possible
                        int matchLen = (int)(wordEnd - wordStart);
                        for(;;) {
                            int c = 0;
                            bool allCandidatesMatches = true;
                            for (int i = 0; i < candidates.Size && allCandidatesMatches; i++) {
                                if (i == 0) {
                                    c = toupper(candidates[i][matchLen]);
                                } else if (c == 0 || c != toupper(candidates[i][matchLen])) {
                                    allCandidatesMatches = false;
                                }
                            }
                            if (!allCandidatesMatches) {
                                break;
                            }
                            matchLen++;
                        }

                        if (matchLen > 0) {
                            data->DeleteChars((int)(wordStart - data->Buf), (int)(wordEnd - wordStart));
                            data->InsertChars(data->CursorPos, candidates[0], candidates[0] + matchLen);
                        }

                        // List matches
                        addLog("Possible matches:\n");
                        for (int i = 0; i < candidates.Size; i++)
                            addLog("- %s\n", candidates[i]);
                    }
                }
                break;
                // Command history (up and down arrows)
            case ImGuiInputTextFlags_CallbackHistory:
                {
                    const int previousHistoryPos = historyPos;
                    if (data->EventKey == ImGuiKey_UpArrow) {
                        if (historyPos == -1)
                            historyPos = history.Size - 1;
                        else if (historyPos > 0)
                            historyPos--;
                    } else if (data->EventKey == ImGuiKey_DownArrow) {
                        if (historyPos != -1)
                            if (++historyPos >= history.Size)
                                historyPos = -1;
                    }

                    // A better implementation would preserve the data on the current input line along with cursor position.
                    if (previousHistoryPos != historyPos) {
                        const char* historyStr = (historyPos >= 0) ? history[historyPos] : "";
                        data->DeleteChars(0, data->BufTextLen);
                        data->InsertChars(0, historyStr);
                    }
                }
                break;
        }
        return 0;
    }
}
