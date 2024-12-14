# Piperifle

---

[Piperifle](https://github.com/tylerh111/piperifle) is a simplified implementation of C++ Proposal P2300 [`std::execution`](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2300r10.html).
It proposes to add `std::execution` asynchronous support via "sender"s and "receiver"s.
Developers can create tasks that are piped into a scheduler and executed.

The basic idea of "piping" from one task to the next is enough motivation to build a library the implements that behavior.
That is what `piperifle` aims to do.
The immediate concern does not consider scheduling, switching contexts, or managing different value channels.
Its primary goal is to provide a simple interface for developers to build pipelines of tasks while utilizing the "sender"-"receiver" / "producer"-"consumer" pattern spelled out in P2300.

## Usage

Task pipelines can be built easily by connecting them together via `|` operator.
Use an appropriate pipe to wrap the task.
The pipe is the piece that knows how to connect the tasks together.
So long as the value sent by a task can be received by the next task, the connection will be made, and the program will compile.

#### Terminology

* Task: the user defined function that does some work (e.g. lambdas)
* Pipe: the connector between task (e.g. `piperifle::then`)
  * This serves the same purpose as both "sender"s and "receiver"s in P2300.
    There is no real distinguishing quality between these two concepts in `piperifle`.

#### **`piperifle::then`**

```c++
// ---(1)---(2)---
auto pipeline =
    | piperifle::pipeline{}
    | /* (1) */ piperifle::then([] (int x) { return x + 10; })
    | /* (2) */ piperifle::then([] (int x) { return std::format{"Hello, World! {}", x}; })
    ;

auto [result] = piperifle::execute(pipeline, 32);
ASSERT(result == "Hello, World! 42");
```

## Building

The project is built with `meson`.
Use the following to build `piperifle`.

```bash
meson setup build && cd build
meson compile
```

## Roadmap

The following is a list of future execution pipes.

* [x] Simple pipe - run task from previous task's output: `piperifle::then` (`std::execution::then`).
* [ ] Branching pipe - duplicate value to all next tasks: `piperifle::when` (`std::execution::when_all`).
* [ ] Branching pipe - split value out to next applicable tasks: `piperifle::split` (`std::execution::split`).
* [ ] Branching pipe - choose task(s) based on value type: `piperifle::choose` (`std::execution::into_variant`).
* [ ] Stateful pipe - defined a variable for next task: `piperifle::let` (`std::execution::let_value`).
* [ ] Bulk operation pipe - run task multiple times: `piperifle::bulk` (`std::execution::bulk`).
* [ ] Pass-through pipe - run a side effect task: `piperifle::effect` (no equivalent).
* [ ] Generator pipe - produce a value: `piperifle::just` (`std::execution::just`).
* [ ] Collector pipe - consume a value: `piperifle::fill` (no equivalent).

The following is a list of future execution tasks.

* [ ] Reference - read/write from/to address directly (see "generator pipe").
* [ ] Streams - read/write from/to streams via `std::istream`/`std::ostream`
  * [ ] `std::cin` / `std::cout`
  * [ ] `std::fstream`
  * [ ] `std::sstream`
* [ ] Sockets - read/write from/to sockets via ...
  * [ ] [`boost::asio`](https://www.boost.org/doc/libs/release/doc/html/boost_asio.html)/[`asio`](https://think-async.com/Asio/)
  * [ ] [`zmq`](https://zeromq.org/)

The following is a list of future execution concepts.

* [ ] Sender - an object that can send (return) a value.
* [ ] Receiver - an object that can receive (arguments) a value.
* [ ] Schedule - an object that can schedule the task execution.

The following is a list of future execution channels.

* [x] Value Channel - capture values which can be piped to a task.
* [ ] Error channel - capture errors which can be piped to a task.
* [ ] Flag channel - capture user defined flags (stop execution) which can be piped to a task.

The following is a list of future execution options.

* [x] Immediate execution - run the pipeline immediately with values
* [ ] Schedule execution - run the pipeline asynchronously via separate threads
