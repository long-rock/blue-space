#include "miner/cuda/miner.h"

#include "miner/common/constants.h"

#include <cgbn/cgbn.h>
#include <gmpxx.h>

// NEEDS TO BE INCLUDED AFTER CGBN AND GMP
#include "cu_helpers.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace miner::common;
using namespace miner::cuda;

CudaMiner::CudaMiner(int device) : device_(device)
{
}

CudaMiner::~CudaMiner()
{
}

namespace kernel
{

static const uint32_t BIT_SIZE = 512;
static const uint32_t C_SIZE = 232;

typedef cgbn_mem_t<BIT_SIZE> bn_mem_t;

struct MimcParams
{
    bn_mem_t P;
    bn_mem_t *C;
    std::size_t C_size;
};

struct CudaWorkItem
{
    int64_t x;
    int64_t y;
    bool is_planet;
    bn_mem_t hash;
};

template <uint32_t tpi> struct BnParams
{
    static const uint32_t TPI = tpi; // GCBN threads per intstance.
};

void to_mpz(mpz_t r, const bn_mem_t &x)
{
    mpz_import(r, BIT_SIZE / 32, -1, sizeof(uint32_t), 0, 0, x._limbs);
}

void from_mpz(mpz_srcptr s, bn_mem_t &n)
{
    uint32_t count = BIT_SIZE / 32;
    uint32_t *x = n._limbs;
    size_t words;

    if (mpz_sizeinbase(s, 2) > count * 32)
    {
        fprintf(stderr, "from_mpz failed -- result does not fit\n");
        exit(1);
    }

    mpz_export(x, &words, -1, sizeof(uint32_t), 0, 0, s);
    while (words < count)
    {
        x[words++] = 0;
    }
}

template <class env_t, class bn_t = typename env_t::cgbn_t>
__device__ __forceinline__ void wrap_coordinate(env_t &env, bn_t &c_bn, int64_t c, const bn_t &p)
{
    typename env_t::cgbn_t n, m;
    if (c >= 0)
    {
        env.set_ui32(n, c);
        env.set(c_bn, n);
        return;
    }
    env.set_ui32(m, -c);
    env.sub(n, p, m);
    env.set(c_bn, n);
}

template <class env_t, class bn_t = typename env_t::cgbn_t>
__device__ __forceinline__ void field_add(env_t &env, bn_t &r, const bn_t &a, const bn_t &b, bn_t &t, const bn_t &p)
{
    env.add(t, a, b);
    env.rem(r, t, p);
}

template <class env_t, class bn_t = typename env_t::cgbn_t>
__device__ __forceinline__ void field_mul(env_t &env, bn_t &r, const bn_t &a, const bn_t &b, bn_t &t, const bn_t &p)
{
    env.mul(t, a, b);
    env.rem(r, t, p);
}

template <class env_t, class bn_t = typename env_t::cgbn_t>
__device__ __forceinline__ void fifth_power(env_t &env, bn_t &r, const bn_t &n, bn_t &s, bn_t &f, bn_t &t,
                                            const bn_t &p)
{
    field_mul(env, s, n, n, t, p);
    field_mul(env, f, s, s, t, p);
    field_mul(env, r, f, n, t, p);
}

template <class env_t, class bn_t = typename env_t::cgbn_t> class Sponge
{
  public:
    __device__ void reset(env_t &env)
    {
        env.set_ui32(l_, 0);
        env.set_ui32(r_, 0);
    }

    __device__ void inject(env_t &env, const bn_t &x, const bn_t &P)
    {
        field_add(env, l_, l_, x, t0_, P);
    }

    __device__ void mix(env_t &env, const bn_t &key, const bn_t *C, std::size_t C_size, const bn_t &P)
    {
        for (uint32_t j = 0; j < C_size; ++j)
        {
            field_add(env, t0_, key, l_, t1_, P);
            field_add(env, t1_, t0_, C[j], t2_, P);
            fifth_power(env, t0_, t1_, t2_, t3_, t4_, P);
            field_add(env, t1_, t0_, r_, t2_, P);
            env.set(r_, l_);
            env.set(l_, t1_);
        }
        field_add(env, t0_, key, l_, t2_, P);
        fifth_power(env, t1_, t0_, t2_, t3_, t4_, P);
        field_add(env, t0_, t1_, r_, t2_, P);
        env.set(r_, t0_);
    }

    __device__ void result(env_t &env, bn_t &out)
    {
        env.set(out, l_);
    }

  private:
    bn_t l_;
    bn_t r_;

    bn_t t0_;
    bn_t t1_;
    bn_t t2_;
    bn_t t3_;
    bn_t t4_;
};

template <class bn_params>
__global__ void mine_batch_kernel(cgbn_error_report_t *report, CudaWorkItem *batch, std::size_t batch_size,
                                  uint32_t items_per_thread, bn_mem_t planet_threshold_mem, bn_mem_t key_mem,
                                  bn_mem_t P_mem, bn_mem_t *C_mem, std::size_t C_size)
{
    using context_t = cgbn_context_t<bn_params::TPI>;
    using env_t = cgbn_env_t<context_t, BIT_SIZE>;
    using bn_t = typename env_t::cgbn_t;

    context_t ctx(cgbn_report_monitor, report);
    env_t env(ctx);

    uint32_t block_x = blockIdx.x;

    // Copy mimc constants to memory
    bn_t C[C_SIZE];
    for (uint32_t i = 0; i < C_size; ++i)
    {
        env.load(C[i], &(C_mem[i]));
    }

    bn_t P, key, planet_threshold;
    env.load(P, &P_mem);
    env.load(key, &key_mem);
    env.load(planet_threshold, &planet_threshold_mem);

    __syncthreads();

    Sponge<env_t> sponge;
    uint32_t idx;
    bn_t yi, xi, hash;
    for (std::size_t i = 0; i < items_per_thread; ++i)
    {
        idx = block_x * items_per_thread + i;
        if (idx < batch_size)
        {
            wrap_coordinate(env, xi, batch[idx].x, P);
            sponge.reset(env);
            sponge.inject(env, xi, P);
            sponge.mix(env, key, C, C_size, P);
            wrap_coordinate(env, yi, batch[idx].y, P);
            sponge.inject(env, yi, P);
            sponge.mix(env, key, C, C_size, P);
            sponge.result(env, hash);
            env.store(&(batch[idx].hash), hash);
            batch[idx].is_planet = env.compare(hash, planet_threshold) < 0;
        }
    }
}

template <class bn_params>
void run_mine_batch(int device, std::vector<WorkItem> &batch, const MimcParams &mimc, const bn_mem_t &planet_threshold,
                    const bn_mem_t &key)
{
    cgbn_error_report_t *bn_report;

    CUDA_CHECK(cudaSetDevice(device));
    CUDA_CHECK(cgbn_error_report_alloc(&bn_report));

    bn_mem_t *d_C;
    CUDA_CHECK(cudaMalloc(&d_C, sizeof(bn_mem_t) * mimc.C_size));
    CUDA_CHECK(cudaMemcpy(d_C, mimc.C, sizeof(bn_mem_t) * mimc.C_size, cudaMemcpyHostToDevice));

    CudaWorkItem *d_batch, *cpu_batch;
    cpu_batch = static_cast<CudaWorkItem *>(malloc(sizeof(CudaWorkItem) * batch.size()));
    CUDA_CHECK(cudaMalloc(&d_batch, sizeof(CudaWorkItem) * batch.size()));

    for (std::size_t i = 0; i < batch.size(); ++i)
    {
        cpu_batch[i].x = batch[i].x;
        cpu_batch[i].y = batch[i].y;
        cpu_batch[i].is_planet = false;
    }

    CUDA_CHECK(cudaMemcpy(d_batch, cpu_batch, sizeof(CudaWorkItem) * batch.size(), cudaMemcpyHostToDevice));

    uint32_t items_per_thread = 128;

    // grid_size = ceil(size / items_per_thread)
    uint32_t grid_size = (batch.size() + items_per_thread - 1) / items_per_thread;
    dim3 block_size(bn_params::TPI);

    std::cout << "Starting kernel with grid size " << grid_size << std::endl;
    mine_batch_kernel<bn_params><<<grid_size, block_size>>>(bn_report, d_batch, batch.size(), items_per_thread,
                                                            planet_threshold, key, mimc.P, d_C, mimc.C_size);

    CUDA_CHECK(cudaDeviceSynchronize());
    CUDA_CHECK(cudaGetLastError());
    CGBN_CHECK(bn_report);

    CUDA_CHECK(cudaMemcpy(cpu_batch, d_batch, sizeof(CudaWorkItem) * batch.size(), cudaMemcpyDeviceToHost));

    mpz_class planet_hash;
    for (std::size_t i = 0; i < batch.size(); ++i)
    {
        assert(batch[i].x == cpu_batch[i].x && batch[i].y == cpu_batch[i].y);
        batch[i].is_planet = cpu_batch[i].is_planet;
        to_mpz(planet_hash.get_mpz_t(), cpu_batch[i].hash);
        batch[i].hash = planet_hash.get_str();
    }
}

} // namespace kernel

void CudaMiner::mine_batch(std::vector<common::WorkItem> &items, uint32_t rarity, uint32_t key) const
{
    kernel::bn_mem_t planet_threshold_bn, key_bn;

    mpz_class rarity_mpz(rarity);
    mpz_class planet_threshold = P / rarity_mpz;
    kernel::from_mpz(planet_threshold.get_mpz_t(), planet_threshold_bn);

    mpz_class key_mpz(key);
    kernel::from_mpz(key_mpz.get_mpz_t(), key_bn);

    kernel::bn_mem_t P_bn;
    kernel::bn_mem_t C_bn[kernel::C_SIZE];

    assert(C.size() <= kernel::C_SIZE);
    kernel::from_mpz(P.get_mpz_t(), P_bn);
    for (std::size_t i = 0; i < C.size(); i++)
    {
        kernel::from_mpz(C[i].get_mpz_t(), C_bn[i]);
    }

    kernel::MimcParams mimc{
        .P = P_bn,
        .C = C_bn,
        .C_size = C.size(),
    };

    typedef kernel::BnParams<32> bn_params_32;
    kernel::run_mine_batch<bn_params_32>(device_, items, mimc, planet_threshold_bn, key_bn);
}