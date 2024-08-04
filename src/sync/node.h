/*
 * node.hpp
 *
 * Created by Ruibin.Chow on 2024/07/24.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <string>
#include <optional>
#include <filesystem>
#include "jsonable.hpp"

enum class NodeType {
    Unknow,
    Directory,
    File
};

struct Node {
    std::optional<NodeType> type;
    std::optional<std::string> name;
    std::optional<std::string> hash;
    std::optional<u_int32_t> size; // 字节(Byte)
    FIELDS_REFLECT(Node, type, name, hash, size);
};

struct NodeList : Jsonable<NodeList> {
    std::optional<std::vector<Node>> nodes;
    FIELDS_REFLECT(NodeList, nodes);
};

std::string GetFileHash(const std::filesystem::path& path);

#endif /* !NODE_HPP */
