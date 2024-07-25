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
#include "node.hpp"
#include "ignore/ignore.hpp"

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
    std::cout << "content: " << content << std::endl;
    std::cout << "--------------------------------------------------------------"
                << "--------------------------------------------------------------"
                << std::endl;
    
    ignore::GitignoreHelper helper = ignore::GitignoreHelper::Compile(content);
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        std::string name = getName(entry.path(), directory);
        
        if (!helper.Accepts(name)) {
            continue;
        }
        
        ++count;
        if (fs::is_directory(entry.status())) {
            std::cout << "目录: " << name << std::endl;
        } else {
            std::cout << "文件: " << name << std::endl;
        }
    }
    std::cout << "count: " << count << std::endl;
}

void WalkDirectory(const char *directory) {
    if (directory == nullptr || std::strlen(directory) == 0) {
        return ;
    }
    
    list_files_and_directories(directory);
    
    
    std::cout << "--------------------------------------------------------------"
                << "--------------------------------------------------------------"
                << std::endl;
    
    fs::path dir_path("/Users/ruibin.chow/Desktop/test/tmp/aa");
    // 创建新目录
    if (fs::create_directories(dir_path)) {
        std::cout << "Directory created successfully." << std::endl;
    } else {
        std::cout << "Directory already exists or cannot be created." << std::endl;
    }
}



