//
// Created by nikita on 02.05.21.
//

#include <sstream>
#include <utility>
#include <crypto++/sha.h>
#include "Transaction.h"

std::string Transaction::str() {
    std::stringstream ss;
    ss << this->from  << this->to << this->amount;
    return ss.str();
}

std::string Transaction::get_hash() {
    CryptoPP::SHA256 hash;
    auto tran_str = this->str();
    std::string digest;

    hash.Update((const byte*)tran_str.data(), tran_str.size());
    digest.resize(hash.DigestSize());
    hash.Final((byte*)&digest[0]);

    return digest;
}

const std::string &Transaction::getFrom() const {
    return from;
}

const std::string &Transaction::getTo() const {
    return to;
}

int64_t Transaction::getAmount() const {
    return amount;
}

Transaction::Transaction(std::string from, std::string to, int64_t amount) {
    this->from = std::move(from);
    this->to = std::move(to);
    this->amount = amount;
}