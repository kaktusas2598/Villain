#ifndef __DebugConsole__
#define __DebugConsole__

#include "imgui/imgui.h"

#include <map>
#include <string>
#include <iostream>

namespace Villain {

    struct DebugConsole {
        public:
            static DebugConsole* Instance() {
                if (s_pInstance == 0) {
                    s_pInstance = new DebugConsole();
                    return s_pInstance;
                }
                return s_pInstance;
            }

            char inputBuf[256];
            ImVector<char*> items;
            ImVector<const char*> commands;
            ImVector<char*> history;
            int historyPos; // -1: new line, 0..History.Size-1 browsing history.
            ImGuiTextFilter filter;
            bool autoScroll;
            bool scrollToBottom;

            void addLog(const char* fmt, ...);// IM_FMTARGS(2)
            void clearLog();

            void exec(const char* command);

            void render();

            static bool layerVisibility; ///< Toggle collision layer render
            static bool colliderVisibility; ///< Toggle collider component render
            static bool showDemoWindow; ///< Toggle IMGui Demo Window for Docs
            static int textEditCallbackStub(ImGuiInputTextCallbackData* data);
            int textEditCallback(ImGuiInputTextCallbackData* data);

            static std::map<std::string, std::string> customInfo; ///< Display custom information in Console
            void setInfo(const std::string& key, const std::string& value) { customInfo[key] = value; }

            static ImVec4 clearColor;

        private:
            DebugConsole();
            ~DebugConsole();

            static DebugConsole* s_pInstance;
    };
}

#endif // __DebugConsole__
