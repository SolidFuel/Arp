#include "HashRandom.hpp"

#include "TinySHA1.hpp"

void HashRandom::initialize(const std::uint8_t digest[20]) {
    long long seed = digest[0] |
        ((long long)digest[1] << 8) |
        ((long long)digest[2] << 16) |
        ((long long)digest[3] << 24) |
        ((long long)digest[4] << 32) |
        ((long long)digest[5] << 40) |
        ((long long)digest[6] << 48) |
        ((long long)digest[7] << 56);

    rng_ = std::make_unique<juce::Random>(seed);

}


HashRandom::HashRandom(const std::uint8_t digest[20]) {
    initialize(digest);
}
HashRandom::HashRandom(const std::string &category, long long key, double slot) {
        std::stringstream buf;
        buf << category << ':' << std::hex << key << ':' << slot;

        sha1::SHA1 hash;
        hash.processBytes(buf.str().c_str(), buf.str().size());

        uint8_t digest[20];
        hash.getDigestBytes(digest);
        initialize(digest);

}
int HashRandom::nextInt(int min, int max) {
    return rng_->nextInt({min, max});
}
