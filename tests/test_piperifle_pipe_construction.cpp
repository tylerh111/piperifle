

#include <print>

#include <piperifle.hpp>

#define BOOST_TEST_MODULE test_piperifle_pipe_construction
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_construction1)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::then([]         { return 21; })
        | piperifle::then([] (int x) { return x + 21; })
        ;

    std::println("run pipeline");
    auto [result] = execute(pipeline);
    std::println("run pipeline (done) | results = {}", result);
}


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_construction2)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::then([] (int x) { return x + 1; })
        | piperifle::then([] (int x) { return x + 2; })
        ;

    std::println("run pipeline");
    auto [result] = execute(pipeline, 0);
    std::println("run pipeline (done) | results = {}", result);
}


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_construction3)
{
    auto pipeline =
        piperifle::pipeline{}
        | piperifle::then([] (int x)         { return x + 1; })
        | piperifle::then([] (int x)         { return std::format("magic {}", x); })
        | piperifle::then([] (std::string s) { return std::format("{} magic", s); })
        ;

    std::println("run pipeline");
    auto [result] = execute(pipeline, 0);
    std::println("run pipeline (done) | results = {}", result);
}


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_construction4)
{
    int result = 0;
    int passes = 0;

    auto pipeline =
        piperifle::pipeline{}
        | piperifle::then([]                        { return 42; })
        | piperifle::then([&passes] (int x) mutable { std::println("{}", passes); return x + passes++; })
        | piperifle::then([&result] (int x) mutable { result = x; })
        ;

    std::println("run pipeline");
    execute(pipeline);
    std::println("run pipeline (done) | results = {}", result);
    execute(pipeline);
    std::println("run pipeline (done) | results = {}", result);
    execute(pipeline);
    std::println("run pipeline (done) | results = {}", result);
    execute(pipeline);
    std::println("run pipeline (done) | results = {}", result);

}

