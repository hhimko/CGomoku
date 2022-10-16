# CGomoku

An opensource game of [Gomoku] made in pure C and [SDL2]. Made as an university assignment for a C course. Enjoy!
    
> TODO: Add an in-game screenshot


## Overview

CGomoku (otherwise known as *Five in a Row*) is a two player strategy game played on a 15x15 grid. 

> TODO: Add Project overview and How-To-Play sections 


## Installation

> TODO: Add latest release build links


## Dependencies

CGomoku uses no more than the `SDL2` library.

### Installing SDL2
  - On **Ubuntu**/**Debian** based distributions run: 
    ```console
    sudo apt-get install libsdl2-dev
    ```

  - On **MacOS** run:
    ```console
    brew install sdl2
    ```
    
   > TODO: Add an installation guide for Windows

  For other OSs please refer to [SDL Wiki's Installation Guide].


## Building & Compiling From Source

If you wish to build the binaries on your own, first make sure you have all [dependencies] installed on your machine.  
CGomoku uses [CMake] as its build system.

- Clone this repo:
  ```console
  git clone https://github.com/hhimko/CGomoku
  cd CGomoku
  ```

- Make a build folder:
  ```console
  mkdir build && cd build
  ```

- Build the project with cmake:
  ```console
  cmake ..
  make
  ```

The executable is called `Gomoku` and should be under `/build/bin`.


## License

Author: Dawid Cyganek [@hhimko]

> Todo: Add license description




[Gomoku]: https://en.wikipedia.org/wiki/Gomoku
[SDL2]: https://www.libsdl.org/
[SDL Wiki's Installation Guide]: https://wiki.libsdl.org/Installation
[CMake]: https://cmake.org/
[dependencies]: #dependencies
[@hhimko]: https://github.com/hhimko
