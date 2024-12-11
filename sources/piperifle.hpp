#ifndef PIPERIFLE_HPP_
#define PIPERIFLE_HPP_

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>


namespace piperifle {

// template <typename Results, typename... Params>
// struct node {
//     std::string id;
//     std::function<Result (Params...)> op;
//     std::vector<node*> prev;
//     // std::vector<node*> next;
// };

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


struct node {
    virtual ~node() = default;

    template <typename Result, typename... Params>
    auto operator() (this auto&& self, Params&&... params) -> Result {
        if constexpr (std::is_void_v<Result>)
            self(std::forward<Params>(params)...);
        else
            return self(std::forward<Params>(params)...);
    }
};

template <typename Result, typename... Params>
struct transformation : node
{
    using result_t = Result;
    using parameters_t = std::tuple<Params...>;
    virtual auto operator() (this transformation&, Params&&...) -> Result = 0;
};


struct pipeline {

    pipeline() = default;

    auto operator() ()

    template <typename Other>
    auto connect(this pipeline& self, Other&& other) -> pipeline&
    {
        nodes_.emplace_back(std::move(other));
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
