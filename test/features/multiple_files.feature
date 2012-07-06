Feature: Create multiple log files

  Feed msgpack-autosplit a lot of data so that it creates multiple files
  
  Scenario: start msgpack-autosplit and feed it a lot of data.
  
    Given a directory named "aruba"
    And a file named "tmp/aruba/msgpack-large.data" with large msgpack data
    When I run `sh -c 'msgpack-autosplit -s 1000 -d logdir-large < msgpack-large.data'`
    Then a directory named "logdir-large" should exist
    And a file named "logdir-large/.current" should exist
    And the number of files for "tmp/aruba/logdir-large/*" should be 500.
    And every file for "tmp/aruba/logdir-large/*" should be a valid msgpack stream
