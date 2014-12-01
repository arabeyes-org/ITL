New Method "prayer"
===================

The new method uses a simpler but approximate method from the US
Naval Observatory (USNO, [1]) to compute the Sun Coordinates.
This method (referred to later as "USNO") has a very adequate
accuracy for the purposes of computing sunset and sunrise. The
USNO method is valid for two centuries starting from 2000 (i.e.,
valid until 2199). The goal is to replace the current calculation
method in ITL (which is table-based and cumbersome to understand)
with the USNO-based method.

Roadmap
-------
Here is a "rough" roadmap for implementing the new method completely.
* Implement the basic method (DONE)
* Load the configuration from a separate config file (DONE)
* Add test suite (DONE)
* Handle extreme latitude methods (TBD)

Config files
------------
For the configuration file, we use the old good plain text format.
The file takes the following form:

    key : value
    key : value
    ...

Keys and values are separated by a colon ':'. The pair is ended by
a new line. For examples, see `tests/locations`

Test Suite
----------
To make development easier, the new method comes with an automated
test suite. The testing infrastructure is written in Python.
To run the tests, execute `run_tests.py` located under `tests`

