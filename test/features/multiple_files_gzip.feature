Feature: Create multiple gzipped log files

  Feed msgpack-autosplit a lot of data so that it creates multiple
  gzipped files

  Scenario: start msgpack-autosplit with gzip compression and feed it a lot
  of data.

    Given a directory named "aruba"
    And a file named "tmp/aruba/msgpack-large.data" with large msgpack data
    When I run `sh -c 'msgpack-autosplit -z gzip -s 1000 -d logdir-large-gzip < msgpack-large.data'`
    And I run `sh -c 'gzip -d logdir-large-gzip/*.gz'`    
    Then a directory named "logdir-large-gzip" should exist
    And a file named "logdir-large-gzip/.current.gz" should exist
    And the number of files for "tmp/aruba/logdir-large-gzip/*.gz" should be 0.
    And the number of files for "tmp/aruba/logdir-large-gzip/*" should be 501.
    And every file for "tmp/aruba/logdir-large-gzip/*" should be a valid msgpack stream
