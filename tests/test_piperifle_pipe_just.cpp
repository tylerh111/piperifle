

#include <print>

#include <piperifle.hpp>

#define BOOST_TEST_MODULE test_piperifle_pipe_just
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_just1)
{
    int result = 0;
    int passes = 0;

    auto pipeline =
        piperifle::pipeline{}
        | piperifle::just(42)
        ;

    execute(pipeline);
    BOOST_TEST(result == 42);
}


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_just2)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::just(21)
        | piperifle::then([] (int x) { return x + 21; })
        ;

    auto [result] = execute(pipeline);
    BOOST_TEST(result == 42);
}


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_just3)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::then([] (int x) { return x + 1; })
        | piperifle::just(2)
        | piperifle::then([] (int x, int y) { return x + y; })
        ;

    auto [result] = execute(pipeline, 0);
    BOOST_TEST(result == 3);
}


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_just4)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::just("Hello")
        | piperifle::just("World")
        | piperifle::then([] (std::string s1, std::string s2) { return std::format("{}, {}!", s1, s2); })
        ;

    auto [result] = execute(pipeline);
    BOOST_TEST_REQUIRE(result == "Hello, World!");
}

