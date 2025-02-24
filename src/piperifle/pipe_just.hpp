#ifndef PIPERIFLE_PIPE_JUST_HPP_
#define PIPERIFLE_PIPE_JUST_HPP_

#include <tuple>
#include <utility>

namespace piperifle {
namespace details {

template <typename Root, typename Value>
struct pipe_just_connected_
{
    using root_t = Root;
    using value_t = Value;
    root_t root;
    value_t value;

    template <typename... Args>
    constexpr auto execute(Args&&... args) -> decltype(auto)
    {
        auto taskargs = root.execute(std::forward<Args>(args)...);
        return std::tuple_cat(taskargs, std::make_tuple(value));
    }
};

template <typename Value>
struct pipe_just_
{
    template <typename Root>
    using root_t = pipe_just_connected_<Root, Value>;
    using value_t = Value;
    value_t value;

    template <typename Root>
    constexpr auto connect(Root&& root) {
        return pipe_just_connected_<Root, Value>{std::move(root), std::move(value)};
    }
};

}  // namespace details

template <typename Root, typename Value>
constexpr auto just(Root&& root, Value&& value) -> decltype(auto)
{
    return details::pipe_just_<Value>{std::move(value)}.connect(std::move(root));
}

template <typename Value>
constexpr auto just(Value&& value) -> decltype(auto)
{
    return details::pipe_just_<Value>{std::move(value)};
}

}  // namespace piperifle

#endif  // PIPERIFLE_PIPE_JUST_HPP_
