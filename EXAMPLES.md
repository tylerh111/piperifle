
# Examples

Trying to make sense of what the problem is and how to solve it.
Figuring out the assumptions that can be made.

---

```c++

// >-o-x
Pipe()
    < Source()
    | Transform()
    | Sink()

// >-o-x
//   |
//   +-x
Pipe()
    < Source()
    | Transform()
    | Sink()
    | Sink()

// >-o-x
//   |
// >-+
Pipe()
    < Source()
    < Source()
    | Transform() // takes both sources
    | Sink()

// >-o-o-x
//     |
// >---+
Pipe()
    < (Source() | Transform())
    < Source()
    | Transform()
    | Sink()
```

---


```c++
int main() {

    p = piperifle::Pipeline();

    p
        .add_source(piperifle::Source(...))
        .add_transformation([](piperifle::Container& c) { ... })
        .add_transformation([](piperifle::Container& c) { ... })
        .add_sink(piperifle::Sink(...))
        ;

    p.run(<....>);  // starts the thread for executing the pipeline

}
```

```c++
int main() {
    p = piperifle::Pipeline();

    s = Source{};
    t = Transformation{};
    x = Sink{};

    p
        .connect(s)
        .connect(t)
        .connect(x)
        ;

    p.start();

    piperifle::feed(s, {10, ...});  // feed a value through the source / pipe
    piperifle::flush(x, n);  // flush a value out of the sink / pipe
}
```



---

question: can we build the pipeline with operators
answer: yes, `connect` or `|`, `<<`, `>>`, etc.

!!! note, the `pipeline` and `piperifle::run` were not meant to do anything (maybe)
!!! they were added to just get something written down

```c++
auto p = pipe{};

piperifle::source::Immediate s;
piperifle::sink::Immediate u;

auto pipeline = piperifle::run(
    p
    << (s = piperifle::sources::Immediate{})
    | [] (int x) { return x + 2; }
    | [] (int x) { return x * 2; }
    >> (u = piperifle::sinks::Immediate{})
    | [] (int x) { return x + 3; }
);

s.feed(1);

// the following cannot be done with a generic `Sink` object.
// all sinks must have a common interface
// an "immediate" sink (e.g. like below) could have an "extract" function
// but a "socket" sink (e.g. writes to network) would not have an "extract" function
int x = u.extract();

p | t | t | t |

```

---

question: why do sources and sinks need `<<` and `>>` respectively
answer: they don't however it makes it clear what they do

```c++
auto p = pipe{};

// `piperifle::source::Immediate` and `piperifle::sink::Immediate` overload call operator
// if there is no input / parameter, it's a source
// if there is no output / return, it's a sink
// this differs from transformations which take input and output
piperifle::source::Immediate s;
piperifle::sink::Immediate u;

auto pipeline = piperifle::run(
    p
    | (s = piperifle::sources::Immediate{})  // `piperifle::sources::Immediate` overloads: `auto operator()()    -> int`
    | [] (int x) { return x + 2; }           // <lambda> (transformation)       overloads: `auto operator()(int) -> int`
    | [] (int x) { return x * 2; }           // <lambda> (transformation)       overloads: `auto operator()(int) -> int`
    | (u = piperifle::sinks::Immediate{})    // `piperifle::sinks::Immediate`   overloads: `void operator()(int)`
    | [] (int x) { return x + 3; }           // <lambda> (transformation)       overloads: `auto operator()(int) -> int`
);

s.feed(1);
int x = u.extract();
```

---

question: how could we make it such that it's only transformations; that is, there are no sources or sinks
answer: either
* we don't allow it (the interface only has ways to open the source and sink)
* we allow it by adding feed / extract which are only applicable if there are no sources or sinks respectfully

the second option is preferred as we could just "ignore" the sources and sinks to allow it to always work

```c++
auto p = pipe{};

auto pipeline = piperifle::run(
    p
    | piperifle::sources::Socket{}
    | [] (int x) { return x + 2; }
    | [] (int x) { return x * 2; }
    | piperifle::sinks::Socket{}
    | [] (int x) { return x + 3; }
);

// ignores the socket source and sink
// only runs the transformations
int x = p.feed(1).extract();
```


---


question: what does `piperifle::run` and `pipeline` for
answer: nothing, maybe??

actually, the `piperifle::run` starts the async threads
perhaps the above `pipeline` object is nothing, and it should just be void
in this case, we don't need the async threads running to execute the pipeline (for debugging)

```c++
auto pipeline = piperifle::pipeline{}
    | piperifle::sources::Socket{}
    | [] (int x) { return x + 2; }
    | [] (int x) { return x * 2; }
    | piperifle::sinks::Socket{}
    | [] (int x) { return x + 3; }
;

// no need to "run" the pipeline
// we can manually feed and extract values
int x = pipeline.feed(1).extract();
```

