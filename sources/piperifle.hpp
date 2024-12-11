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


template <typename F, typename... Args>
concept transformable =
    std::invocable<F, Args...>
    && requires(F&& f, Args&&... args) {
        !std::is_void_v<decltype(f(std::forward<Args>(args)...))>;
    };



struct node {

    using kinds = std::variant<transformation>;

    kinds var_;

    template <typename F, typename... Args>
    node(F&& f) requires transformable<F, Args...>
        : var_{
            transformation{
                [f = std::move(f)] (std::any args_hidden) -> std::any {
                    auto args = std::any_cast<std::tuple<Args...>>(args_hidden);
                    return f(std::get<Args>(args)...);
                }
            }
        }
    {}

    template <typename R, typename... Args>
    auto operator() (this node& self, Args&&... args) -> R {
        return std::visit(
            overloaded{
                [...args = std::move(args)] (transformation& f) -> R { return f(std::forward<Args>(args)...); },
            },
            self.var_
        );
    }

};



struct pipeline {

    pipeline() = default;

    auto connect(this auto&& self, auto&& other) -> pipeline&
    {
        self.nodes_.emplace_back(node{std::forward<std::decay_t<decltype(other)>>(other)});
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
