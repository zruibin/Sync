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
#include "sync/sync.h"
#include "platform/runloop.h"


int main() {
    RunLoopInitMain();
    std::cout << "Hello Sync." << std::endl;
    
    const char *destPath = "/Users/ruibin.chow/Desktop/swiftDemo";
    WalkDirectory(destPath);
    
    std::thread([] {
        while (true) {
            std::string str;
            std::getline(std::cin, str);
            std::cout << "输入的字符串是：" << str << std::endl;
            if (str == "exit") {
                RunLoopRef ref = RunLoopGetMain();
                ref->isExit = true;
                break;
            }
        }
    }).detach();
    
    RunLoopRun();
}