---

question: should the `|` be used or should `|=`
answer: really, `|=` since we are employing a builder pattern; this also avoids the operator precedence

```c++
auto pipeline = piperifle::pipeline{}
    |= piperifle::sources::Socket{}
    |= [] (int x) { return x + 2; }
    |= [] (int x) { return x * 2; }
    |= piperifle::sinks::Socket{}
    |= [] (int x) { return x + 3; }
;

int x = pipeline.feed(1).extract();
```

---

question: how do we allow for multiple sources and multiple sinks, while also allowing `feed` and `extract` to be called on the `pipeline`.
answer: ...

`feed` should be able to take any number of values corresponding to the number of inputs
`extract` should be able to return multiple values corresponding to the number of outputs

this brings up the problem of multiple convergences and divergences

```c++

// 3 sources
// 4 transformations
// 2 sinks
//       +--->
//       |
// >-o-o-+-o->
//     |   |
// >-o-+   |
// >-------+
auto pipeline = piperifle::pipeline{}
    |= (
        piperifle::pipeline{}
        |= piperifle::sources::Socket{}
        |= [] (int x) { return ...; }
    )
    &= (
        piperifle::pipeline{}
        |= piperifle::sources::Device{}
        |= [] (int y) { return ...; }
    )
    |= [] (int x, int y) { return ...; }
    |= piperifle::sink::File{}  // note, passthrough if pipe continues
    &= piperifle::sources::Stdin{}
    |= [] (int x, int y) { return ...; }



    |= piperifle::sources::Socket{}
    |= [] (int x) { return ...; }
    |= [] (int x) { return ...; }
    |= piperifle::sinks::Socket{}
    |= [] (int x) { return ...; }
;
```

---


question: what operators do we need to handle all cases of splitting and joining pipes.
answer: ...

```
a---b : a |= b

a1-----b : x &= (a1) &= (a2) |= b
    /
a2--


// no, use &= instead (no difference)
a-----b1 : a ^= (b1) ^= (b2) |= x
   \
    --b2
```

```c++
// using pipe (|=)
auto p = pipeline{}
    |= source
    |= transformation
    |= sink

// using split (&=)
auto p = pipeline{}
    &= (pipeline |= source)
    &= (pipeline |= source)  // parens required , new pipeline required (maybe - easier to implement if required)
    |= transformation
    |= sink

// no need
// using join (^=)
auto p = pipeline
    |= source
    ^= (pipeline |= transformation |= sink)
    ^= (pipeline |= transformation |= sink) // parens required , new pipeline required (maybe - easier to implement if required)


auto p = pipeline
    |= source
    &= (pipeline |= transformation |= sink)
    &= (pipeline |= transformation |= sink) // parens required , new pipeline required (maybe - easier to implement if required)
```


---

question: what can and can't we do **in one statement**
answer:

one statement pipelines can represent any graph where nodes are only referenced once
the reason we can't "copy" `transformation(3)` or `transformation(4)` is because we need to reference the pipe itself (not just the function it calls)
when doing `t1 |= t2`, piperifle binds `t1`'s output to one of `t2`'s inputs (where there could be multiple inputs)
we can't bind more than one output (maybe - we could bind a tuple of size 2 to 2 input parameters, but this could get messy and produce undesirable results)

```
(1) not planar

         --1-----3--
        /    \ /    \
... ---o      x      o--- ...
        \    / \    /
         --2-----4--
```


```
(2) node referenced more than once

           3---o--- ...
          /   /
         1---4
        /   /
... ---o---2

```

```c++
// (1)
p2 = pipeline
    |= transformation(3)

p3 = pipeline
    |= transformation(4)

p1 = pipeline
    |= ...
    |= transformation
    &= (pipeline |= transformation(1) &= p2 &= p3)
    &= (pipeline |= transformation(2) &= p2 &= p3)
    |= transformation
    |= ...
```

```c++
// (2)
p2 = pipeline
    |= transformation(4)

p1 = pipeline
    |= ...
    |= transformation
    &= (pipeline |= transformation(1) &= p2 &= transformation(3))
    &= (pipeline |= transformation(2) |= p2)
    |= transformation
    |= ...
```


---

question: how do we do conditionals?
answer: use `^=` which decides which branch (maybe?)

for example:
* choose branch 1 if the pipeline should: "continue doing normal things with the data"
* choose branch 2 if the pipeline should: "the data should be sent to an external process"


```c++
p = pipeline
    |= ...
    |= transformation
    ^= conditional // looks like: [](input, branch...) { return branch; }
    &= (pipeline |= ...)
    &= (pipeline |= ...)
```

question: what does piping after a branch mean?
answer:

