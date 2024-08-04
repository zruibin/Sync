/*
 *
 * sync.cc
 *
 * Created by Ruibin.Chow on 2024/07/05.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#include "sync.h"
#include <iostream>
#include <filesystem>
#include "log/logging.h"
#include "node.h"
#include "connection/network/network.h"

void WalkDirectory(const char *directory) {
    if (directory == nullptr || std::strlen(directory) == 0) {
        return ;
    }
    
//    RecursiveDirectory(directory);
    
    Log(DEBUG) << "--------------------------------------------------------------"
                << "--------------------------------------------------------------";
    
    /*
    auto main = node::RecursiveDirectory("/Users/ruibin.chow/Desktop/main");
    auto slave = node::RecursiveDirectory("/Users/ruibin.chow/Desktop/slave");
    auto diffs = node::GetNodeDiffs(main.value().nodes.value(), slave.value().nodes.value());
    if (diffs.has_value()) {
        for (const auto& diff : diffs.value()) {
            Log(DEBUG) << "type: " << diff.type
                            << ", main: " << diff.mainNode.name.value_or("")
                            << "(" << diff.mainNode.hash.value_or("") << ")"
                            << ", slave: " << diff.slaveNode.name.value_or("")
                            << "(" << diff.slaveNode.hash.value_or("") << ")";
        }
    }
    // */
    
    
//    connection::PrintNetworkInfo();
//    
//    fs::path dir_path("/Users/ruibin.chow/Desktop/test/tmp/aa");
//    // 创建新目录
//    if (fs::create_directories(dir_path)) {
//        Log(DEBUG) << "Directory created successfully.";
//    } else {
//        std::cout << "Directory already exists or cannot be created.";
//    }
}



