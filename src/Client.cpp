//
// Created by nikita on 02.05.21.
//

#include <sstream>
#include <utility>
#include "Client.h"
#include "iostream"
#include "crypto++/cryptlib.h"
#include "crypto++/osrng.h"
#include "crypto++/hex.h"
#include "crypto++/ecp.h"
#include "crypto++/eccrypto.h"
#include "crypto++/oids.h"
#include "crypto++/sha.h"
#include "crypto++/ripemd.h"
#include "crypto++/integer.h"
#include "crypto++/files.h"

std::string Client::generate_private_key() {
    CryptoPP::SecByteBlock k_block(32);
    CryptoPP::OS_GenerateRandomBlock(false, k_block, k_block.size());
    std::string k;

    CryptoPP::HexEncoder hex(new CryptoPP::StringSink(k));
    hex.Put(k_block, k_block.size());
    hex.MessageEnd();

    return k;
}

CryptoPP::ECPPoint Client::calculate_public_key(std::string private_key) {
    CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP> group;
    group.Initialize(CryptoPP::ASN1::secp256k1());
    auto base_point = group.GetSubgroupGenerator();

    CryptoPP::StringSource source(private_key, true, new CryptoPP::HexDecoder);
    CryptoPP::Integer pr_key(source, source.MaxRetrievable());

    return group.GetCurve().ScalarMultiply(base_point, pr_key);;
}

std::string Client::generate_wallet_address(const CryptoPP::ECPPoint &pub_key) {
    CryptoPP::RIPEMD160 hash_ripe;
    CryptoPP::SHA256 hash;
    byte digest_ripe[hash_ripe.DigestSize()];
    byte digest[hash.DigestSize()];
    auto str_pub_key = public_key_to_string(pub_key);

    hash.CalculateDigest(digest, (byte *)str_pub_key.c_str(), str_pub_key.length());

    CryptoPP::HexEncoder encoder;
    std::string output;
    encoder.Attach( new CryptoPP::StringSink( output ) );
    encoder.Put( digest, sizeof(digest) );
    encoder.MessageEnd();

    hash.CalculateDigest(digest_ripe, (byte *)output.c_str(), output.length());

    CryptoPP::HexEncoder encoder_ripe;
    std::string output_ripe;
    encoder.Attach( new CryptoPP::StringSink( output_ripe ) );
    encoder.Put( digest_ripe, sizeof(digest_ripe) );
    encoder.MessageEnd();

    // todo maybe add some real base58 later
    return output_ripe;
}

std::string Client::public_key_to_string(const CryptoPP::ECPPoint &pub_key) {
    std::stringstream ss;
    ss << pub_key.x << pub_key.y;
    return ss.str();
}

Client::Client(Blockchain *blockchain) {
    this->blockchain = blockchain;
    this->private_key = generate_private_key();
    this->public_key = calculate_public_key(this->private_key);
    this->address = generate_wallet_address(this->public_key);
}

const std::string &Client::getAddress() const {
    return address;
}

void Client::make_transaction(std::string to, int64_t amount) {
    auto tran = Transaction(this->address, std::move(to), amount);

    this->blockchain->add_transaction(tran);
}
