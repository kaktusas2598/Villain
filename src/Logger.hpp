#pragma once

#include "ErrorHandler.hpp"
#include "LuaScript.hpp"
#include "editor/DebugConsole.hpp"
#include "spdlog/common.h"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// Logging macros to simplify logging in the engine
#define VILLAIN_TRACE(...) Villain::Logger::Instance()->trace(__VA_ARGS__)
#define VILLAIN_DEBUG(...) Villain::Logger::Instance()->debug(__VA_ARGS__)
#define VILLAIN_INFO(...) Villain::Logger::Instance()->info(__VA_ARGS__)
#define VILLAIN_WARN(...) Villain::Logger::Instance()->warn(__VA_ARGS__)
#define VILLAIN_ERROR(...) Villain::Logger::Instance()->error(__VA_ARGS__)
#define VILLAIN_CRIT(...) Villain::Logger::Instance()->critical(__VA_ARGS__)

namespace Villain {

    // Main Villain Engine logging utility, using spdlog and variadic templates to log formatted messages
    class Logger {
        public:
            static Logger* Instance() {
                if(s_pInstance == 0) {
                    s_pInstance = new Logger();
                    return s_pInstance;
                }
                return s_pInstance;
            }

            template<typename... Args>
            void trace(const char* fmt, const Args&... args) {
                fileLogger->trace(fmt, args...);
                consoleLogger->trace(fmt, args...);
                DebugConsole::Instance()->addLog("[TRACE]: %s", fmt::format(fmt, args...).c_str());
            }

            template<typename... Args>
            void debug(const char* fmt, const Args&... args) {
                fileLogger->debug(fmt, args...);
                consoleLogger->debug(fmt, args...);
                DebugConsole::Instance()->addLog("[DEBUG]: %s", fmt::format(fmt, args...).c_str());
            }

            template<typename... Args>
            void info(const char* fmt, const Args&... args) {
                fileLogger->info(fmt, args...);
                consoleLogger->info(fmt, args...);
                DebugConsole::Instance()->addLog("[INFO]: %s", fmt::format(fmt, args...).c_str());
            }

            template<typename... Args>
            void warn(const char* fmt, const Args&... args) {
                fileLogger->warn(fmt, args...);
                consoleLogger->warn(fmt, args...);
                DebugConsole::Instance()->addLog("[WARN]: %s", fmt::format(fmt, args...).c_str());
            }

            template<typename... Args>
            void error(const char* fmt, const Args&... args) {
                fileLogger->error(fmt, args...);
                consoleLogger->error(fmt, args...);
                DebugConsole::Instance()->addLog("[ERROR]: %s", fmt::format(fmt, args...).c_str());
            }

            template<typename... Args>
            void critical(const char* fmt, const Args&... args) {
                fileLogger->critical(fmt, args...);
                consoleLogger->critical(fmt, args...);
                DebugConsole::Instance()->addLog("[CRIT]: %s", fmt::format(fmt, args...).c_str());
            }

            void dumpStack (lua_State *L);
        private:
            Logger() {
                spdlog::set_pattern("%^[%H:%M:%S.%e %l] : %v%$");
                // TODO: Set different level for release builds
                spdlog::set_level(spdlog::level::trace);
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
