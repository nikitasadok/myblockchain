//
// Created by nikita on 02.05.21.
//

#ifndef BLOCKCHAIN_TRANSACTION_H
#define BLOCKCHAIN_TRANSACTION_H
#include <string>

class Transaction {
public:
    Transaction(std::string from, std::string to, int64_t amount);
    std::string from;
public:
    const std::string &getFrom() const;

private:
    std::string to;
public:
    const std::string &getTo() const;

private:
    int64_t amount;
public:
    int64_t getAmount() const;

private:
    std::string str();
public:
    std::string get_hash();
};

#endif //BLOCKCHAIN_TRANSACTION_H
