
libpiperifle_cfg = configuration_data()
libpiperifle_cfg.set_quoted('PIPERIFLE_VERSION', meson.project_version())
libpiperifle_cfg.set_quoted('PIPERIFLE_VERSION_MAJOR', meson.project_version().split('.')[0])
libpiperifle_cfg.set_quoted('PIPERIFLE_VERSION_MINOR', meson.project_version().split('.')[1])

libpiperifle_inc = include_directories('.')

libpiperifle_hdr = custom_target(
    'amalgamate',
    output: 'piperifle.hpp',
    command: [
        'python3',
        '@SOURCE_ROOT@/tools/amalgamate/amalgamate.py',
        '-O',
        '@BUILD_ROOT@/dist',
        '-I',
        '@CURRENT_SOURCE_DIR@',
        '--preamble',
        '@SOURCE_ROOT@/tools/amalgamate/preamble.hpp',
        '@OUTDIR@/piperifle.hpp',
    ],
    build_always_stale: true,
    install: true,
    install_dir: get_option('includedir'),
)

libpiperifle_dep = declare_dependency(
    sources: libpiperifle_hdr,
    include_directories: libpiperifle_inc,
)

configure_file(
    input: 'piperifle.hpp.in',
    output: 'piperifle.hpp',
    configuration: libpiperifle_cfg,
)
