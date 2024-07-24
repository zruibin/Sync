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
#include "sync/sync.h"


int main() {
    std::cout << "Hello Sync." << std::endl;
    
    const char *destPath = "/Users/ruibin.chow/Desktop/swiftDemo";
    WalkDirectory(destPath);
}



