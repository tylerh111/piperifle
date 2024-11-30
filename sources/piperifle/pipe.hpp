#ifndef PIPERIFLE_PIPE_HPP_
#define PIPERIFLE_PIPE_HPP_

#include <string>
#include <vector>

#include <piperifle/node.hpp>

namespace piperifle {
inline namespace v1 {

class Pipe {
public:
    auto feed(this Pipe& self) -> std::vector<Node>;

    auto connect(this Pipe& self, const Node& node) -> Pipe&;
    auto connect(this Pipe& self, Node&& node) -> Pipe&;

    auto operator<<(this Pipe& self, const Node& node) -> Pipe&;
    auto operator<<(this Pipe& self, Node&& node) -> Pipe&;

    auto operator| (this Pipe& self, const Node& node) -> Pipe&;
    auto operator| (this Pipe& self, Node&& node) -> Pipe&;

    auto operator>>(this Pipe& self, const Node& node) -> Pipe&;
    auto operator>>(this Pipe& self, Node&& node) -> Pipe&;

private:
    std::vector<Node> nodes;
};

} // namespace v1
} // namespace piperifle


#endif  // PIPERIFLE_PIPE_HPP_
