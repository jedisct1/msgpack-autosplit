Feature: Create a log file

  Feed msgpack-autosplit some data so that it creates a file.
  
  Scenario: start msgpack-autosplit and feed it some data.
  
    Given a directory named "aruba"
    And a file named "tmp/aruba/msgpack-short.data" with short msgpack data
    When I run `sh -c 'msgpack-autosplit -d logdir < msgpack-short.data'`
    Then a directory named "logdir" should exist
    And a file named "logdir/.current" should exist
    And the number of files for "tmp/aruba/logdir/*" should be 1.
    And every file for "tmp/aruba/logdir/*" should be a valid msgpack stream
