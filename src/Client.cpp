//
// Created by nikita on 02.05.21.
//

#include <sstream>
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
    CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(std::cout));

    auto str_pub_key = public_key_to_string(pub_key);
    CryptoPP::SHA256 hash;
    std::string digest;

    hash.Update((const byte*)str_pub_key.data(), str_pub_key.size());
    digest.resize(hash.DigestSize());
    hash.Final((byte*)&digest[0]);

    CryptoPP::RIPEMD160 hash_ripe;
    std::string digest_ripe;
    hash_ripe.Update((const byte*)digest.data(), digest.size());
    digest_ripe.resize(hash_ripe.DigestSize());
    hash_ripe.Final((byte*)&digest_ripe[0]);

    // todo maybe add some real base58 later
    return digest_ripe;
}

std::string Client::public_key_to_string(const CryptoPP::ECPPoint &pub_key) {
    std::stringstream ss;
    ss << pub_key.x << pub_key.y;
    return ss.str();
}
