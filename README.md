Pure C/C++ Matrix Multiplication Implementations Plus Test Suite
================================================================

A set of pure ANSI C/C++ matrix multiplication implementations and a test suite to validate and perform benchmarks of each implementation.

All implementations are written in compatible ANSI C/C++ with sensible function names.  Pick the function you like the best and copy the function's source code verbatim into your project to get started, renaming the function however you see fit.  The result won't be the fastest possible code ever since it will be capped to the limitations of an ANSI C/C++ software solution, but each function has been tested to, well, function.

All the implementations here are licensed MIT or LGPL, your choice.  Simple, sensible, functional, and elegant - the way software should be!

[![Donate](https://cubiclesoft.com/res/donate-shield.png)](https://cubiclesoft.com/donate/) [![Discord](https://img.shields.io/discord/777282089980526602?label=chat&logo=discord)](https://cubiclesoft.com/product-support/github/)

Features
--------

* Approximately 18 different opinionated matrix multiplication software implementations.  At least one of them is bound to strike your fancy!
* Includes contiguous memory (double *) and non-contiguous memory (double **) flavors.
* Easy to compile.  Just one ANSI Standards compliant .cpp file.
* Easy to integrate into your software project.  Just copy and paste the function that you like from the .cpp file, rename the function to whatever name you prefer, and carry on with development.
* Has a test suite for performance benchmarking on real hardware.
* A good educational experience.  Optimizing the unoptimizable by utilizing knowledge of the hardware that the software will run on.
* Has a liberal open source license.  MIT or LGPL, your choice.
* Sits on GitHub for all of that pull request and issue tracker goodness to easily submit changes and ideas respectively.

Background
----------

Matrix multiplication from linear algebra is a VERY deep rabbit hole that many a software developer has disappeared into and few have returned unscathed from.  There have been many, many, many dry, boring, mind-numbing papers written about matrix multiplication.  And every StackOverflow post on the topic has at least one developer recommending [this specific BLAS library](https://netlib.org/blas/) (BLAS = Basic Linear Algebra Subprogram) that was written in Fortran and later [lazily converted to C via F2C](https://people.math.sc.edu/Burkardt/cpp_src/cblas/cblas.html) (ew, gross!) with function names [written by a cat walking across a keyboard](https://www.youtube.com/watch?v=6enJ-ufukSA) and takes most people several days to figure out how to compile just to implement this naturally O(N^3) algorithm.  Multithreaded CBLAS is approximately 16 times faster than the fastest single-threaded implementations found here (See the "Additional Ideas" section of this page to learn how to make implementations multithreaded).

Some optimizations, notably Strassen, have reduced O(N^3) to C * O(N^2.8) for marginally better results and some people have even [used artifical intelligence tools to find even more optimal solutions](https://arstechnica.com/information-technology/2022/10/deepmind-breaks-50-year-math-record-using-ai-new-record-falls-a-week-later/) while others have [developed dedicated hardware](https://developer.nvidia.com/cublas) to implement the algorithm.

However, matrix multiplication is roughly O(N^3) and there's not much that can be done about the fact that the time taken launches into the stratosphere for seemingly small values of N.

If so much work has been put into matrix multiplication, why does this repository need to exist?  Software developers shouldn't have to drag in an entire third party library into a project to solve a basic problem if they can avoid it or waste a bunch of time repeating the same work as others.  The included test suite exercises the naive implementation against a variety of progressive enhancements.

Validation
----------

Before using any of the functions or running benchmarks, it is a good idea to make sure that the implementations are valid in your target environment.

The first step of validation is to build the software.  On Windows, run `build.bat` from a Visual C++ Command Prompt (requires Visual Studio to be installed).  On other OSes, run the `build.sh` shell script (requires gcc to be installed).

Next, run these validation tests to exercise various edge cases:

```
matrixmult validate 1
matrixmult validate 2
matrixmult validate 3
matrixmult validate 4
matrixmult validate 32
matrixmult validate 32 12 20
```

The syntax of the validation mode is:

```
matrixmult validate A_rows [A_cols B_cols]
Validates all implementations against the naive implementation.
When A_cols and B_cols are omitted, A_rows is used for all dimensions.
```

Benchmarks
----------

You should run the test suite benchmarks yourself on your own hardware to figure out which implementation(s) are right for you.  Implementation 6 is a general, overall good choice as it balances code length, code readability, and performance with a very stable performance curve.  Just because Implementation 9 for non-contiguous and Implementation 8 for contiguous are generally the fastest on modern hardware do not necessarily make those the best options!

PHP is required to be setup to run the benchmark test suite.  Running it is easy:

```
php bench.php
```

That will generate a series of CSV files.  From there, you can dump the output into Excel, Google Sheets, GNU plot, etc.

Here are a few pretty charts from data generated with `bench.php`.  The benchmarks were run on an Intel Core i7 6th Gen CPU (code compiled with Visual Studio 2017 64-bit):

![Non-Contiguous Benchmark (~10 second threshold)](https://user-images.githubusercontent.com/1432111/208131954-4aa1838d-1874-4e14-9592-57200cf5d495.png)

![Non-Contiguous Benchmark (1000x1000 limit)](https://user-images.githubusercontent.com/1432111/208131960-3fdfce69-96c7-4ec6-a2f6-6a3f51953a72.png)

![Non-Contiguous Benchmark (300x300 limit)](https://user-images.githubusercontent.com/1432111/208131963-bcd47ab1-dc62-4cdc-b395-f84b03e4f132.png)

![Contiguous Benchmark (~10 second threshold)](https://user-images.githubusercontent.com/1432111/208131966-300606a7-289e-486d-915f-d40332ac358e.png)

![Contiguous Benchmark (1000x1000 limit)](https://user-images.githubusercontent.com/1432111/208131969-c286a438-46e5-432b-ad03-e7ba4c44be98.png)

![Contiguous Benchmark (300x300 limit)](https://user-images.githubusercontent.com/1432111/208131971-693e7467-42d7-4cfe-a550-3df663bab961.png)

Your mileage may vary.

The Implementations
-------------------

* Implementation 1 - The classic naive implementation.  Also the slowest.  Good for showing the baseline worst case and validating the other implementations.  Not so great for actual, real world usage.
* Implementation 2 - Uses a temporary variable for C which might end up using a CPU register to do the addition.
* Implementation 3 - Swaps the j and k loops from Implementation 1.  The result is a bit more CPU cache friendly but adds a comparison per loop and the temporary from Implementation 2 is lost.
* Implementation 4 - The temporary variable makes a comeback but this time on one of the operands (matrix A) instead of the assignment.
* Implementation 5 - Move the conditional outside the innermost for loop.  Now we have two inner for-loops.
* Implementation 6 - Remove conditional altogether.  This implementation arguably offers the best balance between code length, code readability, and performance.  That is, both contiguous and non-contiguous functions are short, easy to understand, and faster than the earlier implementations.  It is good enough that the next three Implementations use it as their starting point.
* Implementation 7 - Precalculate base row start for the contiguous memory implementation.  The non-contiguous version remains identical to Implementation 6.  However, the performance gains are negligible over Implementation 6.
* Implementation 8 - Sacrifice a LOT of code readability to use simple pointer math.  The result completely removes all array access multiplication.  Variant of Implementation 6.  The contiguous version performs better than Implementation 9.  The non-contiguous version performs about the same as Implementation 6.
* Implementation 9 - Return to the readable code of Implementation 6 to implement a [blocking algorithm](https://netlib.org/utk/papers/autoblock/node2.html).  Processing small blocks at a time allows larger arrays to stay in the CPU cache during inner loop processing for a small increase in performance at around 1200x1200 array sizes but also results in a wacky performance graph that shows it can actually perform far worse than other Implementations.

Additional Ideas
----------------

* Adjust the MATRIX_STEP parameter of Implementation 9.  Try different step sizes for j and k loops.  Try different step sizes for non-contiguous and contiguous.
* Add multithreading to an implementation.  Start around 8 threads when the total number of operations exceed around 50 milliseconds of execution time.  That works out to about 150 million operations on my computer (`matrixmult bench n 9 1 550` is roughly 50ms).  Then divvy up the outer 'i' loop across each thread.  Starting and stopping 8 threads takes around 5 to 25 milliseconds depending on how busy the system is and what OS is in use.
* Switch to integer multiplication.  Integer math is usually faster than floating point math.  It is easy enough to convert the functions from `double` to an `int64_t` with a simple search and replace.
* Figure out how to use CBLAS.  It's supposedly around 16 times faster than the fastest implementations here but it also uses multithreading to handle larger arrays, which could account for the majority of the performance boost.  The downside is that it can take days to figure out how to build the library, another day to figure out which function to call, and it apparently has to be custom built per architecture (x64, ARM64, etc).  Again, this only flattens the curve just a little bit more before that O(N^3) completion time launches the into the stratosphere.
