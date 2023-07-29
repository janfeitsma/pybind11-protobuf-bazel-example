
# Pybind11 Protobuf Bazel Example

Simple example of how to define a protobuf schema and work with it in python & c++ 
interchangably using [pybind11_protobuf](https://github.com/pybind/pybind11_protobuf)

Original source by davidtwomey: https://github.com/davidtwomey/pybind11-protobuf-bazel-example

## Purpose

Stress test performance of passing large data on the interface. See which pybind11 magic and compiler flags etc. are relevant. (Some are mentioned in Davids repo).

## Modifications

* new example C++ function `f` runs a simple operation on provided (large) data
  * sum first `n` values in an array of size `N`
  * repeat `k` times
* protobuf message `M`:
  * large array `v`
  * parameters `n`, `k`
  * sum `s`

## Performance

My naive requirement was that the time between "python call start" and "c++ f start" should not be significant, should not be sensitive to `N`. It should just basically be some pointer copies, so well below 1ms duration.

Trying `PYBIND11_PROTOBUF_ENABLE_PYPROTO_API` did not seem to improve anything.

Using `memoryview` and wrapping function `g` just shifts the problem to the developer, to more explicitly code serialize/deserialize and how to manage memory.

Preliminary conclusion: it seems the overhead cannot be (fully) avoided.

```
$ bazel run my_package:stress
...

testing function f (straightforward)

1690625451.192237 python test start f
1690625451.192247 python call start
1690625452.605811 c++ f start 
field_size: 10000000
Sum of values in v: 18000300000
1690625452.633408 c++ f end 
1690625453.037949 python call end
1690625453.037973 python test end
N=10000000     n=60000        k=10    s=18000300000            elapsed=  1.846s

testing function g (using memoryview)

1690625453.039331 python test start g
1690625454.259873 python using memoryview of size 10103501
1690625454.259897 python call start
1690625454.259909 c++ g start 
1690625454.466894 c++ f start 
field_size: 10000000
Sum of values in v: 18000300000
1690625454.495030 c++ f end 
Buffer size is not sufficient for serialization: 10103501 < 10103507
1690625454.758221 python call end
1690625454.882936 python test end
N=10000000     n=60000        k=10    s=0                      elapsed=  1.844s
```

