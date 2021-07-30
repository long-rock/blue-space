cpp_sources = [
    'lib/hash.cc',
    'lib/miner.cc'
]

miner_cpu_inc = include_directories([
    'include'
])

gmp_dep = dependency('gmp')
gmpxx_dep = dependency('gmpxx')
omp = dependency('openmp')

libminer_cpu = static_library(
    'miner-cpu',
    cpp_sources,
    include_directories: [miner_common_inc, miner_cpu_inc],
    dependencies: [gmp_dep, gmpxx_dep, omp]
)

gtest_main_dep = dependency('gtest', main : true, fallback: ['gtest', 'gtest_main_dep'])
gtest_dep = dependency('gtest', fallback: ['gtest', 'gtest_dep'])

if gtest_dep.found() and gtest_main_dep.found()
    test_sources = [
        'test/cpu_miner_test.cc'
    ]

    ex = executable(
        'test-miner-cpu',
        test_sources,
        dependencies: [gtest_main_dep],
        include_directories: [miner_common_inc, miner_cpu_inc],
        link_with: [libminer_cpu]
    )

    test('test_miner_cpu', ex)
endif

benchmark_main_dep = dependency('google-benchmark', main : true, fallback: ['google-benchmark', 'google_benchmark_main_dep'])
benchmark_dep = dependency('google-benchmark', fallback: ['google-benchmark', 'google_benchmark_dep'])

if benchmark_dep.found() and benchmark_main_dep.found()
    benchmark_sources = [
        'test/cpu_miner_benchmark.cc'
    ]

    ex = executable(
        'benchmark-miner-cpu',
        benchmark_sources,
        dependencies: [benchmark_main_dep, benchmark_dep],
        include_directories: [miner_common_inc, miner_cpu_inc],
        link_with: [libminer_cpu]
    )

    test('benchmark_miner_cpu', ex)
endif