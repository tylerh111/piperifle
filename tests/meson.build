
testing_framework = dependency(
    'boost',
    modules: 'unit_test_framework',
    required: false,
)

if not testing_framework.found()
    warning('cannot find dependency required for unit tests: "boost/unit_test_framework"')
    warning('skipping unit test recipes')
    subdir_done()
endif


test_piperifle_pipe = executable(
    'test_piperifle_pipe',
    'test_piperifle_pipe.cpp',
    dependencies: [
        libpiperifle_dep,
        testing_framework,
    ],
)

test_piperifle_pipe_just = executable(
    'test_piperifle_pipe_just',
    'test_piperifle_pipe_just.cpp',
    dependencies: [
        libpiperifle_dep,
        testing_framework,
    ],
)

test_piperifle_pipe_then = executable(
    'test_piperifle_pipe_then',
    'test_piperifle_pipe_then.cpp',
    dependencies: [
        libpiperifle_dep,
        testing_framework,
    ],
)


test(
    'test_piperifle_pipe',
    test_piperifle_pipe,
)

test(
    'test_piperifle_pipe_just',
    test_piperifle_pipe_just,
)

test(
    'test_piperifle_pipe_then',
    test_piperifle_pipe_then,
)
