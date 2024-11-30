
libpiperifle_config = configuration_data()
libpiperifle_config.set_quoted('PIPERIFLE_VERSION', meson.project_version())

configure_file(
    input: 'piperifle/config.hpp.in',
    output: 'config.hpp',
    configuration: libpiperifle_config,
)

install_headers(
    'piperifle.hpp',
    subdir: 'piperifle',
)

libpiperifle_inc = include_directories('.')

libpiperifle = library(
    'piperifle',
    'piperifle/pipe.cpp',
    include_directories: libpiperifle_inc,
    install: true,
)

libpiperifle_dep = declare_dependency(
    include_directories: libpiperifle_inc,
    link_with: libpiperifle,
)

