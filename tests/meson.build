
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

