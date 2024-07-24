/*
 * sync.h
 *
 * Created by Ruibin.Chow on 2024/07/05.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#ifndef SYNC_H
#define SYNC_H

#include <vector>
#include <string>
#include <optional>

enum class NodeType {
    Unknow,
    Directory,
    File
};

struct Node {
    NodeType type;
    std::string name;
    std::string hash;
};

void WalkDirectory(const char *dir);


#endif /* !SYNC_H */
