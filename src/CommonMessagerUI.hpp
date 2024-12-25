#ifndef COMMON_MESSAGER_UI_HPP
#define COMMON_MESSAGER_UI_HPP

#include <iostream>
#include <queue>
#include <string>

namespace CommonMessager {

    void run(std::queue<std::string> (*getMessages)(), void (*sendMessage)(const std::string), std::string username);
}

#endif // COMMON_MESSAGER_UI_HPP
