//
// Created by nikita on 03.05.21.
//

#ifndef BLOCKCHAIN_UTILS_H
#define BLOCKCHAIN_UTILS_H
#include <string>
#include <vector>

class Utils {
public:
    static std::string build_merkle_tree(std::vector<std::string> merkle);
};


#endif //BLOCKCHAIN_UTILS_H
