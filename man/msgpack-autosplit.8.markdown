msgpack-autosplit(8) -- A tool to safely rotate msgpack log files
=================================================================

## SYNOPSIS

`msgpack-autosplit` `--dir=<directory>` [<options>]

## DESCRIPTION

Since records in a MessagePack stream are not delimited by carriage
returns, tools like Logrotate can hardly safely rotate this kind of
log file without breaking arbitrary records.

msgpack-autosplit reads a MessagePack stream on the standard input,
writes this stream to disk, and automatically, and safely perform
logfile rotation after a file reaches a maximum size, or after a
maximum delay.

You'd rather use Fluent. Fluent is awesome.
But, well, sometimes, Fluent not an option.

## OPTIONS

  * `-d`, `--dir=<directory>`: set the target directory, where logs
    are going to be written.

  * `-h`, `--help`: show usage.

  * `-s`, `--soft-limit=<bytes>`: set the limit after which a log
    rotation will occur.

  * `-t`, `--rotate-after=<seconds>`: trigger a rotation after this
    many seconds.

  * `-z`, `--compress=<compressor>`: compress data using `compressor`.
    Only `gzip` is currently implemented.

  * `-V`, `--version`: display software version.

## SIMPLE USAGE EXAMPLE

    $ msgpack-autosplit -d /var/log/queries

## ADVANCED USAGE EXAMPLE

    $ msgpack-autosplit -d /var/log/queries -s 1000000 -t 86400 -z gzip
