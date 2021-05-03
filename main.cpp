#include <iostream>
#include "src/Client.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    auto blockchain = new Blockchain();
    auto clientFrom = new Client(blockchain);
    auto clientTo = new Client(blockchain);
    clientFrom->make_transaction(clientFrom->getAddress(), clientTo->getAddress(), 10000);
    clientTo->make_transaction(clientTo->getAddress(), clientFrom->getAddress(), 5000);
    blockchain->mine();
    std::cout << blockchain->get_all_time_min_max(clientFrom->getAddress()).first << std::endl;
    std::cout << blockchain->get_all_time_min_max(clientFrom->getAddress()).second << std::endl;
//    std::cout << c->generate_wallet_address(c->calculate_public_key(c->generate_private_key())) << std::endl;
    return 0;
}
