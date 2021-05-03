#include <iostream>
#include "src/Client.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    auto c = new Client();
    std::cout << c->generate_wallet_address(c->calculate_public_key(c->generate_private_key())) << std::endl;
    return 0;
}
