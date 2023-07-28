
from my_package.proto.example_pb2 import MyMessage
from my_package.pybind11 import my_extension as m
# import my_package.my_extension as m

import time



def main():
    N = 5000000
    n = 60000
    k = 10
    msg_add = MyMessage()
    msg_add.n = n
    msg_add.k = k
    msg_add.s = 0
    msg_total = MyMessage()
    print('{:.6f} python init start'.format(time.time()))
    msg_add.v.extend(range(1, 1+n))
    msg_add.v.extend([0] * (N-n))
    print('{:.6f} python init end'.format(time.time()))

    # call the function, time it
    print('{:.6f} python call start'.format(time.time()))
    t0 = time.time()

    #m.f(msg_total, msg_add) # why does this not work
    msg_total = m.f(msg_total, msg_add)

    elapsed = time.time() - t0
    print('{:.6f} python call end'.format(time.time()))

    print(f'N={N:<12d} n={n:<12d} k={k:<5d} s={msg_total.s:<20d}   elapsed={elapsed:7.3f}s')


if __name__ == '__main__':
    main()

