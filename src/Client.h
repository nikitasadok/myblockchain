//
// Created by nikita on 02.05.21.
//

#ifndef BLOCKCHAIN_CLIENT_H
#define BLOCKCHAIN_CLIENT_H
#include <string>
#include <crypto++/ecp.h>

class Client {
    std::string private_key;
    std::string public_key;
    std::string address;

private:
    std::string public_key_to_string(const CryptoPP::ECPPoint& pub_key);
public:
    std::string generate_private_key();
    CryptoPP::ECPPoint calculate_public_key(std::string private_key);
    std::string generate_wallet_address(const CryptoPP::ECPPoint& pub_key);
};


#endif //BLOCKCHAIN_CLIENT_H
