
libpiperifle_config = configuration_data()
libpiperifle_config.set_quoted('PIPERIFLE_VERSION', meson.project_version())

configure_file(
    input: 'piperifle.hpp.in',
    output: 'piperifle.hpp',
    configuration: libpiperifle_config,
    install_dir : get_option('includedir') + '/' + 'foo',
)

install_headers(
    'piperifle/pipe.hpp',
    'piperifle/pipe_then.hpp',
    # 'piperifle.hpp',
    # subdir: 'piperifle',
)

libpiperifle_inc = include_directories('.')

# libpiperifle = library(
#     'piperifle',
#     include_directories: libpiperifle_inc,
#     install: true,
# )

libpiperifle_dep = declare_dependency(
    include_directories: libpiperifle_inc,
    # link_with: libpiperifle,
)
