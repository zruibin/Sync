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
#include <regex>
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


//std::string EscapeRegex(const std::string& pattern) {
//    std::string escaped;
//    for (char c : pattern) {
//        if (std::string("-[]/{}()+?.\\^$|").find(c) != std::string::npos) {
//            escaped += '\\';
//        }
//        escaped += c;
//    }
//    return escaped;
//}
//
//std::string PrepareRegexPattern(const std::string& pattern) {
//    std::string escaped = EscapeRegex(pattern);
//    std::string regexPattern = std::regex_replace(escaped, std::regex("\\*\\*"), "(.+)");
//    regexPattern = std::regex_replace(regexPattern, std::regex("\\*"), "([^\\/]+)");
//    return regexPattern;
//}
//
//std::string PreparePartialRegex(const std::string& pattern) {
//    std::stringstream ss;
//    std::istringstream iss(pattern);
//    std::string item;
//    bool first = true;
//
//    while (std::getline(iss, item, '/')) {
//        if (!first) {
//            ss << "([\\/]?(" << PrepareRegexPattern(item) << "\\b|$))";
//        } else {
//            ss << "(" << PrepareRegexPattern(item) << "\\b)";
//            first = false;
//        }
//    }
//
//    return ss.str();
//}
//
//std::pair<std::regex, std::regex> PrepareRegexes(const std::string& pattern) {
//    return {
//        std::regex("^(" + PrepareRegexPattern(pattern) + ")"),
//        std::regex("^(" + PreparePartialRegex(pattern) + ")")
//    };
//}
//
//std::vector<std::vector<std::pair<std::regex, std::regex>>> Parse(const std::string& content) {
//    std::vector<std::string> lines;
//    std::istringstream iss(content);
//    std::string line;
//
//    while (std::getline(iss, line)) {
//        line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
//        if (!line.empty() && line[0] != '#') {
//            lines.push_back(line);
//        }
//    }
//
//    std::vector<std::string> positives, negatives;
//    for (const auto& line : lines) {
//        if (line[0] == '!') {
//            negatives.push_back(line.substr(1));
//        } else {
//            positives.push_back(line);
//        }
//    }
//
//    auto prepareList = [](const std::vector<std::string>& list) {
//        std::vector<std::pair<std::regex, std::regex>> regexes;
//        for (const auto& pattern : list) {
//            regexes.push_back(PrepareRegexes(pattern));
//        }
//        return regexes;
//    };
//
//    return { prepareList(positives), prepareList(negatives) };
//}
//
//GitignoreHelper GitignoreHelper::Compile(const std::string& content) {
//    auto parsed = Parse(content);
//    return { parsed[0], parsed[1] };
//}
//
//bool GitignoreHelper::Accepts(const std::string& input) const {
//    std::string path = input[0] == '/' ? input.substr(1) : input;
//    return std::any_of(negatives.begin(), negatives.end(), [&path](const auto& regexPair) {
//        return std::regex_match(path, regexPair.first);
//    }) || !std::any_of(positives.begin(), positives.end(), [&path](const auto& regexPair) {
//        return std::regex_match(path, regexPair.first);
//    });
//}
//
//bool GitignoreHelper::Denies(const std::string& input) const {
//    std::string path = input[0] == '/' ? input.substr(1) : input;
//    return !Accepts(path);
//}
//
//bool GitignoreHelper::Maybe(const std::string& input) const {
//    std::string path = input[0] == '/' ? input.substr(1) : input;
//    return std::any_of(negatives.begin(), negatives.end(), [&path](const auto& regexPair) {
//        return std::regex_match(path, regexPair.second);
//    }) || !std::any_of(positives.begin(), positives.end(), [&path](const auto& regexPair) {
//        return std::regex_match(path, regexPair.second);
//    });
//}


void GitIgnore::Compile(const std::string& content) {
    std::vector<std::string> lines;
    std::istringstream iss(content);
    std::string line;

    while (std::getline(iss, line)) {
        line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
        if (!line.empty() && line[0] != '#') {
            lines.push_back(line);
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
        if (tmpStr.compare(tmpStr.size()-1, 1, "/") == 0) {
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
    
bool GitIgnore::Accepts(const std::string& input) const {
    std::string tmp{ "/" + input };
    std::cout << "file: " << tmp;
    std::cout << " ";
    for (auto& re : negativeDirs_) {
//        std::cout << "!";
//        this->ProcessDir(input, re);
    }
//    for (auto& re : negativeFiles_) {
//        std::cout << ", !匹配: " << re;
//    }
    
    for (auto& re : positiveDirs_) {
//        this->ProcessDir(input, re);
    }
    for (auto& re : positiveFiles_) {
//        std::cout << ", 匹配: " << re;
        this->ProcessFile(input, re);
    }

    std::cout  << std::endl;
    return false;
}
    
bool GitIgnore::Denies(const std::string& input) const {
    return !this->Accepts(input);
}

bool GitIgnore::ProcessDir(const std::string& input,
                           const std::string& re) const {
    bool match = false;
    if (re.compare(0, 1, "/") == 0 && re.compare(re.size()-1, 1, "/") == 0
        && re.find("*") == std::string::npos) {
        // /file/
        std::cout << "匹配: " << re << ", ";
    } else if (re.compare(0, 1, "/") == 0 && re.compare(re.size()-1, 1, "/") == 0
               && re.find("*") != std::string::npos) {
        // /.git/**/refs/
        std::cout << "匹配: " << re << ", ";
    } else if (re.compare(0, 1, "/") != 0 && re.compare(re.size()-1, 1, "/") == 0
               && re.find("*") == std::string::npos) {
        // file/
        std::cout << "匹配: " << re << ", ";
    } else if (re.compare(0, 1, "/") != 0 && re.compare(re.size()-1, 1, "/") == 0
               && re.find("*") != std::string::npos) {
        // .git/**/refs/
        std::cout << "匹配: " << re;
    }
    return match;
}

bool GitIgnore::ProcessFile(const std::string& input,
                            const std::string& re) const {
    bool match = false;
    if (re.find("/") == std::string::npos && re.find("*") == std::string::npos) {
        // .DS_Store
    } else if (re.compare(0, 1, "/") == 0 && re.find("*") == std::string::npos) {
        // /file/start.sh
    } else if (re.compare(0, 1, "/") != 0 && re.find("*") == std::string::npos) {
        // file/backup.py
    } else if (re.find("/") == std::string::npos && re.find("*") != std::string::npos) {
        // *.swift
    } else if (re.compare(0, 1, "/") == 0 && re.find("*") != std::string::npos
               && re.find("**") == std::string::npos) {
        // /logs/*.log,
    } else if (re.compare(0, 1, "/") == 0 && re.find("*") != std::string::npos
               && re.find("**") != std::string::npos) {
        // /file/**/*.swift
    } else if (re.compare(0, 1, "/") != 0 && re.find("*") != std::string::npos
               && re.find("**") == std::string::npos) {
        // src/*.swift
    } else if (re.compare(0, 1, "/") != 0 && re.find("*") != std::string::npos
               && re.find("**") != std::string::npos) {
        // file/**/*.swift
    }
    
    return match;
}


}
