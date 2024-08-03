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
#include "node.hpp"
#include "ignore/ignore.h"
#include "connection/network/network.h"

#define TESTDIR(path) PROJECT_DIR#path

namespace fs = std::filesystem;

bool is_directory(const std::string& path) {
    return fs::exists(path) && fs::is_directory(path);
}

std::string getName(const fs::path& path, const char* home) {
    std::string filePath = path.string();
    std::string releativePath = path.relative_path();
    
    std::string dir = path.parent_path().string();
    std::string dirName = dir.erase(0, std::strlen(home) + 1);
    std::string name = filePath.erase(0, std::strlen(home) + 1);
    return name;
}

void list_files_and_directories(const char *directory) {
    uint count = 0;
    
    std::string content = ignore::ReadFile(TESTDIR(/test/gitignore));
//    Log(DEBUG) << "content: " << content;
    Log(DEBUG) << "--------------------------------------------------------------"
                << "--------------------------------------------------------------";
    
    ignore::GitIgnore ignored;
    ignored.Compile(content);
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        std::string name = getName(entry.path(), directory);
        
        
        if (fs::is_directory(entry.status())) {
//            Log(DEBUG) << "目录: " << name;
        } else {
//            Log(DEBUG) << "文件: " << name;
            if (ignored.Accepts(name)) {
                Log(DEBUG) << "name: " << name;
                ++count;
            }
        }
    }
    Log(DEBUG) << "count: " << count;
}

void WalkDirectory(const char *directory) {
    if (directory == nullptr || std::strlen(directory) == 0) {
        return ;
    }
    
    list_files_and_directories(directory);
    
    
    Log(DEBUG) << "--------------------------------------------------------------"
                << "--------------------------------------------------------------";
    
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



