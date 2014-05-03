PUZZLE - SOLVING ALPHAMETICS
----------------------------

[![Build Status](https://travis-ci.org/aaronpuchert/puzzle.svg?branch=master)](https://travis-ci.org/aaronpuchert/puzzle)

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

Given the number of files the project consists of, it
seems to be unnecessary to provide a Makefile. As of
today, nothing but the most basic operations are used,
thus there should be no problems with compiling on
your favorite exotic platform. (a rather new compiler
provided - we rely on templates here, for example)

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
