
#include <print>

#include <piperifle/pipe.hpp>

namespace piperifle {
namespace v0 {

auto Pipe::feed(this Pipe& self) -> std::vector<Node>
{
    std::println("Hello World {}", 1);

    return self.nodes;
}


auto Pipe::connect(this Pipe& self, const Node& node) -> Pipe&
{
    self.nodes.emplace_back(node);
    return self;
}

auto Pipe::connect(this Pipe& self, Node&& node) -> Pipe&
{
    self.nodes.emplace_back(std::move(node));
    return self;
}


auto Pipe::operator<<(this Pipe& self, const Node& node) -> Pipe&
{
    return self.connect(node);
}

auto Pipe::operator<<(this Pipe& self, Node&& node) -> Pipe&
{
    return self.connect(node);
}


auto Pipe::operator| (this Pipe& self, const Node& node) -> Pipe&
{
    return self.connect(node);
}

auto Pipe::operator| (this Pipe& self, Node&& node) -> Pipe&
{
    return self.connect(node);
}


auto Pipe::operator>>(this Pipe& self, const Node& node) -> Pipe&
{
    return self.connect(node);
}

auto Pipe::operator>>(this Pipe& self, Node&& node) -> Pipe&
{
    return self.connect(node);
}




} // namespace v0
} // namespace piperifle

