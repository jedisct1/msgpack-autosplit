Feature: Create a gzipped log file

  Feed msgpack-autosplit some data so that it creates a gzipped file.

  Scenario: start msgpack-autosplit with gzip compression and feed it
  some data.
  
    Given a directory named "aruba"
    And a file named "tmp/aruba/msgpack-short.data" with short msgpack data
    When I run `sh -c 'msgpack-autosplit -z gzip -d logdir-gzip < msgpack-short.data'`
    And I run `sh -c 'gzip -d logdir-gzip/*.gz'`
    Then a directory named "logdir-gzip" should exist
    And a file named "logdir-gzip/.current.gz" should exist
    And the number of files for "tmp/aruba/logdir-gzip/*.gz" should be 0.
    And the number of files for "tmp/aruba/logdir-gzip/*" should be 1.
    And every file for "tmp/aruba/logdir-gzip/*" should be a valid msgpack stream
