msgpack-autosplit
=================

A simple tool to safely rotate logs made of MessagePack records.

Blurb
-----

Since records in a MessagePack stream are not delimited by carriage
returns, tools like Logrotate can hardly safely rotate this kind of
log file without breaking arbitrary records.

msgpack-autosplit reads a MessagePack stream on the standard input,
writes this stream to disk, and automatically, and safely perform
logfile rotation after a file reaches a maximum size, or after a
maximum delay.

You'd rather use Fluent. Fluent is awesome.
But, well, sometimes, Fluent not an option.

Installation
------------

Download a [msgpack-autosplit tarball](https://github.com/jedisct1/msgpack-autosplit/downloads)
from Github.

Make sure that you have the MessagePack and Zlib libraries installed,
with their development headers.

Compile and install `msgpack-autosplit` using the standard procedure:

    $ ./configure
    # make install

Usage
-----

    $ man 8 msgpack-autosplit
    
should be enough to get you started.
