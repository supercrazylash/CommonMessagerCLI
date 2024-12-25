#include <csignal>
#include "CommonMessagerUI.hpp"
#include "ConUtils.hpp"

class CursorVisibilityGuard {
public:
    CursorVisibilityGuard() {
        ConUtils::setCursorVis(false);
        ConUtils::setRawMode(true);
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
    }

    ~CursorVisibilityGuard() {
        ConUtils::setCursorVis(true);
        ConUtils::setRawMode(false);
    }

    static void signalHandler(int signal) {
        ConUtils::setCursorVis(true);
        ConUtils::setRawMode(false);
        std::exit(signal);
    }
};

namespace CommonMessager {
    using namespace ConUtils;

    void run(std::queue<std::string> (*getMessages)(), void (*sendMessage)(const std::string), std::string username) {
        CursorVisibilityGuard cursorGuard; // Ensures cursor visibility is reset on exit

        WindowSize windowSize = getConsoleDimensions();
        clearConsole();

        std::queue<std::string> msgsRcvd;

        std::string message = "";
        int i = 0;

        while (true) {
            int send = false;

            if (!(windowSize.rows == getConsoleDimensions().rows && windowSize.cols == getConsoleDimensions().cols)) {
                windowSize = getConsoleDimensions();
                clearConsole();
            }

            moveCursor(windowSize.rows - 1, 0);
            std::cout << "[" << username << "] " << message << "█ ";
            std::cout.flush();

            if (keyPressed()) {
                char c = readKeyPress();
                switch (c)
                {
                    case 8:
                    case 127:
                        if (!message.empty())
                            message.pop_back();
                        break;
                    case '\033':
                        break;
                    case '\n':
                    case '\r':
                        send = true;
                        break;
                    default:
                        message.push_back(c);
                        break;
                }

            }

            if (send) {
                std::string msgToSend = "[" + username + "] " + message;
                sendMessage(msgToSend);
                message = "";
                send = false;
            }

            msgsRcvd = getMessages();
            std::cout.flush();

            while (msgsRcvd.size() > 0) {
                moveCursor(windowSize.rows - 1, 0);
                std::string msg = msgsRcvd.front();
                int spacesForClear = windowSize.cols - msg.size() - 1;
                std::cout << msgsRcvd.front() << std::string(spacesForClear, ' ') << std::endl;
                msgsRcvd.pop();
            }

        }
    }
}
