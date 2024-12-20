
#include <format>

#include <piperifle.hpp>

#define BOOST_TEST_MODULE test_piperifle_pipe_just
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_just1)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::just(42)
        ;

    auto [result] = execute(pipeline);
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


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_just5)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::just(0)
        | piperifle::just(1)
        ;

    auto [result0, result1] = execute(pipeline);
    BOOST_TEST(result0 == 0);
    BOOST_TEST(result1 == 1);
}
