
# Pybind11 Protobuf Bazel Example

Simple example of how to define a protobuf schema and work with it in python & c++ 
interchangably using [pybind11_protobuf](https://github.com/pybind/pybind11_protobuf)

Original source by davidtwomey: https://github.com/davidtwomey/pybind11-protobuf-bazel-example

## Purpose

Stress test performance of passing large data on the interface. See which compiler flags etc. are relevant. (Some are mentioned in Davids repo).

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

todo table


