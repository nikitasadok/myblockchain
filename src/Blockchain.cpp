//
// Created by nikita on 02.05.21.
//

#include "Blockchain.h"
#include "crypto++/sha.h"
#include <unordered_map>
#include "Utils.h"
#include <fstream>
#include "iostream"
#include <vector>
#include <sstream>

const int blockNumFields = 9;

void Blockchain::add_block(Block* block) {
    block->setPrevBlock(this->last);
    this->last = block;
    this->size++;
    this->pending_transactions.clear();
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
    return hash.substr(0,difficulty) == std::string(difficulty, '0');
}

void Blockchain::mine() {
    auto block_candidate = this->create_block_candidate();
    while (true) {
        for (int i = 0; i < std::numeric_limits<int>::max(); i++) {
            block_candidate->setBlockHeaderNonce(i);
            auto hash = block_candidate->get_hash();
            if (is_valid_guess(hash, 1)) {
                this->add_block(block_candidate);
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
    std::cout << "size: " << this->size << std::endl;
    auto cur_bal = get_cur_bal(address);
    std::pair<int64_t, int64_t> result;
    int64_t min = cur_bal;
    int64_t max = cur_bal;
    std::cout << "min: " << min << std::endl;
    std::cout << "max: " << max << std::endl;

    auto top = this->last;
    while (top != nullptr) {
        for (const auto &tran : top->getTransactions()) {
            if (tran.getFrom() == address) {
                cur_bal += tran.getAmount();
                if (cur_bal > max)
                    max = cur_bal;
            }

            if (tran.getTo() == address) {
                cur_bal -= tran.getAmount();
                if (cur_bal < min)
                    min = cur_bal;
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
    std::vector<std::string> res;

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
    return tran.getAmount() <= this->get_cur_bal(tran.getFrom()) || this->size == 0;
}

bool Blockchain::address_is_known(const std::string& address) {
    return std::find(client_addresses.begin(), client_addresses.end(), address) != client_addresses.end();
}

std::string Blockchain::get_blockchain_merkle_hash() {
    auto top = this->last;
    std::vector<std::string> hashes;

    while (top != nullptr) {
        hashes.push_back(top->get_hash());
        top = top->get_prev_block();
    }

    return Utils::build_merkle_tree(hashes);
}

void Blockchain::load_from_file(std::string filename) {
    std::ifstream file;
    file.open(filename);
    if (!file.is_open())
        return;

    std::string buf;

    std::getline(file, buf);
    auto size = std::stoi(buf);
    for (int i = 0; i < size && !file.eof(); i++) {
        std::cout << "i: " << i << std::endl;
        auto b = new Block();
        std::getline(file, buf);
        std::cout << "buf: " << buf << std::endl;
        b->setHashPrevBlock(buf);
        std::getline(file, buf);
        std::cout << "buf: " << buf << std::endl;
        b->setHashMerkleRoot(buf);
        std::getline(file, buf);
        std::cout << "buf: " << buf << std::endl;
        b->setBlockHeaderTimestamp(std::stoi(buf));
        std::getline(file, buf);
        std::cout << "buf: " << buf << std::endl;
        b->setBlockHeaderNonce(std::stoi(buf));
        std::getline(file, buf);
        std::cout << "buf: " << buf << std::endl;
        b->setTarget(std::stoi(buf));
        std::getline(file, buf);
        std::cout << "buf: " << buf << std::endl;
        auto tranNum = std::stoi(buf);
        for (int j = 0; j < tranNum && !file.eof(); j++) {
            std::getline(file, buf);
            std::cout << "buf: " << buf << std::endl;
            auto from = buf;
            std::getline(file, buf);
            std::cout << "buf: " << buf << std::endl;
            auto to = buf;
            std::getline(file, buf);
            std::cout << "buf: " << buf << std::endl;
            auto amount = std::stoi(buf);
            Transaction tran(from, to, amount);
            b->add_transaction(tran);
        }
        this->add_block(b);
    }


}

void Blockchain::save_to_file(const std::string& filename) {
    std::ofstream file;
    file.open(filename);
    if (!file.is_open())
        return;

    std::vector<Block*> queue;

    file << this->size << std::endl;
    auto topQ = this->last;

    while (topQ != nullptr) {
        queue.push_back(topQ);
        topQ = topQ->get_prev_block();
    }

    for (int i = queue.size() - 1; i >= 0; i--) {
        auto header = queue[i]->get_block_header();
        file <<  header.hash_prev_block << std::endl;
        file <<  header.hash_merkle_root << std::endl;
        file <<  header.timestamp_unix << std::endl;
        file <<  header.nonce << std::endl;
        file <<  header.target << std::endl;
        file <<  queue[i]->getTransactions().size() << std::endl;
        for (const auto& tran: queue[i]->getTransactions()) {
            file << tran.getFrom() << std::endl;
            file << tran.getTo() << std::endl;
            file << tran.getAmount() << std::endl;
        }
    }
   /* while (top != nullptr) {
        auto header = top->get_block_header();
        file <<  header.hash_prev_block << std::endl;
        file <<  header.hash_merkle_root << std::endl;
        file <<  header.timestamp_unix << std::endl;
        file <<  header.nonce << std::endl;
        file <<  header.target << std::endl;
        file <<  top->getTransactions().size() << std::endl;
        for (const auto& tran: top->getTransactions()) {
            file << tran.getFrom() << std::endl;
            file << tran.getTo() << std::endl;
            file << tran.getAmount() << std::endl;
        }
        top = top->get_prev_block();
        cnt++;
    }*/
    file.close();
}

void Blockchain::add_genesis_block(int64_t amount, const std::string& first_receiver) {
    Transaction tran("Satoshi Nakamoto", first_receiver, amount);
    this->add_transaction(tran);
    auto b = new Block();
    b->setTransactions(this->pending_transactions);
    b->setBlockHeaderTimestamp(std::time(nullptr));
    b->setBlockHeaderNonce(1337);
    b->setHashMerkleRoot(b->get_merkle_root_hash());
    add_block(b);
}
