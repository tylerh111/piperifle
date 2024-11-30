
libpiperifle_cfg = configuration_data()
libpiperifle_cfg.set_quoted('PIPERIFLE_VERSION', meson.project_version())
libpiperifle_cfg.set_quoted('PIPERIFLE_VERSION_MAJOR', meson.project_version().split('.')[0])
libpiperifle_cfg.set_quoted('PIPERIFLE_VERSION_MINOR', meson.project_version().split('.')[1])

libpiperifle_inc = include_directories('.')

libpiperifle_dep = declare_dependency(
    include_directories: libpiperifle_inc,
)

configure_file(
    input: 'piperifle.hpp.in',
    output: 'piperifle.hpp',
    configuration: libpiperifle_cfg,
    install_dir : get_option('includedir'),
)
