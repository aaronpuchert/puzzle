PUZZLE - SOLVING ALPHAMETICS
----------------------------

[![Build Status](https://github.com/aaronpuchert/puzzle/actions/workflows/build-test.yml/badge.svg)](https://github.com/aaronpuchert/puzzle/actions/workflows/build-test.yml)

Introduction
------------

This is a program to solve puzzles like the famous

	  SEND
	+ MORE
	------
	 MONEY

where letters have to be replaced by digits injectively,
i.e. different letters by different digits. Obviously
this can be generalized to arbitrary radices instead
of 10. Furthermore there should be far more operations
allowed than "+". This application implements the four
basic operations +, -, * and / and leaves room for more,
if wanted.

Compiling
---------

After downloading the zip or tarball, just switch into the
project directory and execute

	make

or, if you prefer gcc,

	export CXX=g++; make -e

If you want to be really sure that everything went fine,
execute

	make test

Usage
-----

The program is called by

	puzzle [RADIX] PUZZLE

where `RADIX` is an optional radix and `PUZZLE` is an
alphametic expression consisting of numbers and words.
`MONEY` would be fine, so would be `42`. `2DOLLARS` would not.
So the famous example would be solved by

	puzzle SEND+MORE=MONEY

The result is written to `stdout`. Here we get:

	There are 8 different letters.

	D E M N O R S Y
	7 5 1 6 0 8 9 2
	1 solutions found.
