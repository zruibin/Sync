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
    FIELDS_REFLECT(Node, type, name, hash);
};

struct NodeList : Jsonable<NodeList> {
    std::optional<std::vector<Node>> nodes;
    FIELDS_REFLECT(NodeList, nodes);
};

#endif /* !NODE_HPP */
