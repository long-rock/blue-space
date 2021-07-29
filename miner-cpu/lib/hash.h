#pragma once

#include <gmpxx.h>

namespace miner
{
namespace cpu
{

class Sponge
{
  public:
    Sponge();
    ~Sponge();

    void reset();
    void inject(mpz_srcptr x);
    void mix(mpz_srcptr key);
    void result(mpz_t out) const;

    void debug() const;

  private:
    mpz_t l_;
    mpz_t r_;

    mpz_t t0_;
    mpz_t t1_;
    mpz_t t2_;
    mpz_t t3_;
    mpz_t t4_;
};

void mimc_hash(Sponge &sponge, mpz_t result, mpz_srcptr x, mpz_srcptr y, mpz_srcptr key);
bool is_planet(mpz_srcptr planet, mpz_srcptr threshold);

} // namespace cpu
} // namespace miner