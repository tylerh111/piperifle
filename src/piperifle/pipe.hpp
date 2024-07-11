

#include <piperifle/sink.hpp>
#include <piperifle/source.hpp>
#include <piperifle/transform.hpp>

namespace piperifle {
inline namespace v1 {

class Pipe {
public:
    auto feed() -> void;

    template <typename In, typename Out> auto operator<(this Pipe& self, Source<In, Out>& node) -> Pipe& { return self; }
    template <typename In, typename Out> auto operator|(this Pipe& self, Transform<In, Out>& node) -> Pipe& { return self; }
    template <typename In, typename Out> auto operator|(this Pipe& self, Sink<In, Out>& node) -> Pipe& { return self; }
};

} // namespace v1
} // namespace piperifle

