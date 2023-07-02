#pragma once

#include "ErrorHandler.hpp"
#include "LuaScript.hpp"
#include "editor/DebugConsole.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Villain {

    // Main Villain Engine logging utility
    class Logger {
        public:
            static Logger* Instance() {
                if(s_pInstance == 0) {
                    s_pInstance = new Logger();
                    return s_pInstance;
                }
                return s_pInstance;
            }

            void info(const char* message) {
                fileLogger->info(message);
                consoleLogger->info(message);
                DebugConsole::Instance()->addLog("[INFO]: %s", message);
            }

            void warn(const char* message) {
                fileLogger->warn(message);
                consoleLogger->warn(message);
                DebugConsole::Instance()->addLog("[WARN]: %s", message);
            }

            void error(const char* message) {
                fileLogger->error(message);
                consoleLogger->error(message);
                DebugConsole::Instance()->addLog("[ERROR]: %s", message);
            }

            void dumpStack (lua_State *L);
        private:
            Logger() {
                fileLogger = spdlog::basic_logger_mt("Villain Engine Log", "debug.log", true);
                consoleLogger = spdlog::stdout_color_mt("console");
                if (!fileLogger) {
                    exitWithError("Debug file couldn't be opened");
                }
            }
            ~Logger() {}

            static Logger* s_pInstance;

            std::shared_ptr<spdlog::logger> fileLogger;
            std::shared_ptr<spdlog::logger> consoleLogger;
    };
}
