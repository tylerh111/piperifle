project(
    'piperifle',
    'cpp',
    version: '0.0',
    license: 'MIT',
    license_files: 'LICENSE.md',
)

# options
cpp_std='c++11'
default_library = 'both'

message('::DEBUG:: ', cpp_std)

# dependencies
boost = dependency('boost')

# targets

piperifle_inc = include_directories('src')

piperifle = library(
    'piperifle',
    'src/piperifle.cpp',
    # 'src/piperifle/pipe.cpp',
    install: true,
    include_directories: piperifle_inc,
    dependencies: [
        boost,
    ],
)

install_headers('src/piperifle.hpp')

executable(
    'piperifle_test',
    'src/main.cpp',
    include_directories: piperifle_inc,
    link_with: [
        piperifle,
    ],
)
