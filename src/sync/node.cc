/*
 *
 * node.cc
 *
 * Created by Ruibin.Chow on 2024/08/04.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#include "node.h"
#include <iostream>
#include <md5/md5.h>
#include "ignore/ignore.h"

namespace node {

namespace fs = std::filesystem;

std::string GetFileHash(const std::filesystem::path& path) {
    std::ifstream in(path.string().c_str(), std::ios::binary);
    if (!in) {
        return "";
    }
    security::MD5 md5;
    std::streamsize length;
    char buffer[1024];
    while (!in.eof()) {
        in.read(buffer, 1024);
        length = in.gcount();
        if (length > 0) {
            md5.update(buffer, length);
        }
    }
    in.close();
    return md5.toString();
}

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
    
    //#define TESTDIR(path) PROJECT_DIR#path
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
            node.type = NodeType::Directory;
        } else if (fs::exists(path) && fs::is_regular_file(path)) {
            node.type = NodeType::File;
            node.size = fs::file_size(path);
            node.hash = GetFileHash(path);
        }
        nodes.emplace_back(node);
    }
    NodeList nodeInfo;
    nodeInfo.nodes = nodes;
    
    return nodeInfo;
}

std::optional<std::vector<NodeAction>>
GetNodeDiffs(const std::vector<Node>& main,
             const std::vector<Node>& slave) {
    std::vector<NodeAction> actions;
    
    for (auto& mainNode : main) {
        if (mainNode.type != NodeType::File) continue;
        NodeAction action;
        action.type = NodeAction::Unknow;
        
        bool isExist = false;
        for (auto& slaveNode : slave) {
            if (slaveNode.type != NodeType::File) continue;
            
            if (mainNode.name.value().compare(slaveNode.name.value()) == 0 &&
                mainNode.size.value() != slaveNode.size.value()) {
                action.type = NodeAction::Modify;
                action.mainNode = mainNode;
                action.slaveNode = slaveNode;
                isExist = true;
                break;
            } else if (mainNode.size.value() == slaveNode.size.value() &&
                       mainNode.hash.value().compare(slaveNode.hash.value()) == 0 &&
                       mainNode.name.value().compare(slaveNode.name.value()) != 0) {
                action.type = NodeAction::Move;
                action.mainNode = mainNode;
                action.slaveNode = slaveNode;
                isExist = true;
                break;
            } else if (mainNode.name.value().compare(slaveNode.name.value()) == 0 &&
                       mainNode.hash.value().compare(slaveNode.hash.value()) == 0 &&
                       mainNode.size.value() == slaveNode.size.value()) {
                isExist = true;
                break;
            }
        }
        
        if (!isExist) {
            action.type = NodeAction::Add;
            action.mainNode = mainNode;
        }
        if (action.type != NodeAction::Unknow) {
            actions.emplace_back(action);
        }
    }
    
    for (auto& slaveNode : slave) {
        if (slaveNode.type != NodeType::File) continue;
        
        bool isExist = false;
        for (auto& mainNode : main) {
            if (mainNode.type != NodeType::File) continue;
            
            if (slaveNode.hash.value().compare(mainNode.hash.value()) == 0) {
                isExist = true;
                break;
            } else if (mainNode.name.value().compare(slaveNode.name.value()) == 0 &&
                       mainNode.size.value() != slaveNode.size.value()) {
                isExist = true;
                break;
            } else if (mainNode.name.value().compare(slaveNode.name.value()) == 0 &&
                       mainNode.hash.value().compare(slaveNode.hash.value()) == 0 &&
                       mainNode.size.value() == slaveNode.size.value()) {
                isExist = true;
                break;
            }
        }
        if (!isExist) {
            NodeAction action;
            action.type = NodeAction::Delete;
            action.slaveNode = slaveNode;
            actions.emplace_back(action);
        }
    }
    
    return actions.size() == 0 ? std::nullopt : std::make_optional(actions);
}

}
