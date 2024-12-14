#ifndef PIPERIFLE_HPP_
#define PIPERIFLE_HPP_

#include <any>
#include <functional>
#include <source_location>
#include <tuple>
#include <concepts>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>


namespace piperifle {

struct pipeline
{
    template <typename... Args>
    constexpr auto run(Args... args) -> decltype(auto)
    {
        return std::make_tuple(std::forward<Args>(args)...);
    }
};


template <typename... Args>
constexpr auto execute(auto pipeline, Args... args) -> decltype(auto)
{
    return pipeline.run(std::forward<Args>(args)...);
}


template <typename Orig, typename Task>
struct then_pipe_connected_
{
    using orig_t = Orig;
    using task_t = Task;
    orig_t orig;
    task_t task;

    template <typename... Args>
    constexpr auto run(Args... args) -> decltype(auto)
    {
        auto taskargs = orig.run(std::forward<Args>(args)...);
        if constexpr (std::is_void_v<decltype(std::apply(task, taskargs))>) {
            std::apply(task, taskargs);
            return;
        } else {
            auto results = std::apply(task, taskargs);
            if constexpr (requires {std::tuple_size<decltype(results)>::value;}) {
                return results;
            }
            return std::make_tuple(results);
        }
    }

};

template <typename Task>
struct then_pipe_
{
    template <typename Orig>
    using orig_t = then_pipe_connected_<Orig, Task>;
    using task_t = Task;
    task_t task;

    template <typename Orig>
    constexpr auto connect(Orig&& orig) {
        return then_pipe_connected_<Orig, Task>{std::move(orig), std::move(task)};
    }
};


template <typename Orig, typename Task>
constexpr auto then(Orig orig, Task task) -> decltype(auto)
{
    return then_pipe_<Task>{std::move(task)}.connect(std::move(orig));
}

template <typename Task>
constexpr auto then(Task task) -> decltype(auto)
{
    return then_pipe_<Task>{std::move(task)};
}

template <typename Orig, typename Pipe>
constexpr auto operator|(Orig&& orig, Pipe&& pipe) -> decltype(auto)
{
    return pipe.connect(std::move(orig));
}


}  // namespace piperifle



#endif  // PIPERIFLE_HPP_
