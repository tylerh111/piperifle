
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
        '@OUTDIR@',
        '@INPUT@',
    ],
    build_by_default: false,
    install: true,
    install_dir: get_option('includedir'),
)

libpiperifle_inc = include_directories('.')

libpiperifle_dep = declare_dependency(
    sources: libpiperifle_hdr,
    include_directories: libpiperifle_inc,
)
