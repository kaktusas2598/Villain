#include "EditorApplication.hpp"

int main(int argc, char *argv[]) {

    Villain::Editor::EditorApplication editor;

    Villain::Engine engine;
    engine.init(
            &editor,
            "Villain Editor",
            800,
            600,
            0,
            true
        );

    engine.setEditMode(true);
    engine.run();

    return 0;
}
