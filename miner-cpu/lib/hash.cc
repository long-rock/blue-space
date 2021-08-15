#include <gmpxx.h>
#include <iostream>

#include "hash.h"

#include "miner/common/constants.h"

using namespace miner::common;
using namespace miner::cpu::hash;

// P needs around 254 bits of storage.
// Use 512 bits integers to avoid overflows.
const uint32_t MPZ_BIT_SIZE = 512;

void miner::cpu::hash::init_mpz(mpz_t n)
{
    mpz_init2(n, MPZ_BIT_SIZE);
}

void miner::cpu::hash::realloc_mpz(mpz_t n)
{
    mpz_realloc2(n, MPZ_BIT_SIZE);
}

void miner::cpu::hash::wrap_coordinate(mpz_t w, int64_t c)
{
    if (c >= 0)
    {
        mpz_set_ui(w, static_cast<uint32_t>(c));
        return;
    }
    uint32_t c_ = static_cast<uint32_t>(-c);
    mpz_sub_ui(w, P.get_mpz_t(), c_);
}


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

miner::cpu::hash::Sponge::Sponge()
{
    init_mpz(l_);
    init_mpz(r_);
    init_mpz(snap_l_);
    init_mpz(snap_r_);
    init_mpz(t0_);
    init_mpz(t1_);
    init_mpz(t2_);
    init_mpz(t3_);
    init_mpz(t4_);
}

miner::cpu::hash::Sponge::~Sponge()
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

void miner::cpu::hash::Sponge::reset()
{
    mpz_set_ui(l_, 0);
    mpz_set_ui(r_, 0);
}

void miner::cpu::hash::Sponge::inject(mpz_srcptr x)
{
    internal::field_add(l_, l_, x, t0_);
}

void miner::cpu::hash::Sponge::mix(mpz_srcptr key)
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

void miner::cpu::hash::Sponge::save()
{
    mpz_set(snap_l_, l_);
    mpz_set(snap_r_, r_);
}

void miner::cpu::hash::Sponge::restore()
{
    mpz_set(l_, snap_l_);
    mpz_set(r_, snap_r_);
}

void miner::cpu::hash::Sponge::result(mpz_t out) const
{
    mpz_set(out, l_);
}

bool miner::cpu::hash::is_planet(mpz_srcptr planet_hash, mpz_srcptr threshold)
{
    return mpz_cmp(planet_hash, threshold) < 0;
}