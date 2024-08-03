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

/**
 https://blog.csdn.net/yimeng_Sama/article/details/140797720
 未实现：?通配符、[abc]模式、[abc]模式、{string1,string2,...}模、\字符
 */

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
#define DELIMITER_ASTERISK "**/*"

#define MATCH_COUT //std::cout << "匹配: " << re << ", ";

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

std::vector<std::string> StringSplitByDelimiter(std::string_view str,
                                                std::string_view delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
 
    while (end != std::string::npos) {
        tokens.emplace_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
 
    tokens.emplace_back(str.substr(start)); // 添加最后一个子字符串
    return tokens;
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
    std::string tmp{ DELIMITER + std::string{ input} };
    bool accept = false;
    for (auto& re : negativeDirs_) {
        if (this->ProcessDir(tmp, re)) {
            accept = true;
            break;
        }
    }
    if (accept) return true;
    
    for (auto& re : negativeFiles_) {
        if (this->ProcessFile(tmp, re)) {
            accept = true;
            break;
        }
    }
    if (accept) return true;
    
    bool match = false;
    for (auto& re : positiveDirs_) {
        if (this->ProcessDir(tmp, re)) {
            match = true;
            break;
        }
    }
    if (match) return false;
    
    for (auto& re : positiveFiles_) {
        if (this->ProcessFile(tmp, re)) {
            match = true;
            break;
        }
    }
    if (match) return false;
    
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
        }
    } else if (!StartWithString(re, DELIMITER) && EndWithString(re, DELIMITER)
               && !ContainWithString(re, ASTERISK)) {
        // file/
        if (ContainWithString(input, re)) {
            match = true;
        }
    } else if (StartWithString(re, DELIMITER) && EndWithString(re, DELIMITER)
               && ContainWithString(re, DOUBLE_ASTERISK)) {
        // /.git/**/refs/
        auto subs = StringSplitByDelimiter(re, DOUBLE_ASTERISK);
        if (StartWithString(input, subs[0]) && ContainWithString(input, subs[1])) {
            match = true;
        }
        if (StartWithString(input, subs[0]) && EndWithString(input, subs[1])) {
            match = true;
        }
    } else if (!StartWithString(re, DELIMITER) && EndWithString(re, DELIMITER)
               && ContainWithString(re, DOUBLE_ASTERISK)) {
        // .git/**/refs/
        auto subs = StringSplitByDelimiter(re, DOUBLE_ASTERISK);
        if (ContainWithString(input, subs[0])) {
            auto subs2 = StringSplitByDelimiter(input, subs[0]);
            if (subs2.size() > 1 && ContainWithString(subs2[1], subs[1])) {
                match = true;
            }
        }
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
        }
    } else if (StartWithString(re, DELIMITER) && !ContainWithString(re, ASTERISK)) {
        // /file/start.sh
        if (EndWithString(input, re)) {
            match = true;
        }
    } else if (!StartWithString(re, DELIMITER) && !ContainWithString(re, ASTERISK)) {
        // file/backup.py
        if (EndWithString(input, re)) {
            match = true;
        }
    } else if (!ContainWithString(re, DELIMITER) && ContainWithString(re, ASTERISK)) {
        // *.swift
        auto str = std::string{re};
        auto subStr = str.replace(re.find(ASTERISK), 1, "");
        if (EndWithString(input, subStr)) {
            match = true;
        }
    } else if (StartWithString(re, DELIMITER) && ContainWithString(re, ASTERISK)
               && !ContainWithString(re, DOUBLE_ASTERISK)) {
        // /logs/*.log,
        auto subs = StringSplitByDelimiter(re, ASTERISK);
        if (StartWithString(input, subs[0]) && EndWithString(input, subs[1])) {
            match = true;
        }
    } else if (!StartWithString(re, DELIMITER) && ContainWithString(re, ASTERISK)
               && !ContainWithString(re, DOUBLE_ASTERISK)) {
        // src/*.swift
        auto subs = StringSplitByDelimiter(re, ASTERISK);
        if (StartWithString(input, subs[0]) && EndWithString(input, subs[1])) {
            match = true;
        }
    } else if (StartWithString(re, DELIMITER) && ContainWithString(re, ASTERISK)
               && ContainWithString(re, DOUBLE_ASTERISK)) {
        // /file/**/*.swift
        auto subs = StringSplitByDelimiter(re, DELIMITER_ASTERISK);
        if (subs.size() > 1 && StartWithString(input, subs[0]) && EndWithString(input, subs[1])) {
            match = true;
        }
    } else if (!StartWithString(re, DELIMITER) && ContainWithString(re, DOUBLE_ASTERISK)
               && ContainWithString(re, DOUBLE_ASTERISK)) {
        // file/**/*.swift
        auto subs = StringSplitByDelimiter(re, DELIMITER_ASTERISK);
        if (subs.size() > 1 && ContainWithString(input, subs[0]) && EndWithString(input, subs[1])) {
            match = true;
        }
    }
    
    return match;
}


}
