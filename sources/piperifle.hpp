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
// ---------------------------------------
// concept sender      = ()       -> X
// concept receiver    = (Y, ...) -> void
// concept transformer = (Y, ...) -> X
// concept effector    = ()       -> void
// concept toggler     = (Y)      -> bool
// ---------------------------------------

// template <typename... Args>
// struct completion_signature



// template <typename T>
// concept queryable;


template <typename S>
concept sender =
    requires (S s) {
        [] <typename... Args> (Args... args) {
            auto r = s.run(std::forward<Args>(args)...);
        }
    };

// template <typename R>
// concept receiver = requires {};


// template <typename T>
// concept sender_of {};

// template <typename T>
// concept receiver_of {};


struct pipeline {

    template <typename... Args>
    constexpr auto run(Args... args) -> decltype(auto) {
        return std::make_tuple(std::forward<Args>(args)...);
    }

};


template <typename... Args>
constexpr auto execute(sender auto pipeline, Args... args) -> decltype(auto) {
    return pipeline.run(std::forward<Args>(args)...);
}


template <sender S, typename F>
struct then_sender_ {
    S s_;
    F f_;

    template <typename... Args>
    constexpr auto run(this auto&& self, Args... args) -> decltype(auto) {
        auto fargs = self.s_.run(std::forward<Args>(args)...);
        auto results = std::apply(self.f_, fargs);
        if constexpr (requires {std::tuple_size<decltype(results)>::value;}) {
            return results;
        } else {
            return std::make_tuple(results);
        }
    }

};


template <sender S, typename F>
constexpr auto then(S s, F f) {
    return then_sender_<S, F>(std::move(s), std::move(f));
}


}  // namespace piperifle



#endif  // PIPERIFLE_HPP_
