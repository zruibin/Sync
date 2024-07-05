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

bool is_directory(const std::string& path) {
    namespace fs = std::filesystem;
    return fs::exists(path) && fs::is_directory(path);
}

int main() {
    std::cout << "Hello Sync." << std::endl;
    
    std::string destPath = "/Users/ruibin.chow/Documents/code/check";
    // 遍历目录下的文件
    for (const auto &entry : std::filesystem::recursive_directory_iterator(destPath)) {
        const std::filesystem::path path = entry.path();
        std::string filePath = path.string();
        std::string fileName = path.filename().string();
        std::string releativePath = path.relative_path();
        
        if (is_directory(path)) {
            continue;
        }
        
        std::string dir = path.parent_path().string();
        
        std::cout << dir //.erase(0, destPath.length() + 1)
                    << " -> " << fileName << std::endl;
        
//        std::cout << filePath << " -> " << fileName << " -> " << releativePath << std::endl;
        
    }
}



