/*
 *
 * ignore.cc
 *
 * Created by Ruibin.Chow on 2024/07/25.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#include "ignore.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>

namespace ignore {

std::string ReadFile(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    
    if (file) {
        buffer << file.rdbuf(); // 读取整个文件内容到stringstream
        file.close();           // 关闭文件流
    } else {
        std::cerr << "Unable to open file" << std::endl;
        return "";
    }
    
    // 从stringstream中获取字符串
    return buffer.str();
}


#define DELIMITER "/"
#define ASTERISK "*"
#define DOUBLE_ASTERISK "**"

bool StartWithString(std::string_view originStr, std::string_view subStr) {
    if (originStr.size() == 0 || subStr.size() == 0
        || originStr.size() < subStr.size()) {
        return false;
    }
    return originStr.substr(0, subStr.size()) == subStr;
}

bool EndWithString(std::string_view originStr, std::string_view subStr) {
    if (originStr.size() == 0 || subStr.size() == 0
        || originStr.size() < subStr.size()) {
        return false;
    }
    return originStr.substr(originStr.size() - subStr.size()) == subStr;
}

bool ContainWithString(std::string_view originStr, std::string_view subStr) {
    if (originStr.size() == 0 || subStr.size() == 0
        || originStr.size() < subStr.size()) {
        return false;
    }
    return originStr.find(subStr) != std::string_view::npos;
}

void GitIgnore::Compile(const std::string_view content) {
    std::vector<std::string> lines;
    std::istringstream iss({std::string(content)});
    std::string line;

    while (std::getline(iss, line)) {
        // line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
        if (!line.empty() && line[0] != '#') {
            lines.emplace_back(line);
        }
    }

    for (const auto& line : lines) {
        bool isNegative = false;
        std::string tmpStr = line;
        if (line[0] == '!') {
            if (lines.size() == 1) {
                continue;
            }
            isNegative = true;
            tmpStr = line.substr(1);
        }
        if (EndWithString(tmpStr, DELIMITER)) {
            if (isNegative) {
                negativeDirs_.emplace_back(tmpStr);
            } else {
                positiveDirs_.emplace_back(tmpStr);
            }
        } else {
            if (isNegative) {
                negativeFiles_.emplace_back(tmpStr);
            } else {
                positiveFiles_.emplace_back(tmpStr);
            }
        }
    }
}
    
bool GitIgnore::Accepts(const std::string_view input) const {
    std::string tmp{ DELIMITER + std::string{input} };
    std::cout << "file: " << tmp;
    std::cout << " ";
    for (auto& re : negativeDirs_) {
//        std::cout << "!";
        this->ProcessDir(tmp, re);
    }
//    for (auto& re : negativeFiles_) {
//        std::cout << ", !匹配: " << re;
//    }
    
    for (auto& re : positiveDirs_) {
        this->ProcessDir(tmp, re);
    }
    for (auto& re : positiveFiles_) {
//        std::cout << ", 匹配: " << re;
        this->ProcessFile(tmp, re);
    }

    std::cout  << std::endl;
    return false;
}
    
bool GitIgnore::Denies(const std::string_view input) const {
    return !this->Accepts(input);
}

bool GitIgnore::ProcessDir(const std::string_view input,
                           const std::string_view re) const {
    bool match = false;
    if (StartWithString(re, DELIMITER) && EndWithString(re, DELIMITER)
        && !ContainWithString(re, ASTERISK)) {
        // /file/
        if (StartWithString(input, re)) {
            match = true;
            std::cout << "匹配: " << re << ", ";
        }
    } else if (!StartWithString(re, DELIMITER) && EndWithString(re, DELIMITER)
               && !ContainWithString(re, ASTERISK)) {
        // file/
        if (ContainWithString(input, re)) {
            match = true;
            std::cout << "匹配: " << re << ", ";
        }
    } else if (StartWithString(re, DELIMITER) && EndWithString(re, DELIMITER)
               && ContainWithString(re, ASTERISK)) {
        // /.git/**/refs/
    } else if (!StartWithString(re, DELIMITER) && EndWithString(re, DELIMITER)
               && ContainWithString(re, ASTERISK)) {
        // .git/**/refs/
    }
    return match;
}

bool GitIgnore::ProcessFile(const std::string_view input,
                            const std::string_view re) const {
    bool match = false;
    if (!ContainWithString(re, DELIMITER) && !ContainWithString(re, ASTERISK)) {
        // .DS_Store
        if (EndWithString(input, re)) {
            match = true;
            std::cout << "匹配: " << re << ", ";
        }
    } else if (StartWithString(re, DELIMITER) && !ContainWithString(re, ASTERISK)) {
        // /file/start.sh
        if (EndWithString(input, re)) {
            match = true;
            std::cout << "匹配: " << re << ", ";
        }
    } else if (!StartWithString(re, DELIMITER) && !ContainWithString(re, ASTERISK)) {
        // file/backup.py
        if (EndWithString(input, re)) {
            match = true;
            std::cout << "匹配: " << re << ", ";
        }
    } else if (!ContainWithString(re, DELIMITER) && ContainWithString(re, ASTERISK)) {
        // *.swift
        
    } else if (StartWithString(re, DELIMITER) && ContainWithString(re, ASTERISK)
               && !ContainWithString(re, DOUBLE_ASTERISK)) {
        // /logs/*.log,
    } else if (StartWithString(re, DELIMITER) && ContainWithString(re, ASTERISK)
               && ContainWithString(re, DOUBLE_ASTERISK)) {
        // /file/**/*.swift
    } else if (!StartWithString(re, DELIMITER) && ContainWithString(re, ASTERISK)
               && !ContainWithString(re, DOUBLE_ASTERISK)) {
        // src/*.swift
    } else if (!StartWithString(re, DELIMITER) && ContainWithString(re, DOUBLE_ASTERISK)
               && ContainWithString(re, DOUBLE_ASTERISK)) {
        // file/**/*.swift
    }
    
    return match;
}


}
