//
// Created by nikita on 02.05.21.
//

#include <crypto++/sha.h>
#include <sstream>
#include "Block.h"
void Block::add_transaction(const Transaction& tran) {
    this->transactions.push_back(tran);
}

std::string Block::build_merkle_tree(std::vector<std::string> merkle) {
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

std::vector<std::string> Block::hash_block_transactions(const Block &block) {
    CryptoPP::SHA256 hash;
    std::vector<std::string> hashes;
    for (auto b: this->transactions) {
       hashes.push_back(b.get_hash());
    }

    return hashes;
}

Block *Block::get_prev_block() {
    return this->prev_block;
}

std::string Block::get_hash() {
    CryptoPP::SHA256 hash;
    auto block_header_str = this->str();
    std::string digest;

    hash.Update((const byte*)block_header_str.data(), block_header_str.size());
    digest.resize(hash.DigestSize());
    hash.Final((byte*)&digest[0]);

    return digest;
}

BlockHeader Block::get_block_header() {
    return this->header;
}

void Block::setHeader(const BlockHeader &header) {
    Block::header = header;
}

void Block::setTransactionCounter(int64_t transactionCounter) {
    transaction_counter = transactionCounter;
}

void Block::setTransactions(const std::vector<Transaction> &transactions) {
    Block::transactions = transactions;
}

void Block::setPrevBlock(Block *prevBlock) {
    prev_block = prevBlock;
}

std::string Block::get_merkle_root_hash() {
    std::vector<std::string> tran_hashes;
    for (auto tran: this->transactions) {
        tran_hashes.push_back(tran.get_hash());
    }

    return this->build_merkle_tree(tran_hashes);
}

void Block::setBlockHeaderNonce(int i) {
    this->header.nonce = i;
}

void Block::setBlockHeaderTimestamp(int i) {
    this->header.timestamp_unix = i;
}

const std::vector<Transaction> &Block::getTransactions() const {
    return transactions;
}

std::string BlockHeader::str() {
    std::stringstream ss;
    ss << this->hash_prev_block  << this->hash_merkle_root << this->timestamp_unix << this -> target << this->nonce;
    return ss.str();
}

void BlockHeader::setHashMerkleRoot(const std::string &hashMerkleRoot) {
    hash_merkle_root = hashMerkleRoot;
}
