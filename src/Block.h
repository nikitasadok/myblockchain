//
// Created by nikita on 02.05.21.
//

#ifndef BLOCKCHAIN_BLOCK_H
#define BLOCKCHAIN_BLOCK_H
#include "Transaction.h"
#include <string>
#include <vector>

struct BlockHeader {
    std::string hash_prev_block;
    std::string hash_merkle_root;

    void setHashMerkleRoot(const std::string &hashMerkleRoot);

    int64_t  timestamp_unix;
    int target;
    int nonce;

    std::string str();
};

class Block {
    BlockHeader header;
public:
    void setHeader(const BlockHeader &header);

    void setTransactionCounter(int64_t transactionCounter);

    void setTransactions(const std::vector<Transaction> &transactions);

    void setPrevBlock(Block *prevBlock);

    void setHashMerkleRoot(const std::string& hash);

private:
    int64_t  transaction_counter;
    std::vector<Transaction> transactions;
public:
    const std::vector<Transaction> &getTransactions() const;

private:
    Block* prev_block;

private:
    std::string str();
public:
    std::vector<std::string> hash_block_transactions(const Block& block);
    std::string build_merkle_tree(std::vector<std::string> merkle);
    void add_transaction(const Transaction& tran);
    Block* get_prev_block();
    std::string get_hash();
    std::string get_merkle_root_hash();
    BlockHeader get_block_header();
    void setBlockHeaderNonce(int i);
    void setBlockHeaderTimestamp(int i);
};


#endif //BLOCKCHAIN_BLOCK_H
