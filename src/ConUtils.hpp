#ifndef CONUTILS_HPP
#define CONUTILS_HPP

#include <iostream>

namespace ConUtils {
    struct WindowSize
    {
        int rows = 0;
        int cols = 0;
    };

    void clearConsole();
    void moveCursor(int row, int col);
    WindowSize getConsoleDimensions();
    void setCursorVis(bool visible);
    void setRawMode(bool enable);
    char readKeyPress();
    bool keyPressed();
}

#endif
