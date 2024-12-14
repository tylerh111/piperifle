#ifndef PIPERIFLE_PIPE_THEN_HPP_
#define PIPERIFLE_PIPE_THEN_HPP_

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace piperifle {
namespace details {

template <typename Root, typename Task>
struct pipe_then_connected_
{
    using root_t = Root;
    using task_t = Task;
    root_t root;
    task_t task;

    template <typename... Args>
    constexpr auto execute(Args&&... args) -> decltype(auto)
    {
        auto taskargs = root.execute(std::forward<Args>(args)...);
        if constexpr (std::is_void_v<decltype(std::apply(task, taskargs))>) {
            std::apply(task, taskargs);
            return;
        } else if constexpr (
            auto results = std::apply(task, taskargs);
            requires {std::tuple_size<decltype(results)>::value;}) {
            return results;
        } else {
            return std::make_tuple(results);
        }
    }

};

template <typename Task>
struct pipe_then_
{
    template <typename Root>
    using root_t = pipe_then_connected_<Root, Task>;
    using task_t = Task;
    task_t task;

    template <typename Root>
    constexpr auto connect(Root&& root) {
        return pipe_then_connected_<Root, Task>{std::move(root), std::move(task)};
    }
};

}  // namspace details

template <typename Root, typename Task>
constexpr auto then(Root&& root, Task&& task) -> decltype(auto)
{
    return details::pipe_then_<Task>{std::move(task)}.connect(std::move(root));
}

template <typename Task>
constexpr auto then(Task&& task) -> decltype(auto)
{
    return details::pipe_then_<Task>{std::move(task)};
}

}  // namespace piperifle

#endif  // PIPERIFLE_PIPE_THEN_HPP_
