#ifndef PIPERIFLE_PIPE_HPP_
#define PIPERIFLE_PIPE_HPP_

#include <tuple>
#include <utility>


namespace piperifle {

struct pipeline
{
    template <typename... Args>
    constexpr auto execute(Args... args) -> decltype(auto)
    {
        return std::make_tuple(std::forward<Args>(args)...);
    }
};

template <typename... Args>
constexpr auto execute(auto&& pipe, Args... args) -> decltype(auto)
{
    return pipe.execute(std::forward<Args>(args)...);
}

template <typename Root, typename Pipe>
constexpr auto connect(Root&& root, Pipe&& pipe) -> decltype(auto)
{
    return pipe.connect(std::move(root));
}

template <typename Root, typename Pipe>
inline constexpr auto operator|(Root&& root, Pipe&& pipe) -> decltype(auto)
{
    return connect(std::forward<Root>(root), std::forward<Pipe>(pipe));
}

}  // namespace piperifle

#endif  // PIPERIFLE_PIPE_THEN_HPP_
