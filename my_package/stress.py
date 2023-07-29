
from my_package.proto.example_pb2 import MyMessage
from my_package.pybind11 import my_extension as m
# import my_package.my_extension as m

import time




def main():
    N = 10000000
    n = 60000
    k = 10

    msg = MyMessage()
    msg.n = n
    msg.k = k
    msg.s = 0
    msg.v.extend(range(1, 1+n))
    msg.v.extend([0] * (N-n))

    print('\ntesting function f (straightforward)\n')    
    test_function(m.f, msg, False)

    print('\ntesting function g (using memoryview)\n')    
    test_function(m.g, msg, True)


def test_function(func, msg, use_mview):
    t0 = time.time()
    print('{:.6f} python test start {:s}'.format(time.time(), func.__name__))

    # the object to operate on is either msg or a memoryview
    obj = msg
    if use_mview:
        message_bytes = msg.SerializeToString()
        obj = memoryview(message_bytes)
        print('{:.6f} python using memoryview of size {:d}'.format(time.time(), len(message_bytes)))

    # call the function, time it
    print('{:.6f} python call start'.format(time.time()))

    obj = func(obj) # each function uses inout ...

    print('{:.6f} python call end'.format(time.time()))

    if use_mview:
        msg.ParseFromString(bytes(obj))
    else:
        msg = obj

    print('{:.6f} python test end'.format(time.time()))

    elapsed_total = time.time() - t0
    N = len(msg.v)
    print(f'N={N:<12d} n={msg.n:<12d} k={msg.k:<5d} s={msg.s:<20d}   elapsed={elapsed_total:7.3f}s')


if __name__ == '__main__':
    main()

