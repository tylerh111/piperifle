
```c++

// >-o-x
Pipe()
    < Source()
    | Transform()
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



