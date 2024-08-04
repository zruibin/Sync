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

