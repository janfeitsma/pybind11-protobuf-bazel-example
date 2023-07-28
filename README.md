
# Pybind11 Protobuf Bazel Example

Simple example of how to define a protobuf schema and work with it in python & c++ 
interchangably using [pybind11_protobuf](https://github.com/pybind/pybind11_protobuf)

Original source by davidtwomey: https://github.com/davidtwomey/pybind11-protobuf-bazel-example

## Purpose

Stress test performance of passing large data on the interface. See which pybind11 magic and compiler flags etc. are relevant. (Some are mentioned in Davids repo).

## Modifications

* example C++ function runs a simple operation on provided (large) data
  * add first `n` values in an array of size `N`
  * repeat `k` times
* its interface has two instances of the same protobuf message on the interface:
  * running total (modified inline)
  * const list to be added
* protobuf message `M`:
  * large array `v`
  * parameters `n`, `k`
  * sum `s`

## Performance

NOT OK YET - MAY NEED HELP.

The time between "python call start" and "c++ f start" should not be 1 second, should not be sensitive to `N`.

It should just basically be some pointer copies, so well below 1ms duration.

Trying `PYBIND11_PROTOBUF_ENABLE_PYPROTO_API` did not seem to improve anything.

```
$ bazel run my_package:stress
...
1690556546.866389 python init start
1690556547.891195 python init end
1690556547.891225 python call start
1690556549.019999 c++ f start 
1690556549.142866 c++ f allocated 
1690556550.413722 c++ f end 
1690556550.716841 python call end
N=5000000      n=60000        k=10    s=18000300000            elapsed=  2.826s


$ bazel run my_package:stress --cxxopt=-DPYBIND11_PROTOBUF_ENABLE_PYPROTO_API=1
...
1690556730.830657 python init start
1690556731.844800 python init end
1690556731.844828 python call start
1690556732.979426 c++ f start 
1690556733.101091 c++ f allocated 
1690556734.388063 c++ f end 
1690556734.692237 python call end
N=5000000      n=60000        k=10    s=18000300000            elapsed=  2.847s

```

