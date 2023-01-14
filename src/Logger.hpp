#ifndef __Logger__
#define __Logger__

#include "ErrorHandler.hpp"
#include "LuaScript.hpp"
#include "DebugConsole.hpp"
#include <fstream>

/**
 * This class is only used for internal logging
*/
namespace Villain {
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
                logFile << "[INFO]: " << message << std::endl;
                std::string output = message;
                output = "[info]: " + output;
                DebugConsole::Instance()->addLog(output.c_str());
            }

            void error(const char* message) {
                logFile << "[ERROR]: " << message << std::endl;
                std::string output = message;
                output = "[error]: " + output;
                DebugConsole::Instance()->addLog(output.c_str());
            }

            void dumpStack (lua_State *L);
        private:
            Logger() {
                logFile.open("debug.log");
                if( !logFile ) { // file couldn't be opened
                    exitWithError("Debug file couldn't be opened");
                }
            }
            ~Logger() {
                logFile.close();
            }
            static Logger* s_pInstance;

            std::ofstream logFile;
    };
}

#endif // __Logger__