```c++
p = pipeline
    |= ...
    |= transformation
    ^= conditional
    &= (pipeline |= ...)
    &= (pipeline |= ...)
    |= transformation // looks like: [] (auto d) { if constexpr (std::is_same_v<d, int>) { ... } else { ... } }
                      // or        : [] (variant d) { ... }
```

question: can we branch or diverge more than 2 ways
answer: yes, use more `&=`, the conditional nodes will have more input parameters

---

question: can we dynamically alter the pipeline
answer: yes (maybe), unknown if we should

for example: if a piece of data indicates that another source should be turned on or swapped with another
e.g. switch to reading a different channel

another reason is if the transformation relies on the pipeline setup, e.g. knowing what comes next affects preparation
however, this example isn't great because it would be better have another transformation that follows a branch before the next transformation

```c++
p = pipeline
    |= ...

    // normal transformation
    |= [] (int x) { ... }

    // pipeline transformation
    |= [] (pipeline& p, node& n, int x) {
        if (...) {
            p.node(n)
                |= transformation
                |= transformation
        }
    }
```

---

question: can we have a valve like operation that stops the flow of data at that point
~answer: maybe use `~=` operator which is a pipe itself which can be toggled on and off (not specifying will allow free pass to the next pipeline)~

cannot use `~=` operator, does not exist

use `.disable()`


```c++
p = pipeline
    |= ...
    |= transformation
    |= (~= transformation) // skipped, but can be enabled dynamically inside or outside the pipeline
    |= transformation
    |= ...
```

---

## Using type erasure for node

