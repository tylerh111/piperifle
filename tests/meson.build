
# testing framework
boost_unit_test_framework = dependency(
    'boost',
    modules: 'unit_test_framework',
)


test_piperifle_pipe_construction = executable(
    'test_piperifle_pipe_construction',
    'test_piperifle_pipe_construction.cpp',
    dependencies: [
        libpiperifle_dep,
        boost_unit_test_framework,
    ],
)

test(
    'test_piperifle_pipe_construction',
    test_piperifle_pipe_construction,
)

