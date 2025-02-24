
libpiperifle_hdr = custom_target(
    'amalgamate',
    input: meson.project_build_root() + '/src/piperifle.hpp',
    output: 'piperifle.hpp',
    command: [
        'python3',
        '@SOURCE_ROOT@/tools/amalgamate/amalgamate.py',
        '--preamble',
        '@SOURCE_ROOT@/tools/amalgamate/preamble.hpp',
        '-I',
        '@SOURCE_ROOT@/src',
        '-O',
        '@CURRENT_SOURCE_DIR@',
        '-O',
        '@OUTDIR@',
        '@INPUT@',
    ],
    build_always_stale: true,
    install: true,
    install_dir: get_option('includedir'),
)

libpiperifle_inc = include_directories('.')

libpiperifle_dep = declare_dependency(
    sources: libpiperifle_hdr,
    include_directories: libpiperifle_inc,
)
