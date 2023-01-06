# Matrix multiplication

## Description
This program can [multiply matrices and vectors](https://en.wikipedia.org/wiki/Matrix_multiplication). It is allowed to read matrices from file or standard input. It can generate matrices as well. 

The program supports the following arguments: `-p`, `-f value`, `-g`, `-o`, `--help`

`--help` write the text below.

`-p` enables multithreading. The program will create as many threads as there are cores in your hardware. Without this argument, it uses one thread by default.

`-f value` switches the program to read from a file. `value` must contain the path to the file. 

`-g` generates random matrices.

`-o` prints all matrices.

## Installation
This code use standard C++ 11. You can use `CMakeLists.txt` to build this project.

## Measurements
Matrix size was 2000x2000. Matrices contained values between -1000 and 1000.  
**Hardware**: Intel Core i5 1135G7 (4 cores), CPU clock speed 4.2 GHz  
5 measurements were made in multithreading and in single-threaded mode as well.  
One thread: 26050ms, 24927ms, 23311ms, 24725ms, 25079ms. Mean value is **24818,4ms**  
Multithreading: 12590ms, 12387ms, 12272ms, 12225ms, 12454ms. Mean value is **12385,6ms**  
Therefore multithreading is **2 times faster**.
