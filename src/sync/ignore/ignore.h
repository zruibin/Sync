/*
 * ignore.h
 *
 * Created by Ruibin.Chow on 2024/07/25.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#ifndef IGNORE_H
#define IGNORE_H

#include <string_view>
#include <string>
#include <vector>

namespace ignore {

std::string ReadFile(const char* filePath);

//struct GitignoreHelper {
//    std::vector<std::pair<std::regex, std::regex>> positives;
//    std::vector<std::pair<std::regex, std::regex>> negatives;
//    
//    static GitignoreHelper Compile(const std::string& content);
//    
//    bool Accepts(const std::string& input) const;
//    bool Denies(const std::string& input) const;
//    bool Maybe(const std::string& input) const;
//};

struct GitIgnore {
    void Compile(const std::string& content);
    bool Accepts(const std::string& input) const;
    bool Denies(const std::string& input) const;
    
private:
    bool ProcessDir(const std::string& input,
                    const std::string& regex) const;
    bool ProcessFile(const std::string& input,
                     const std::string& regex) const;
    
private:
    std::vector<std::string> positiveFiles_;
    std::vector<std::string> negativeFiles_;
    std::vector<std::string> positiveDirs_;
    std::vector<std::string> negativeDirs_;
};

}

#endif /* !IGNORE_H */
