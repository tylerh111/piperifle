
#include <format>

#include <piperifle.hpp>

#define BOOST_TEST_MODULE test_piperifle_pipe_then
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_then1)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::then([]         { return 21; })
        | piperifle::then([] (int x) { return x + 21; })
        ;

    auto [result] = execute(pipeline);
    BOOST_TEST(result == 42);
}


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_then2)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::then([] (int x) { return x + 1; })
        | piperifle::then([] (int x) { return x + 2; })
        ;

    auto [result] = execute(pipeline, 0);
    BOOST_TEST(result == 3);
}


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_then3)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::then([] (int x)         { return x + 1; })
        | piperifle::then([] (int x)         { return std::format("magic {}", x); })
        | piperifle::then([] (std::string s) { return std::format("{} magic", s); })
        ;

    auto [result] = execute(pipeline, 0);
    BOOST_TEST_REQUIRE(result == "magic 1 magic");
}


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_then4)
{
    int result = 0;
    int passes = 0;

    auto pipeline =
        piperifle::pipeline{}
        | piperifle::then([]                        { return 42; })
        | piperifle::then([&passes] (int x) mutable { return x + passes++; })
        | piperifle::then([&result] (int x) mutable { result = x; })
        ;

    execute(pipeline);
    BOOST_TEST(result == 42);
    execute(pipeline);
    BOOST_TEST(result == 43);
    execute(pipeline);
    BOOST_TEST(result == 44);
    execute(pipeline);
    BOOST_TEST(result == 45);
}

