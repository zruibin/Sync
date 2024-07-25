/*
 *
 * ignore.cc
 *
 * Created by Ruibin.Chow on 2024/07/25.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#include "ignore.hpp"

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


std::string EscapeRegex(const std::string& pattern) {
    std::string escaped;
    for (char c : pattern) {
        if (std::string("-[]/{}()+?.\\^$|").find(c) != std::string::npos) {
            escaped += '\\';
        }
        escaped += c;
    }
    return escaped;
}

std::string PrepareRegexPattern(const std::string& pattern) {
    std::string escaped = EscapeRegex(pattern);
    std::string regexPattern = std::regex_replace(escaped, std::regex("\\*\\*"), "(.+)");
    regexPattern = std::regex_replace(regexPattern, std::regex("\\*"), "([^\\/]+)");
    return regexPattern;
}

std::string PreparePartialRegex(const std::string& pattern) {
    std::stringstream ss;
    std::istringstream iss(pattern);
    std::string item;
    bool first = true;

    while (std::getline(iss, item, '/')) {
        if (!first) {
            ss << "([\\/]?(" << PrepareRegexPattern(item) << "\\b|$))";
        } else {
            ss << "(" << PrepareRegexPattern(item) << "\\b)";
            first = false;
        }
    }

    return ss.str();
}

std::pair<std::regex, std::regex> PrepareRegexes(const std::string& pattern) {
    return {
        std::regex("^(" + PrepareRegexPattern(pattern) + ")"),
        std::regex("^(" + PreparePartialRegex(pattern) + ")")
    };
}

std::vector<std::vector<std::pair<std::regex, std::regex>>> Parse(const std::string& content) {
    std::vector<std::string> lines;
    std::istringstream iss(content);
    std::string line;

    while (std::getline(iss, line)) {
        line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
        if (!line.empty() && line[0] != '#') {
            lines.push_back(line);
        }
    }

    std::vector<std::string> positives, negatives;
    for (const auto& line : lines) {
        if (line[0] == '!') {
            negatives.push_back(line.substr(1));
        } else {
            positives.push_back(line);
        }
    }

    auto prepareList = [](const std::vector<std::string>& list) {
        std::vector<std::pair<std::regex, std::regex>> regexes;
        for (const auto& pattern : list) {
            regexes.push_back(PrepareRegexes(pattern));
        }
        return regexes;
    };

    return { prepareList(positives), prepareList(negatives) };
}

GitignoreHelper GitignoreHelper::Compile(const std::string& content) {
    auto parsed = Parse(content);
    return { parsed[0], parsed[1] };
}

bool GitignoreHelper::Accepts(const std::string& input) const {
    std::string path = input[0] == '/' ? input.substr(1) : input;
    return std::any_of(negatives.begin(), negatives.end(), [&path](const auto& regexPair) {
        return std::regex_match(path, regexPair.first);
    }) || !std::any_of(positives.begin(), positives.end(), [&path](const auto& regexPair) {
        return std::regex_match(path, regexPair.first);
    });
}

bool GitignoreHelper::Denies(const std::string& input) const {
    std::string path = input[0] == '/' ? input.substr(1) : input;
    return !Accepts(path);
}

bool GitignoreHelper::Maybe(const std::string& input) const {
    std::string path = input[0] == '/' ? input.substr(1) : input;
    return std::any_of(negatives.begin(), negatives.end(), [&path](const auto& regexPair) {
        return std::regex_match(path, regexPair.second);
    }) || !std::any_of(positives.begin(), positives.end(), [&path](const auto& regexPair) {
        return std::regex_match(path, regexPair.second);
    });
}


}


//
// .gitignore 文件用于告诉 Git 哪些文件或目录应该被忽略，不纳入版本控制。以下是 .gitignore 文件中可以使用的全部规则和语法：
//
// 1. 空行：空行会被忽略，可以用来增加文件的可读性。
//
// 2. 注释：以 `#` 开头的行是注释，会被忽略。
//
// ```
// # 这是一个注释
// ```
//
// 3. 通配符：
//   * `*` 匹配零个或多个字符。
//   * `?` 匹配单个字符。
//   * `[abc]` 匹配方括号内的任意一个字符。
//
// ```
// *.log       # 忽略所有 .log 文件
// build/      # 忽略 build 目录及其所有内容
// temp?       # 忽略 temp1, temp2, tempA 等文件
// ```
//
// 4. 目录：以斜杠 `/` 结尾表示目录。
//
// ```
// logs/       # 忽略 logs 目录及其所有内容
// ```
//
// 5. 否定规则：以` ! `开头的模式表示例外规则，即不忽略匹配的文件或目录。
//
// ```
// *.log       # 忽略所有 .log 文件
// !important.log  # 但不忽略 important.log
// ```
//
// 6. 斜杠` /`：
//
//   * 开头的 / 表示相对于仓库根目录。
//   * 结尾的 / 表示目录。
// ```
// /config/    # 仅忽略仓库根目录下的 config 目录
// config/     # 忽略所有名为 config 的目录
// ```
//
// 7. 双星号 `**`：
//
//  * `**` 匹配任意数量的目录。
//
// ```
// **/logs     # 忽略所有 logs 目录
// logs/**     # 忽略 logs 目录及其所有子目录和文件
// ```
//
// 8. 转义字符：使用反斜杠 \ 转义特殊字符。
//
// ```
// \#file      # 忽略名为 #file 的文件
// ```
//
// 9. 组合规则：可以组合使用上述规则来实现复杂的忽略模式。
//
// ```
// # 忽略所有 .log 文件，但不忽略 logs 目录中的 .log 文件
// *.log
// !logs/*.log
// ```
//
// 以下是一个示例 `.gitignore` 文件：
//
// ```
// # 忽略所有 .log 文件
// *.log
//
// # 忽略所有 .tmp 文件
// *.tmp
//
// # 忽略 build 目录及其所有内容
// build/
//
// # 忽略所有名为 temp 的目录
// temp/
//
// # 忽略所有 logs 目录，但不忽略 logs 目录中的 .log 文件
// logs/
// !logs/*.log
//
// # 忽略所有 .DS_Store 文件（常见于 macOS）
// .DS_Store
//
// # 忽略所有 node_modules 目录
// node_modules/
// ```
//
// 通过合理使用 .gitignore 文件，可以有效地管理哪些文件和目录应该被版本控制系统忽略，从而保持代码仓库的整洁和高效。
