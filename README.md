# Piperifle

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

#### Pipeline Diagram Notation

The following describe the diagram notation used below.

* A `-` or `/\` represent the flow of the pipeline.
* A `=` indicates the value is duplicated for the next pipe.
* A `~` represents the flow of pipeline, noting that it is a passthrough.
* A `(0)` represents one of the pipes in the pipeline.
* A `*` indicates the value is captured but not an input to the pipe, e.g. a lambda capture.
* A `(   )` indicates the pipe operation is contained in the brackets.
* A `[   ]` represents a subpipeline.

### **`piperifle::then`**

The most common pipe is `then`.
It takes the previous pipe's output as input and returns the next value in the pipeline.
It is equivalent to "transforming".

Transformations are `then` pipe tasks that have both input and output.
Sources are `then` pipe tasks that have no input.
Sinks are `then` pipe tasks that have no output.
A task with no input and no output do not do anything; typically this is an error, though it is still permitted.

```c++
// ---(0)---(1)---
auto pipeline =
  piperifle::pipeline{}
  | /* (0) */ piperifle::then([] (int x) { return x + 42; })
  | /* (1) */ piperifle::then([] (int x) { return std::format("Hello, World! {}", x); })
  ;

auto [result] = piperifle::execute(pipeline, 0);
assert((result == "Hello, World! 42"));
```

### **`piperifle::just`**

A `just` pipe acts as a source by producing the value that is provided.
It stores it locally and produced when executed.

Note, a `just` pipe can appear anywhere in the pipe.
It will append its value to the list of arguments for the next pipe.
When it appears that the beginning of the pipeline, it will provide the first and only argument to the next pipe.

```c++
// ---(0)--(-------(1)-)--(2)---
//         ( o--/      )
auto pipeline =
  piperifle::pipeline{}
  | /* (0) */ piperifle::then([] (int x) { return x + 4; })
  | /* (1) */ piperifle::just(2)
  | /* (2) */ piperifle::then([] (int x, int y) { return std::format("Hello, World! {}{}", x, y); })
  ;

auto [result] = piperifle::execute(pipeline);
assert((result == "Hello, World! 42"));
```

### (WIP) **`piperifle::fill`**

A `fill` pipe acts as a sink by storing the value at the given address or reference.
The reference must be alive by the time the pipeline is executed.

Note, a `fill` pipe can appear anywhere in the pipe.
It will propagate the previously sent value to the next pipe.

```c++
// ---(0)--(-(1)-------)--(2)---
//         (      \--o )
int immediate = 0;
auto pipeline =
  piperifle::pipeline{}
  | /* (0) */ piperifle::then([] (int x) { return x + 42; })
  | /* (1) */ piperifle::fill(&immediate)
  | /* (2) */ piperifle::then([] (int x) { return std::format("Hello, World! {}", x); })
  ;

auto [result] = piperifle::execute(pipeline, 0);
assert((result == "Hello, World! 42"));
assert((immediate == 42));
```

### (WIP) **`piperifle::when`**

A `when` pipe will duplicate (copy) the input values and send them to all provided pipes - the subpipelines.
The copy is unconditionally, including when only one pipe is captured.
The result of the `when` pipe is the collection of all output values of the subpipelines.
The subpipelines need not produce the same type.

Note, this is different from `split` and `choose` pipelines.
All three "branch" the pipeline, but `when` duplicates the input as apposed to separating the input or selecting the next pipes.

```c++
// ---(-(0)--               --)---
//    (      \==[--(1)--]==/  )
//    (      \==[--(2)--]==/  )
//    (      \==[--(3)--]==/  )
auto pipeline =
  piperifle::pipeline{}
  | /* (0) */ piperifle::when(
      /* (1) */ piperifle::then([] (int x) { return x + 0; }),
      /* (2) */ piperifle::then([] (int x) { return x + 1; }),
      /* (3) */ piperifle::then([] (int x) { return x + 2; })
  )
  ;

auto [result1, result2, result3] = piperifle::execute(pipeline, 0);
assert((result0 == 0));
assert((result1 == 1));
assert((result2 == 2));
```

### (WIP) **`piperifle::split`**

A `split` pipe will separate the input values (e.g. a tuple of values) and send them to provided pipes - the subpipelines.
Each next pipe will get exactly one value from the input in the order the subpipelines were provided.
The result of the `split` pipe is the collection of all output values of the subpipelines.
The subpipelines need not produce the same type.

Note, this is different from `when` and `choose` pipelines.
All three "branch" the pipeline, but `split` separates the multivalued input as apposed to duplicating the input value or selecting the next pipes.

```c++
// ---(-(0)--               --)---
//    (      \--[--(1)--]--/  )
//    (      \--[--(2)--]--/  )
//    (      \--[--(3)--]--/  )
auto pipeline =
  piperifle::pipeline{}
  | /* (0) */ piperifle::split(
      /* (1) */ piperifle::then([] (int         x) { return x + 42; }),
      /* (2) */ piperifle::then([] (double      y) { return y * 2; })
      /* (3) */ piperifle::then([] (std::string z) { return std::format("{}, World!", z); }),
  )
  ;

