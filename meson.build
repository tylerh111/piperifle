project(
    'piperifle',
    'cpp',
    version : '0.0',
    license: 'MIT',
    default_options : [
        'warning_level=3',
        'cpp_std=c++23'
    ],
)

subdir('sources')
subdir('tests')

pkg_mod = import('pkgconfig')
pkg_mod.generate(
    name: 'piperifle',
    filebase: 'piperifle',
    description: 'Fast Data Plumbing',
    subdirs: 'piperifle',
    libraries: libpiperifle,
    version: meson.project_version(),
)