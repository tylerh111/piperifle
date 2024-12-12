#ifndef PIPERIFLE_HPP_
#define PIPERIFLE_HPP_

#include <any>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>


namespace piperifle {

template<typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };


struct source         : std::function<std::any(void)> {};
struct transformation : std::function<std::any(std::any)> {};
struct sink           : std::function<void(std::any)> {};
struct effect         : std::function<void(void)> {};
// struct toggle         : std::function<std::any(std::any)> {};

// using injector = source;
// using reflector = transformation;
// using extractor = sink;
// using effector = effect;
// using toggler = toggle;


template <typename FWrapper>
concept transformable = requires {
    requires !std::is_void_v<typename FWrapper::result>;
    requires 0 < std::tuple_size_v<typename FWrapper::arguments>;
};


template <typename F>
concept callable_with_info = requires {
    typename F::result;
    typename F::arguments;
};


// template <typename...>
// struct function_with_callinfo
// {
//     // using result = void;
//     using arguments = void;
// };

template <typename R, typename... Args>
struct function_with_callinfo
    : std::function<R(Args...)>
{
    using result = R;
    using arguments = std::tuple<Args...>;
};

// template <typename R, typename... Args>
// function_with_callinfo(std::function<R(Args...)>) -> function_with_callinfo<R, Args...>;





struct node {

    using kinds = std::variant<transformation>;

    kinds var_;

    // template <
    //     typename F
    //     // typename FWrapper = function_type_<
    //     //     decltype(&std::decay_t<F>::operator())
    //     // >
    // >
    // node(F&& f)
    //     // requires transformable<function_type_<
    //     //     decltype(&std::decay_t<F>::operator())
    //     // >>
    //     : var_{
    //         transformation{
    //             [f = std::move(f)] (std::any args_hidden) -> std::any
    //             {
    //                 using arguments = typename function_type_<decltype(&std::decay_t<F>::operator())>::arguments;
    //                 auto args = std::any_cast<arguments>(args_hidden);
    //                 return std::make_tuple(std::apply(f, args));
    //             }
    //         }
    //     }
    // {}

    node(callable_with_info auto&& f)
        : var_{
            transformation{
                [f = std::move(f)] (std::any args_hidden) -> std::any
                {
                    using arguments = typename decltype(f)::arguments;
                    auto args = std::any_cast<arguments>(args_hidden);
                    return std::make_tuple(std::apply(f, args));
                }
            }
        }
    {}

    template <typename R, typename... Args>
    auto operator() (this node& self, Args&&... args) -> R {
        return std::visit(
            overloaded{
                [...args = std::move(args)] (transformation& f) -> R
                {
                    return std::get<R>(
                        std::any_cast<std::tuple<R>>(f(std::forward<Args>(args)...))
                    );
                }
            },
            self.var_
        );
    }

};



struct pipeline {

    pipeline() = default;

    auto connect(this auto&& self, auto&& other) -> pipeline&
    {
        // self.nodes_.emplace_back(node{std::forward<std::decay_t<decltype(other)>>(other)});
        self.nodes_.emplace_back(node{function_with_callinfo{std::function{other}}});
        return self;
    }

    inline auto operator|= (this auto&& self, auto&& other) -> pipeline&
    {
        return self.connect(std::forward<std::decay_t<decltype(other)>>(other));
    }

    // auto connect(this pipeline& self, const pipeline&  other) -> pipeline&;
    // auto connect(this pipeline& self,       pipeline&& other) -> pipeline&;
    // auto connect(this pipeline& self, const node&  other    ) -> pipeline&;
    // auto connect(this pipeline& self,       node&& other    ) -> pipeline&;

    // inline auto operator|= (this pipeline& self, const pipeline&  other) -> pipeline& { return self.connect(other); }
    // inline auto operator|= (this pipeline& self,       pipeline&& other) -> pipeline& { return self.connect(other); }
    // inline auto operator|= (this pipeline& self, const node&      other) -> pipeline& { return self.connect(other); }
    // inline auto operator|= (this pipeline& self,       node&&     other) -> pipeline& { return self.connect(other); }

    // auto branch(this pipeline& self, const pipeline&  other) -> pipeline&;
    // auto branch(this pipeline& self,       pipeline&& other) -> pipeline&;
    // auto branch(this pipeline& self, const node&      other) -> pipeline&;
    // auto branch(this pipeline& self,       node&&     other) -> pipeline&;

    // inline auto operator&= (this pipeline& self, const pipeline&  other) -> pipeline& { return self.branch(other); }
    // inline auto operator&= (this pipeline& self,       pipeline&& other) -> pipeline& { return self.branch(other); }
    // inline auto operator&= (this pipeline& self, const node&      other) -> pipeline& { return self.branch(other); }
    // inline auto operator&= (this pipeline& self,       node&&     other) -> pipeline& { return self.branch(other); }

    // auto conditional(this pipeline& self, const pipeline&  other) -> pipeline&;
    // auto conditional(this pipeline& self,       pipeline&& other) -> pipeline&;
    // auto conditional(this pipeline& self, const node&      other) -> pipeline&;
    // auto conditional(this pipeline& self,       node&&     other) -> pipeline&;

    // inline auto operator^= (this pipeline& self, const pipeline&  other) -> pipeline& { return self.conditional(other); }
    // inline auto operator^= (this pipeline& self,       pipeline&& other) -> pipeline& { return self.conditional(other); }
    // inline auto operator^= (this pipeline& self, const node&      other) -> pipeline& { return self.conditional(other); }
    // inline auto operator^= (this pipeline& self,       node&&     other) -> pipeline& { return self.conditional(other); }

    // auto enable(this pipeline& self) -> pipeline&;
    // auto disable(this pipeline& self) -> pipeline&;

    std::vector<node> nodes_;

};


// template <typename T>
// auto feed(pipeline& p, T x) -> T {

// }


}  // namespace piperifle



#endif  // PIPERIFLE_HPP_
