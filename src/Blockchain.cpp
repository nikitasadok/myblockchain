//
// Created by nikita on 02.05.21.
//

#include "Blockchain.h"
#include "crypto++/sha.h"
#include <climits>
#include <unordered_map>

void Blockchain::add_block(Block* block) {
    this->last = block;
    this->size++;
}

Block *Blockchain::get_last_transaction() {
    return this->last;
}

bool Blockchain::is_valid() {
    auto top = this->last;
    if (top == nullptr) {
        return true;
    }

    auto copy = *top;
    while (top->get_prev_block() != nullptr) {
        if (top->get_prev_block()->get_hash() != top->get_hash()) {
            return false;
        }
        top = top->get_prev_block();
    }

    this->last = &copy;
    return true;
}

void Blockchain::add_transaction(const Transaction &tran) {
    if (!transaction_allowed(tran))
        return;

    if(!address_is_known(tran.getFrom())) {
        client_addresses.push_back(tran.getFrom());
    }

    if(!address_is_known(tran.getTo())) {
        client_addresses.push_back(tran.getTo());
    }

    this->pending_transactions.push_back(tran);
}

bool Blockchain::is_valid_guess(const std::string& hash, const int difficulty) {
    return hash.substr(0,4) == "0000";
}

void Blockchain::mine() {
    auto block_candidate = this->create_block_candidate();
    while (true) {
        for (int i = 0; i < std::numeric_limits<int>::max(); i++) {
            block_candidate->setBlockHeaderNonce(i);
            auto hash = block_candidate->get_hash();
            if (is_valid_guess(hash, 4)) {
                return;
            }
        }
        block_candidate->setBlockHeaderTimestamp(std::time(nullptr));
    }
 }

Block *Blockchain::create_block_candidate() {
    auto block_candidate_header = BlockHeader();
    block_candidate_header.hash_prev_block = this->last->get_hash();
    block_candidate_header.timestamp_unix = std::time(nullptr);
    block_candidate_header.nonce = 0;

    auto block_candidate = new Block();
    for (const auto& tran : this->pending_transactions)
        block_candidate->add_transaction(tran);

    block_candidate_header.hash_merkle_root = block_candidate->get_merkle_root_hash();

    block_candidate->setHeader(block_candidate_header);
    return block_candidate;
}

std::pair<int64_t, int64_t> Blockchain::get_all_time_min_max(const std::string& address) {
    auto cur_bal = get_bal_from_block(address, this->size - 1);
    std::pair<int64_t, int64_t> result;
    int64_t min, max = cur_bal;

    auto top = this->last;
    while (top != nullptr) {
        for (const auto &tran : top->getTransactions()) {
            if (tran.getFrom() == address) {
                cur_bal -= tran.getAmount();
                if (cur_bal < min)
                    min = cur_bal;
            }

            if (tran.getTo() == address) {
                cur_bal += tran.getAmount();
                if (cur_bal > max)
                    max = cur_bal;
            }
        }
        top = top->get_prev_block();
    }

    result.first = min;
    result.second = max;
    return result;
}

std::vector<std::string> Blockchain::get_addresses_with_money(int block_id) {
    std::unordered_map<std::string, int64_t> wallet_to_sum;
    std::vector<std::string> res;

    for (const auto& w : this->client_addresses) {
        wallet_to_sum[w] = 0;
    }
    auto top = this->last;
    auto last_id = this->size - 1;

    while (last_id != block_id) {
        top = top->get_prev_block();
        last_id--;
    }

    while (top != nullptr) {
        for (const auto &tran : top->getTransactions()) {
            wallet_to_sum[tran.getFrom()] -= tran.getAmount();
            wallet_to_sum[tran.getTo()] += tran.getAmount();
        }
        top = top->get_prev_block();
    }

    for (const auto& entry : wallet_to_sum) {
        if (entry.second > 0) {
            res.push_back(entry.first);
        }
    }

    return res;
}

int64_t Blockchain::get_bal_from_block(const std::string& address, int block_id) {
    int64_t bal = 0;
    std::unordered_map<std::string, int64_t> wallet_to_sum;
    std::vector<std::string> res;

    for (const auto& w : this->client_addresses) {
        wallet_to_sum[w] = 0;
    }
    auto top = this->last;
    auto last_id = this->size - 1;

    while (last_id != block_id) {
        top = top->get_prev_block();
        last_id--;
    }

    while (top != nullptr) {
        for (const auto &tran : top->getTransactions()) {
            if (tran.getFrom() == address)
                bal -= tran.getAmount();
            if (tran.getTo() == address)
                bal += tran.getAmount();
        }
        top = top->get_prev_block();
    }

    return bal;
}

int64_t Blockchain::get_cur_bal(const std::string &address) {
    return this->get_bal_from_block(address, this->size - 1);
}

bool Blockchain::transaction_allowed(const Transaction &tran) {
    return tran.getAmount() <= this->get_cur_bal(tran.getFrom());
}

bool Blockchain::address_is_known(const std::string& address) {
    return std::find(client_addresses.begin(), client_addresses.end(), address) != client_addresses.end();
}
