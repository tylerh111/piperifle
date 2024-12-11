#ifndef PIPERIFLE_HPP_
#define PIPERIFLE_HPP_

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>


namespace piperifle {

// struct source : node {};
// struct transformation : node {};
// struct sink : node {};
// struct effect : node {};
// struct toggle : node {};

// using injector = source;
// using reflector = transformation;
// using extractor = sink;
// using effector = effect;
// using toggler = toggle;


struct data {

}


struct transformation
{
};




struct node {

    template <typename F, typename R, typename... Args>
    struct node_model {

    };

    template <typename F, typename R, typename... Args>
    node(std::callable F&& f)
        :


    template <typename R, typename... Args>
    auto operator() (this node& self, Args&&... params) -> R {
        template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
        std::visit(
            overloaded{
                [=] (transformation<R, Args...>& op) -> R { return op(std::forward<Args>(params)...); },
            },
            self.var
        )
    }

};



struct pipeline {

    pipeline() = default;

    auto operator() ()

    template <typename Other>
    auto connect(this pipeline& self, Other&& other) -> pipeline&
    {
        nodes_.emplace_back(/* ... */); // store other some how via a conversion to node
    }

    inline auto operator|= (this pipeline& self, auto&& other) -> pipeline&
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


template <typename T>
auto feed(pipeline& p, T x) -> T {

}


}  // namespace piperifle



#endif  // PIPERIFLE_HPP_
