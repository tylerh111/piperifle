

#include <print>

#include <piperifle.hpp>

#define BOOST_TEST_MODULE test_piperifle_pipe_construction
#include <boost/test/unit_test.hpp>


// BOOST_AUTO_TEST_CASE(test_piperifle_pipe_construction1)
// {
//     auto pipeline =
//         piperifle::pipeline{}
//         | piperifle::then([] (int x) { return x + 21; })
//         | piperifle::then([] (int x) { return x + 21; })
//         ;

//     auto [result] = execute(pipeline, 0);
// }


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_construction1)
{
    piperifle::sender auto pipe0 = piperifle::pipeline{};
    piperifle::sender auto pipe1 = piperifle::then(pipe0, [] (int x) { std::println("step 1 {}", x); return x + 1; });
    piperifle::sender auto pipe2 = piperifle::then(pipe1, [] (int x) { std::println("step 2 {}", x); return x; });
    piperifle::sender auto pipe3 = piperifle::then(pipe2, [] (int x) { std::println("step 3 {}", x); return x + 2; });
    piperifle::sender auto pipe4 = piperifle::then(pipe3, [] (int x) { std::println("step 4 {}", x); return x; });

    std::println("run pipeline");
    auto [result] = execute(pipe4, 0);
    std::println("run pipeline (done) | results = {}", result);
}


// BOOST_AUTO_TEST_CASE(test_piperifle_pipe_construction1)
// {
//     int result = 0;

//     auto pipeline =
//         piperifle::pipeline{}
//         | piperifle::then([]                    { return 42; })
//         | piperifle::then([i=0] mutable (int x) { return x + i++; })
//         | piperifle::then([&result]     (int x) { r = x; })
//         ;

//     execute(pipeline);

// }