see [Breaking Dependencies - C++ Type Erasure - The Implementation Details - Klaus Iglberger CppCon 2022](https://www.youtube.com/watch?v=qn6OqefuH08)
see [Breaking Dependencies: Type Erasure - A Design Analysis - Klaus Iglberger - CppCon 2021](https://www.youtube.com/watch?v=4eeESJQk-mw)

```c++
class Shape {
private:

    struct ShapeConcept {
        virtual ~ShapeConcept() = default;

        virtual void serialize(/* ... */) const = 0;
        virtual void draw(/* ... */) const = 0;
        virtual std::unique_ptr<ShapeConcept> clone() const = 0;
    };

    template <typename T>
    struct ShapeModel : ShapeConcept {
        ShapeModel(T&& value) : object{std::forward<T>(value)} {}

        void serialize(/* ... */) const override { serialize(object, /* ... */); }
        void draw(/* ... */) const override { draw(object, /* ... */); }
        std::unique_ptr<ShapeConcept> clone() const override { return std::make_unique<ShapeModel>(*this); };

        T object;
    };

    friend void serialize(const Shape& s, /* ... */) { s.pimpl->serialize(/* ... */); }
    friend void draw(const Shape& s, /* ... */) { s.pimpl->draw(/* ... */); }
    friend unique_ptr<ShapeConcept> clone(const Shape& s, /* ... */) { return s.pimpl->clone(/* ... */); }

    std::unique_ptr<ShapeConcept> pimpl;

public:

    template <typename T>
    Shape(const T& x) : pimpl{new ShapeModel<T>(x)} {}

    Shape(const Shape& s) :  pimpl{clone(s)} {}
    Shape(Shape&& s) = default;
    Shape& operator= (const Shape& s) { pimpl = clone(s); }
    Shape& operator= (Shape&& s) = default;

};

int main() {
    std::vector<Shape> shapes;
    shapes.emplace_back(Circle{0.0});
    shapes.emplace_back(Square{0.0});
    shapes.emplace_back(Circle{0.0});

    // draw all shapes
    for (const auto& shape : shapes) {
        draw(shape);
    }
}

```



----

## A good place to start

using `std::any`.
create wrapper lambdas that call the connector object (e.g. the lambda the dev passes) and do the any casts there.


---

## based on senders and receivers?

see [P2300r10](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2300r10.html)


specifically:

```c++
// the `values-sent-by(input)` can be a helper function of it's self
// but it's a function of the input type.
// so, we could do something like `typename input::result_t (typename input::arguments_t...)` (probably not, variadic using not allowed)
// but something like this which can pass the information from the previous sender to the next function.

execution::sender auto then(
    execution::sender auto input,
    std::invocable<values-sent-by(input)...> function
);

// ---

auto snd0 = execution::schedule(thread_pool.scheduler());
auto snd1 = execution::then(snd0, []{ return 123; });
auto snd2 = execution::then(snd1, [](int i){ return 123 * 5; })
auto snd3 = execution::then(snd2, [](int i){ return i - 5; });
auto [result] = *this_thread::sync_wait(snd3);
// result == 610

// or

auto snd = execution::schedule(thread_pool.scheduler())
         | execution::then([]{ return 123; })
         | execution::then([](int i){ return 123 * 5; })
         | execution::then([](int i){ return i - 5; });
auto [result] = this_thread::sync_wait(snd).value();
// result == 610

// ---
// extrapolating from declaration
// possible implementation could be
// _something like this_
// point is, the arguments from the previous sender (e.g. the input) can be used in the `std::invocable` concept
// all compile time
// this probably looses some invariability, i.e. no dynamic changes to the pipeline (which sucks...)

// assume `Sender` implements the `sender` concept
template <typename... Args>
struct Sender { using arguments_t = std::tuple<Args...>; };

sender auto then(
    sender auto input,
    std::invocable<typename decltype(input)::arguments_t...> function
) {
    ...
    return Sender{function};
}

```

for piperifle, we need to switch it up a bit

```c++
// pipeline is it's own type and provides builder types
auto plumbing = piperifle::plumbing{};

auto pipline = plumbing.pipeline();
    | []{ return 0; }
    | [] (int x) { return 3.14 + x; }
    // | [] (std::string y) {...}  // would fail to compile
    | [] (double x) { return ""; }
    ...

```

the only way the original idea would work is if we specified the type ahead of time.
that is, `piperifle::pipeline<X>` where `X` is the data being passed from one type to the next
this could be `std::any` which gives us range, but there's still no way of having variadic number of arguments

---

continuing with senders and receivers:

```c++
auto plumbing = piperifle::plumbing{};

auto pipeline = plumbing.pipeline()
    | piperifle::next([](...){...})
    | piperifle::next([](...){...})
    | piperifle::next([](...){...})
    | piperifle::next([](...){...})
    | piperifle::next([](...){...})
    ;

auto [result] = piperifle::feed(pipeline, ...);


// or

auto pipe0 = plumbing.pipeline();
auto pipe1 = piperifle::next(pipe0, [](...){...});
auto pipe2 = piperifle::next(pipe1, [](...){...});
auto pipe3 = piperifle::next(pipe2, [](...){...});
auto pipe4 = piperifle::next(pipe3, [](...){...});
auto pipe5 = piperifle::next(pipe4, [](...){...});

auto [result] = piperifle::feed(pipe5, ...);
```

complex pipeline shapes

```c++
auto plumbing = piperifle::plumbing{};

auto pipeline = plumbing.pipeline()
    | piperifle::from(...)
    | piperifle::then(...)
    | piperifle::to(...)
    ...
    ;
```

```c++

//================
// basic
//================

// transformation
---a--- : ... then(a) ...

// read from source (or copy from provided default value)
a--- : then|from(a) ...
x--- : just(x) ...  // ?? same as: `then([]{ return x; })`

// write to sink
---a : ... then|to(a)

// or deduced by type - using concepts
---a--- : ... then(a) ...
a--- : then(a) ...
---a : ... then(a)

//================
// complex
//================

// chooses which pipe to run next based on received value
// chooses based on:
//  * received value is variant: match type with next pipe
//  * received value is tuple: split out each value into each next pipe
//  * otherwise: duplicate the value to each pipe
------a1------ : ... when|split|choose(then(a1), then(a2)) ...
   \      /
    --a2--

// ---

// choose (variant)
// a1 is run
// a2 is not run
// if T2, pipe stops (if there were a pipe that took T2, then it wouldn't stop)
// never T3
--- <T1, T2> ----- a1(T1) -------
               \            /
                -- a2(T3) --

// split (tuple)
// unlike when, the tuple isn't duplicated across a1, a2
--- <T1, T2> ----- a1(T1) -------
               \            /
                -- a2(T2) --

// when (otherwise)
// duplicate value across both a1, a2
--- <(T1, T2)> ----- a1(T1, T2) -------
                 \                /
                  -- a2(T1, T2) --

// ---

// pipeline "variable"
// this does not allow for `a1 | ai | a2` where ai does not take `x` (new value)
// this does not allow for `a1 | ai | a2` where ai does not take `o` (original value)
// the only way around this is by taking a function
 x-------
    \    \
    |    |
o---a1---a2---a3 : ...

| ...
| let(
    just(x)
    | then([] (auto o, auto x) { ... return {o, x}; })
    | then([] (auto o, auto x) { ... return {o, x}; }))
| a3



// `*` refers to capturing the value instead of receiving the value
 x---a1---a2---a3---a4
     *         *
    *         *
o------------- : ...

| ...
| let([] (auto o) {
    return just(x)
        | then([&o] (auto x) { ... return x; })  // a1
        | then([  ] (auto x) { ... return x; })  // a2
        | then([&o] (auto x) { ... })  // a3
    })
| a3

// ---

// looping over value (i is the loop var)
// p2300 uses `bulk` which passes a "shape"
---a--- : ... bulk|loop|then_again(i, a)
  / \
  \_/

// ---

// effect takes no values and returns no values
***a*** : ... effect(a) ...


// ---

// resource acquisition and de


```





