
#include <piperifle.hpp>

#define BOOST_TEST_MODULE test_piperifle_pipe
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_piperifle_pipe1)
{
    auto pipeline =
        piperifle::pipeline{}
        ;

    execute(pipeline);
}

