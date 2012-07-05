Feature: Test how msgpack-autosplit behaves when parsing invalid data

  Feed msgpack-autosplit random, invalid data.

  Scenario: start msgpack-autosplit and feed it random, invalid data.

    Given a directory named "aruba"
    When I run `sh -c 'dd if=/dev/urandom of=- bs=1024 count=64 | msgpack-autosplit -d logdir-gibberish'`
    Then the exit status should be 0
    And a directory named "logdir-gibberish" should exist
