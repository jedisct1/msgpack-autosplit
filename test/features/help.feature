Feature: Show help

  Display the help when the -h switch is given.
  
  Scenario: start with the -h switch

    When I run `msgpack-autosplit -h`
    Then the output should contain:
    """
    Options:
    """
    And the exit status should be 0

  Scenario: start with the --help switch

    When I run `msgpack-autosplit --help`
    Then the output should contain:
    """
    Options:
    """
    And the exit status should be 0

  Scenario: start with no switches

    When I run `msgpack-autosplit`
    Then the output should contain:
    """
    Options:
    """
    And the exit status should be 1

  Scenario: start without mentioning

    When I run `msgpack-autosplit --compress=gzip`
    Then the output should contain:
    """
    Directory not specified
    """
    And the exit status should be 1

  Scenario: start the -V switch

    When I run `msgpack-autosplit -V`
    Then the output should contain:
    """
    msgpack-autosplit
    """
    And the output should not contain:
    """
    Options
    """
    And the output should not contain:
    """
    Directory not specified
    """
    And the exit status should be 0
