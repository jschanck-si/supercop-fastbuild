supercop-fastbuild
==================

This is supercop-20140514 with a few changes to make testing new primitives easier.

The main change is the addition of the "do-one-primitive" script. This lets you build
just the packages you need to test the submission you're working on.

For instance, if you want to build pass769:

    $ cd supercop-fastbuild
    $ ./do-one-primitive crypto_core salsa20
    $ ./do-one-primitive crypto_stream salsa20
    $ ./do-one-primitive crypto_hash sha512
    $ ./do-one-primitive crypto_sign pass769

(You may way to read ./bench/hostname/log or ./bench/hostname/work/errors
after each call to do-one-primitive.)

I've also removed most of the compilers from okcompilers/{c,cpp} and told the compilers
to produce debugging information by default.


Original source: http://hyperelliptic.org/ebats/supercop-20140514.tar.bz2

Supercop: http://bench.cr.yp.to/supercop.html

