

#include <print>

#include <piperifle.hpp>

#define BOOST_TEST_MODULE test_piperifle_pipe_construction
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_piperifle_pipe_construction1)
{
    piperifle::Pipe p;
    p.connect(piperifle::Source{"source"});
    p.connect(piperifle::Transform{"transform"});
    p.connect(piperifle::Sink{"sink"});

    auto nodes = p.feed();
    BOOST_CHECK(nodes.at(0).id == "source");
    BOOST_CHECK(nodes.at(1).id == "transform");
    BOOST_CHECK(nodes.at(2).id == "sink");
}
