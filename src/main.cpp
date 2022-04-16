#include "GUI.hpp"

int main() {
    GUI gui; 

    gui.Initialize();
    gui.RunLoop();
    gui.Shutdown();
}
