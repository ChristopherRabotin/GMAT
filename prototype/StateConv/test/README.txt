This directory contains unit tests for stateconv.

Before running the test suite, you need to add the test framework to your
path:

    >> addpath('matlab_xunit/xunit');

To run all tests:

    >> runtests

To run tests in an individual suite (for example):

    >> runtests testMee

To run an individual test case within a suite (for example):

    >> runtests testMee:testCart2MeeCircularEqPro

To see all runtime options:

    >> help runtests