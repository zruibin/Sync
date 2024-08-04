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
#include "ignore/ignore.h"
#include "connection/network/network.h"

#define TESTDIR(path) PROJECT_DIR#path

namespace fs = std::filesystem;

bool IsDirectory(const std::string& path) {
    return fs::exists(path) && fs::is_directory(path);
}

std::string GetNameByPath(const fs::path& path, const char* home) {
    std::string filePath = path.string();
    std::string releativePath = path.relative_path();
    
    std::string dir = path.parent_path().string();
    std::string dirName = dir.erase(0, std::strlen(home) + 1);
    std::string name = filePath.erase(0, std::strlen(home) + 1);
    return name;
}

std::optional<NodeList> RecursiveDirectory(const char *directory) {
    if (!fs::exists(directory)) {
        return std::nullopt;
    }
//    std::string content = ignore::ReadFile(TESTDIR(/test/gitignore));
//    Log(DEBUG) << "content: " << content;
//    ignore::GitIgnore ignored;
//    ignored.Compile(content);
    
    std::vector<Node> nodes;
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        const auto& path = entry.path();
        std::string name = GetNameByPath(entry.path(), directory);
        
        Node node;
        node.name = name;
        if (fs::is_directory(entry.status())) {
//            Log(DEBUG) << "目录: " << name;
            node.type = NodeType::Directory;
        } else if (fs::exists(path) && fs::is_regular_file(path)) {
//            Log(DEBUG) << "文件: " << name;
            node.type = NodeType::File;
            node.size = fs::file_size(path);
            node.hash = GetFileHash(path);
//            if (ignored.Accepts(name)) {
//                Log(DEBUG) << "name: " << name;
//            }
        }
        nodes.emplace_back(node);
    }
    NodeList nodeInfo;
    nodeInfo.nodes = nodes;

    Log(DEBUG) << "Json: " << nodeInfo.ToJsonString();
    return nodeInfo;
}

void WalkDirectory(const char *directory) {
    if (directory == nullptr || std::strlen(directory) == 0) {
        return ;
    }
    
    RecursiveDirectory(directory);
    
    
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



