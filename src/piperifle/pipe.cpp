
#define __cpp_lib_print
#include <print>
#undef __cpp_lib_print

#include <piperifle/pipe.hpp>

namespace piperifle {
inline namespace v1 {

auto Pipe::feed() -> void {
    std::println("Hello World {}", 1);
}

} // namespace v1
} // namespace piperifle

