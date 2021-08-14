#include <gmpxx.h>
#include <iostream>

#include "hash.h"

#include "miner/common/constants.h"

using namespace miner::cpu;

// P needs around 254 bits of storage.
// Use 512 bits integers to avoid overflows.
const uint32_t MPZ_BIT_SIZE = 512;

namespace internal
{

void field_add(mpz_t p, mpz_srcptr a, mpz_srcptr b, mpz_t t)
{
    mpz_add(t, a, b);
    mpz_tdiv_r(p, t, miner::common::P.get_mpz_t());
}

void fifth_power(mpz_t r, mpz_srcptr n)
{
   mpz_powm_ui(r, n, 5, miner::common::P.get_mpz_t());
}

} // namespace internal

miner::cpu::Sponge::Sponge()
{
    mpz_init2(l_, MPZ_BIT_SIZE);
    mpz_init2(r_, MPZ_BIT_SIZE);
    mpz_init2(snap_l_, MPZ_BIT_SIZE);
    mpz_init2(snap_r_, MPZ_BIT_SIZE);
    mpz_init2(t0_, MPZ_BIT_SIZE);
    mpz_init2(t1_, MPZ_BIT_SIZE);
    mpz_init2(t2_, MPZ_BIT_SIZE);
    mpz_init2(t3_, MPZ_BIT_SIZE);
    mpz_init2(t4_, MPZ_BIT_SIZE);
}

miner::cpu::Sponge::~Sponge()
{
    mpz_clear(l_);
    mpz_clear(r_);
    mpz_clear(snap_l_);
    mpz_clear(snap_r_);
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
        internal::fifth_power(t0_, t1_);
        internal::field_add(t1_, t0_, r_, t2_);
        mpz_set(r_, l_);
        mpz_set(l_, t1_);
    }
    internal::field_add(t0_, key, l_, t2_);
    internal::fifth_power(t1_, t0_);
    internal::field_add(t0_, t1_, r_, t2_);
    mpz_set(r_, t0_);
}

void miner::cpu::Sponge::save()
{
    mpz_set(snap_l_, l_);
    mpz_set(snap_r_, r_);
}

void miner::cpu::Sponge::restore()
{
    mpz_set(l_, snap_l_);
    mpz_set(r_, snap_r_);
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