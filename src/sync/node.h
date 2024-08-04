/*
 * node.hpp
 *
 * Created by Ruibin.Chow on 2024/07/24.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>
#include <optional>
#include <filesystem>
#include <nlohmann_json_reflect/jsonable.hpp>

namespace node {

enum class NodeType {
    Unknow,
    Directory,
    File
};

struct Node {
    std::optional<NodeType> type;
    std::optional<std::string> name;
    std::optional<std::string> hash;
    std::optional<size_t> size; // 字节(Byte)
    FIELDS_REFLECT(Node, type, name, hash, size);
};

struct NodeList : Jsonable<NodeList> {
    std::optional<std::vector<Node>> nodes;
    FIELDS_REFLECT(NodeList, nodes);
};

struct NodeAction {
    enum Type {
        Unknow,
        Add,
        Delete,
        Move,
        Modify,
    };
    Type type;
    Node mainNode;
    Node slaveNode;
};

std::string GetFileHash(const char* path);
std::string GetFileSmartHash(const char* path);

bool IsDirectory(const std::string& path);

std::string GetNameByPath(const std::filesystem::path& path, const char* home);

std::optional<NodeList> RecursiveDirectory(const char *directory);

std::optional<std::vector<NodeAction>>
GetNodeDiffs(const std::vector<Node>& main,
             const std::vector<Node>& slave);

}

#endif /* !NODE_H */
