  /*
 *
 * main.cc
 *
 * Created by Ruibin.Chow on 2024/07/05.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#include <iostream>
#include <filesystem>
#include <string>
#include <thread>
#include <chrono>
#include "sync/sync.h"


int main() {
    std::cout << "Hello Sync." << std::endl;
    
    const char *destPath = "/Users/ruibin.chow/Desktop/swiftDemo";
    WalkDirectory(destPath);
    
    bool isExit = false;
    std::thread([&isExit] {
        while (true) {
            std::string str;
            std::getline(std::cin, str);
            std::cout << "输入的字符串是：" << str << std::endl;
            if (str == "exit") {
                isExit = true;
                break;
            }
        }
    }).detach();
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (isExit) {
            std::cout << "Exit." << std::endl;
            break;
        }
    }
}



