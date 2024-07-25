/*
 * ignore.h
 *
 * Created by Ruibin.Chow on 2024/07/25.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#ifndef IGNORE_H
#define IGNORE_H


#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>


namespace ignore {

std::string ReadFile(const char* filePath);

struct GitignoreHelper {
    std::vector<std::pair<std::regex, std::regex>> positives;
    std::vector<std::pair<std::regex, std::regex>> negatives;
    
    static GitignoreHelper Compile(const std::string& content);
    
    bool Accepts(const std::string& input) const;
    bool Denies(const std::string& input) const;
    bool Maybe(const std::string& input) const;
};

}

#endif /* !IGNORE_H */
