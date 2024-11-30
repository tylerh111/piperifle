
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




```c++
int main() {

    p = piperifle::Pipeline();

    p
        .add_source(piperifle::Source(...))
        .add_transformation([](piperifle::Container& c) { ... })
        .add_transformation([](piperifle::Container& c) { ... })
        .add_sink(piperifle::Sink(...))
        ;

    p.run(<....>)



}


```


