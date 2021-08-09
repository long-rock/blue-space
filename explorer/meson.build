cpp_sources = [
    'lib/storage.cc',
    'lib/explorer.cc'
]

explorer_inc = include_directories([
    'include'
])

leveldb_dep = dependency('leveldb', required: true)
cereal_dep = dependency('cereal', required: true, fallback: ['cereal', 'cereal_dep'])

libexplorer = static_library(
    'explorer',
    cpp_sources,
    include_directories: [explorer_inc, miner_common_inc],
    dependencies: [leveldb_dep, cereal_dep]
)

gtest_main_dep = dependency('gtest', main : true, fallback: ['gtest', 'gtest_main_dep'])
gtest_dep = dependency('gtest', fallback: ['gtest', 'gtest_dep'])

if gtest_dep.found() and gtest_main_dep.found()
    test_sources = [
        'test/explorer_test.cc'
    ]

    ex = executable(
        'test-explorer',
        test_sources,
        dependencies: [gtest_main_dep],
        include_directories: [explorer_inc, miner_common_inc],
        link_with: [libexplorer]
    )

    test('test_explorer', ex)
endif