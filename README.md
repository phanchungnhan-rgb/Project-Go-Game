# Project-Go-Game



# Build

<!--
The game UI uses SFML 3.1.0. Install the SFML 3.1.0 development package for
your compiler and make sure CMake can find its `SFMLConfig.cmake` package file.
-->

The game UI uses SFML 3.1.0. Install the SFML 3.1.0 development package
```sh
cd vendored
git clone https://github.com/SFML/SFML
```


Initialize environment for Visual Studio 2026
```sh
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
```

<!---
cmake -B build -G "MinGW Makefiles"
--> 

Create the Visual Studio 2026 build files and compile
```sh
cmake -B build
cmake --build build
```

# Contributing

Just don't use recursion please

# Resourses

https://github.com/lightvector/katago
https://www.gnu.org/software/gnugo/gnugo_4.html