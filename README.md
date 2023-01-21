# CGomoku

An opensource game of [Gomoku] made in pure C and [SDL2]. Made as an university assignment for a C course. Enjoy!
    
> TODO: Add an in-game screenshot


## Overview

CGomoku (otherwise known as *Five in a Row*) is a two player strategy game played on a 15x15 grid. 

> TODO: Add Project overview and How-To-Play sections 


## Installation

> TODO: Add latest release build links

## Building & Compiling From Source

CGomoku uses [CMake] (ver. 3.12+) as its build system.

- Clone this repo:
  ```console
  git clone https://github.com/hhimko/CGomoku
  cd CGomoku
  ```

- Install all the [dependencies].

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


## Dependencies

CGomoku uses no more than the `SDL2` library, along with its extensions: `SDL2 Mixer`.

### Installing SDL2
  - Under **Ubuntu**/**Debian** based distributions run: 
    ```console
    sudo apt-get install libsdl2-dev libsdl2-mixer-dev
    ```

  - Under **MacOS** run:
    ```console
    brew install sdl2 sdl2_mixer
    ```
    
  - Under **Windows**:
    1. Download the latest release of [SDL2-devel] and [SDL2-mixer] for either VC or MinGW.
    2. Extract the downloaded files to `CGomoku/include`.
    3. Change the extracted directory names from `SDL2-x.x.x` to `SDL2` and `SDL2_mixer-x.x.x` to `SDL2_mixer`.

  For other OSs please refer to [SDL Wiki's Installation Guide].


## License

Author: Dawid Cyganek [@hhimko]

> Todo: Add license description




[Gomoku]: https://en.wikipedia.org/wiki/Gomoku
[SDL2]: https://www.libsdl.org/
[SDL Wiki's Installation Guide]: https://wiki.libsdl.org/Installation
[CMake]: https://cmake.org/
[SDL2-mixer]: https://github.com/libsdl-org/SDL_mixer/releases
[SDL2-devel]: https://github.com/libsdl-org/SDL/releases
[dependencies]: #dependencies
[@hhimko]: https://github.com/hhimko
