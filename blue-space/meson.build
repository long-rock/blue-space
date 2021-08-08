cpp_sources = [
    'src/main.cc'
]

bluespace = executable(
    'blue-space',
    cpp_sources,
    include_directories: [explorer_inc, miner_common_inc, miner_cpu_inc],
    dependencies: [],
    link_with: [libexplorer, libminer_cpu]
)