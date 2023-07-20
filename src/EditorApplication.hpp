#pragma once

#include "Application.hpp"

namespace Villain::Editor {

    class EditorApplication : public Villain::Application {
        public:
            // Ovverride to disable automatically trying to load state.xml, not the best approach
            virtual void addStates() override {}
    };
}
