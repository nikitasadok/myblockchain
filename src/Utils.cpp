//
// Created by nikita on 03.05.21.
//

#include <crypto++/sha.h>
#include "Utils.h"

std::string Utils::build_merkle_tree(std::vector<std::string> merkle) {
    if (merkle.empty())
        return "";

    while (merkle.size() > 1)
    {
        if (merkle.size() % 2 != 0)
            merkle.push_back(merkle.back());

        std::vector<std::string> new_merkle;

        for (auto it = merkle.begin(); it != merkle.end(); it += 2)
        {
            auto concat_hashes = *it + *(it + 1);

            CryptoPP::SHA256 hash;
            std::string new_node;
            hash.Update((const byte*)concat_hashes.data(), concat_hashes.size());
            new_node.resize(hash.DigestSize());
            hash.Final((byte*)&new_node[0]);

            new_merkle.push_back(new_node);
        }
        merkle = new_merkle;
    }

    return merkle[0];
}
