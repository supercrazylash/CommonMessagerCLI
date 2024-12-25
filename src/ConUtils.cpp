#include "ConUtils.hpp"
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <term.h>
#include <termios.h>
#include <fcntl.h>
#endif

namespace ConUtils {
    static struct termios oldt, newt;

    void clearConsole() {
    #ifdef _WIN32
        system("cls");
    #else
        if (!cur_term) {
            int result;
            setupterm(nullptr, STDOUT_FILENO, &result);
            if (result <= 0) return;
        }
        putp(tigetstr((char*)"clear"));
    #endif
    }

    void moveCursor(int row, int col) {
    #ifdef _WIN32
        COORD coord = {static_cast<SHORT>(col), static_cast<SHORT>(row)};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    #else
        std::cout << "\033[" << row + 1 << ";" << col + 1 << "H";
    #endif
    }

    WindowSize getConsoleDimensions() {
        int rows, cols;

    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        } else {
            cols = 80; // Default width
            rows = 25; // Default height
        }
    #else
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
            cols = w.ws_col;
            rows = w.ws_row;
        } else {
            cols = 80; // Default width
            rows = 25; // Default height
        }
    #endif

        return {rows, cols};
    }

    void setCursorVis(bool visible) {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = visible;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    #else
        if (!cur_term) {
            int result;
            setupterm(nullptr, STDOUT_FILENO, &result);
            if (result <= 0) return;
        }
        if (visible) {
            putp(tigetstr((char*)"cnorm")); // Show cursor
        } else {
            putp(tigetstr((char*)"civis")); // Hide cursor
        }
    #endif
    }

    void setRawMode(bool enable) {
    #ifndef _WIN32
        if (enable) {
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        } else {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        }
    #endif
    }

    bool keyPressed() {
    #ifdef _WIN32
        return _kbhit();
    #else
        int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        char ch = getchar();

        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if (ch != EOF) {
            ungetc(ch, stdin);
            return true;
        }

        return false;
    #endif
    }

    char readKeyPress() {
    #ifdef _WIN32
        return _getch();
    #else
        return getchar();
    #endif
    }
}
