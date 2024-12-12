

#include <print>

#include <piperifle.hpp>

#define BOOST_TEST_MODULE test_piperifle_pipe_construction
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_construction1)
{
    auto p = piperifle::pipeline{}
        |= [] (int x) {
            std::println("transformation {}", x);
            x += 1;
            std::println("transformation {} (done)", x);
            return x;
        }
        ;

    piperifle::feed(p, 0);
}

// BOOST_AUTO_TEST_CASE(test_piperifle_pipe_construction2)
// {
//     auto p = piperifle::pipeline{};
//         |= [] () { int x = 0; std::println("{} piping", x); return x; }
//         |= [] (int x) { x += 1; std::println("{} piping", x); return x; }
//         |= [] (int x) { std::println("{} piping (done)", x); }
//         ;

// }
