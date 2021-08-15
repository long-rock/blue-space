#pragma once

#include <gmpxx.h>

namespace miner::cpu::hash
{

void init_mpz(mpz_t n);
void realloc_mpz(mpz_t n);
void wrap_coordinate(mpz_t w, int64_t c);

class Sponge
{
  public:
    Sponge();
    ~Sponge();

    void reset();
    void inject(mpz_srcptr x);
    void mix(mpz_srcptr key);
    void save();
    void restore();
    void result(mpz_t out) const;

  private:
    mpz_t l_;
    mpz_t r_;

    mpz_t snap_l_;
    mpz_t snap_r_;

    mpz_t t0_;
    mpz_t t1_;
    mpz_t t2_;
    mpz_t t3_;
    mpz_t t4_;
};

bool is_planet(mpz_srcptr planet, mpz_srcptr threshold);

} // namespace miner::cpu::hash