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


struct GitIgnore {
    void Compile(const std::string_view content);
    bool Accepts(const std::string_view input) const;
    bool Denies(const std::string_view input) const;
    
private:
    bool ProcessDir(const std::string_view input,
                    const std::string_view regex) const;
    bool ProcessFile(const std::string_view input,
                     const std::string_view regex) const;
    
private:
    std::vector<std::string> positiveFiles_;
    std::vector<std::string> negativeFiles_;
    std::vector<std::string> positiveDirs_;
    std::vector<std::string> negativeDirs_;
};

}

#endif /* !IGNORE_H */
