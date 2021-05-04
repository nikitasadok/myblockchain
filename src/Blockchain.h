//
// Created by nikita on 02.05.21.
//

#ifndef BLOCKCHAIN_BLOCKCHAIN_H
#define BLOCKCHAIN_BLOCKCHAIN_H
#include "Block.h"
#include <vector>
#include <utility>

class Blockchain {
    Block* last;
    int64_t size;
    std::vector<Transaction> pending_transactions;
    std::vector<std::string> client_addresses;
private:
    void add_block(Block* block);
    Block* get_last_transaction();
    Block* create_block_candidate();
    bool is_valid_guess(const std::string& hash, const int difficulty);
    int64_t get_bal_from_block(const std::string& address, int block_id);
    int64_t get_cur_bal(const std::string& address);
    bool transaction_allowed(const Transaction& tran);
    bool address_is_known(const std::string& address);
public:
    bool is_valid();
    void load_from_file(std::string filename);
    void save_to_file(const std::string& filename);
    void add_transaction(const Transaction& tran);
    void mine();
    std::pair<int64_t, int64_t> get_all_time_min_max(const std::string& address);
    std::vector<std::string> get_addresses_with_money(int block_id);
    std::string get_blockchain_merkle_hash();

    void add_genesis_block(int64_t amount, const std::string &first_receiver);
};


#endif //BLOCKCHAIN_BLOCKCHAIN_H