auto [result1, result2, result3] = piperifle::execute(pipeline, 0, 3.14, "Hello");
assert((result0 == 42));
assert((result1 == 6.28));
assert((result2 == "Hello, World!"));
```

### (WIP) **`piperifle::choose`**

A `choose` pipe will evaluate the input value type (e.g. a variant of types) and select the appropriate pipe - the subpipelines - to run.
Only one pipe will be chosen per input.
In the case of a multivalued input, multiple pipes will be run based on each type of each value.
The result of the `choose` pipe is the collection of all output values of the subpipelines.

Note, this is different from `when` and `split` pipelines.
All three "branch" the pipeline, but `choose` selects the next pipeline as apposed to duplicating the input value or separating input.

```c++
// ---(-(0)--               --)---
//    (      \  [--(1)--]  /  )
//    (      \  [--(2)--]  /  )
//    (      \--[--(3)--]--/  )
auto pipeline =
  piperifle::pipeline{}
  | /* (0) */ piperifle::split(
      /* (1) */ piperifle::then([] (int         x) { return x + 42; }),
      /* (2) */ piperifle::then([] (double      y) { return y * 2; })
      /* (3) */ piperifle::then([] (std::string z) { return std::format("{}, World!", z); }),
  )
  ;

auto [result] = piperifle::execute(pipeline, "Hello");
assert((result == "Hello, World!"));
```

### (WIP) **`piperifle::let`**

A `let` pipe defines a pipeline variable.
Essentially, it will start a new pipeline with the variable but still have access to the original value.
The pipeline variable will exist through the pipeline execution so long as the next value is used.

Note, in the following example, the `piperifle::just("Hello")` is the new "variable" defined for the tasks.
The previous piped value is stored in `o` and available for capture.

```c++
// ---(--(0)---------      )
//    (             *      )
//    (    [--(1)---(2)--]-)---
auto pipeline =
  piperifle::pipeline{}
  | /* (0) */ piperifle::let(
    [] (int o) {
      return
        piperifle::pipeline{}
        | /* (1) */ piperifle::just("Hello")
        | /* (2) */ piperifle::then([&o] (std::string x) { return std::format("{}, World! {}", x, o); })
        ;
    }
  )
  ;

auto [result] = piperifle::execute(pipeline, 42);
assert((result == "Hello, World! 42"));
```

### (WIP) **`piperifle::bulk`**

A `bulk` pipe implements a looping mechanism into the pipeline.
The bulk operation depends on shape that dictates how the task is executed.

Currently, only integral types are considered.
Other types could be considered in the future, e.g. predicates, arrays, multidimensional arrays, etc.

```c++
// ---(~~~~~~~~~~~~~)---
//    (   \         )
//  n-(-i----(0)--\ )
//    (       \---/ )
auto n = 3;
auto pipeline =
  piperifle::pipeline{}
  | /* (0) */ piperifle::bulk(n, [] (int i, std::vector<int>& v) { ++v[i]; })
  ;

auto [results] = piperifle::execute(pipeline, {0, 1, 2});
assert((results[0] == 1));
assert((results[1] == 2));
assert((results[2] == 3));
```

### (WIP) **`piperifle::effect`**

An `effect` pipe produces a side effect.
Tasks may not have arguments or a result.

```c++
// ---(~~~~~)---
//    ( (0) )
auto pipeline =
  piperifle::pipeline{}
  | /* (0) */ piperifle::effect([] { std::println("debug: here"); })
  ;

piperifle::execute(pipeline, 42);
```

## Building

The project is built with `meson`.
Use the following to build `piperifle`.

```bash
meson setup      build
meson compile -C build
meson test    -C build
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
* [x] Generator pipe - produce a value: `piperifle::just` (`std::execution::just`).
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
* [ ] Vectors - a collection that can be executed in parallel.

The following is a list of future execution channels.

* [x] Value Channel - capture values which can be piped to a task.
* [ ] Error channel - capture errors which can be piped to a task.
* [ ] Flag channel - capture user defined flags (stop execution) which can be piped to a task.

The following is a list of future execution options.

* [x] Immediate execution - run the pipeline immediately with values.
* [ ] Schedule execution - run the pipeline asynchronously via separate threads.
* [ ] Vectorized execution - allow pipes to parallelize task execution with collections.

The following is a list of other future ideas.

* [ ] Amalgamation of C++ headers.
* [ ] Conan support.
* [ ] Vcpkg support.
