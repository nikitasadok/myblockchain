#include <iostream>
#include "src/Client.h"
int main() {
    auto blockchain = new Blockchain();
    auto client1 = new Client(blockchain);
    auto client2 = new Client(blockchain);
    blockchain->add_genesis_block(20000, client1->getAddress());
    client1->make_transaction(client2->getAddress(), 10000);
    blockchain->mine();
    client2->make_transaction(client1->getAddress(), 5000);
    blockchain->mine();
    auto res = blockchain->get_all_time_min_max(client2->getAddress());
    std::cout << res.first << std::endl;
    std::cout << res.second << std::endl;

    blockchain->save_to_file("somefile.txt");
    auto blockchainCopy = new Blockchain();
    auto client1Copy = new Client(blockchainCopy);
    auto client2Copy = new Client(blockchainCopy);
    blockchainCopy->load_from_file("somefile.txt");

    res = blockchainCopy->get_all_time_min_max(client2->getAddress());
    std::cout << res.first << std::endl;
    std::cout << res.second << std::endl;
    return 0;
}
