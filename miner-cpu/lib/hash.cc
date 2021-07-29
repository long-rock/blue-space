#include <gmpxx.h>
#include <iostream>

#include "hash.h"

#include "miner/common/constants.h"

using namespace miner::cpu;

namespace internal
{

void field_mul(mpz_t p, mpz_srcptr a, mpz_srcptr b, mpz_t t)
{
    mpz_mul(t, a, b);
    mpz_tdiv_r(p, t, miner::common::P.get_mpz_t());
}

void field_add(mpz_t p, mpz_srcptr a, mpz_srcptr b, mpz_t t)
{
    mpz_add(t, a, b);
    mpz_tdiv_r(p, t, miner::common::P.get_mpz_t());
}

void fifth_power(mpz_t r, mpz_srcptr n, mpz_t s, mpz_t f, mpz_t t)
{
    field_mul(s, n, n, t);
    field_mul(f, s, s, t);
    field_mul(r, f, n, t);
}

} // namespace internal

miner::cpu::Sponge::Sponge()
{
    mpz_init(l_);
    mpz_init(r_);
    mpz_init(t0_);
    mpz_init(t1_);
    mpz_init(t2_);
    mpz_init(t3_);
    mpz_init(t4_);
}

miner::cpu::Sponge::~Sponge()
{
    mpz_clear(l_);
    mpz_clear(r_);
    mpz_clear(t0_);
    mpz_clear(t1_);
    mpz_clear(t2_);
    mpz_clear(t3_);
    mpz_clear(t4_);
}

void miner::cpu::Sponge::reset()
{
    mpz_set_ui(l_, 0);
    mpz_set_ui(r_, 0);
}

void miner::cpu::Sponge::inject(mpz_srcptr x)
{
    mpz_add(t0_, l_, x);
    mpz_tdiv_r(l_, t0_, miner::common::P.get_mpz_t());
}

void miner::cpu::Sponge::mix(mpz_srcptr key)
{
    for (auto const &c : miner::common::C)
    {
        internal::field_add(t0_, key, l_, t1_);
        internal::field_add(t1_, t0_, c.get_mpz_t(), t2_);
        internal::fifth_power(t0_, t1_, t2_, t3_, t4_);
        internal::field_add(t1_, t0_, r_, t2_);
        mpz_set(r_, l_);
        mpz_set(l_, t1_);
    }
    internal::field_add(t0_, key, l_, t2_);
    internal::fifth_power(t1_, t0_, t2_, t3_, t4_);
    internal::field_add(t0_, t1_, r_, t2_);
    mpz_set(r_, t0_);
}

void miner::cpu::Sponge::result(mpz_t out) const
{
    mpz_set(out, l_);
}

void miner::cpu::Sponge::debug() const
{
    mpz_class l(l_);
    mpz_class r(r_);
    std::cout << "Sponge(l=" << l << ", r=" << r << ")" << std::endl;
}

void miner::cpu::mimc_hash(Sponge &sponge, mpz_t result, mpz_srcptr x, mpz_srcptr y, mpz_srcptr key)
{
    sponge.reset();
    sponge.inject(x);
    sponge.mix(key);

    sponge.inject(y);
    sponge.mix(key);

    sponge.result(result);
}

bool miner::cpu::is_planet(mpz_srcptr planet_hash, mpz_srcptr threshold)
{
    return mpz_cmp(planet_hash, threshold) < 0;
}