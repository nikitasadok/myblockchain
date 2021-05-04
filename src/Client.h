//
// Created by nikita on 02.05.21.
//

#ifndef BLOCKCHAIN_CLIENT_H
#define BLOCKCHAIN_CLIENT_H
#include <string>
#include <crypto++/ecp.h>
#include "Blockchain.h"

class Client {
    std::string private_key;
    CryptoPP::ECPPoint public_key;
    std::string address;
public:
    const std::string &getAddress() const;

private:
    Blockchain* blockchain;

private:
    std::string public_key_to_string(const CryptoPP::ECPPoint& pub_key);
public:
    Client(Blockchain* blockchain);
    std::string generate_private_key();
    CryptoPP::ECPPoint calculate_public_key(std::string private_key);
    std::string generate_wallet_address(const CryptoPP::ECPPoint& pub_key);
    void make_transaction(std::string to, int64_t amount);
};


#endif //BLOCKCHAIN_CLIENT_H
