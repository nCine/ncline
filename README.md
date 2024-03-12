[![Linux](https://github.com/nCine/ncline/workflows/Linux/badge.svg)](https://github.com/nCine/ncline/actions?workflow=Linux)
[![macOS](https://github.com/nCine/ncline/workflows/macOS/badge.svg)](https://github.com/nCine/ncline/actions?workflow=macOS)
[![Windows](https://github.com/nCine/ncline/workflows/Windows/badge.svg)](https://github.com/nCine/ncline/actions?workflow=Windows)
[![MinGW](https://github.com/nCine/ncline/workflows/MinGW/badge.svg)](https://github.com/nCine/ncline/actions?workflow=MinGW)
[![CodeQL](https://github.com/nCine/ncline/workflows/CodeQL/badge.svg)](https://github.com/nCine/ncline/actions?workflow=CodeQL)

# ncline
ncline is the nCine command line tool.  
A tool to automate the download, configuration and compilation processes for the engine and its accompanying projects.  
For additional information: https://ncine.github.io

## Manual

**ncline** is made of five different commands: `set`, `download`, `conf`, `build` and `dist`.

When not using the `set` command you have access to a `-dry-run` option in order to see which commands would be executed on the command line without actually executing them.
This option is very useful to debug an issue or to learn how to perform the actions manually.

Additionally you can invoke **ncline** with the `--help` or `--version` options to respectively print a man page or the version string.

### Set command

The `set` command changes the global settings for the tool. They will affect all other commands and are stored in the `ncline.ini` file in the working directory.

In order to change a setting you have to invoke the tool with the `set` command and an option, for example:

	ncline set -game ncPong

If you want to enable or disable the colored terminal output you can use `-colors` or `-no-colors`.
If left unspecified the default mode would be `-colors`.

#### nCine section

The first option you can set is the target platform. If left uspecified it is assumed to be the current host platform.
You can change it with the `-android` or `-emscripten` option in order to cross-compile for Android or Emscripten.
To revert it to the default use the `-desktop` option.

On Linux and MinGW you can use `-gcc` and `-clang` to specify which compiler to use during the CMake configuration process.
If left unspecified then CMake is left alone to pick a suitable compiler.
On the other supported platforms the compilers are predetermined and can't be changed.

If you want to specify a branch other than `master` or a particular tag when downloading the engine source using Git you can use the `-branch <name>` option:

	ncline set -branch 2020.05

This specific branch or tag will also be used when downloading a game project. This will happen to ensure that the project works with that engine version.

In order to specify which game project will be the target of the remaining commands you can use the `-game <name>` option:

	ncline set -game ncPong

If you specify an official nCine project name you will also be able to download it.

You can specify the directory that will be passed as `nCine_DIR` to game projects using the `-ncine-dir <path>` option:

	ncline set -ncine-dir /path/to/nCine-build

The option is automatically set after each successful compilation or download of the engine.

If you need to specify additional CMake arguments when configuring the engine compilation you can use the `-cmake-args <args>` option:

	ncline set -cmake-args "-D NCINE_BUILD_TESTS=OFF"

If you need to specify additional CMake arguments when configuring the game compilation you can use the `-game-cmake-args <args>` option:

	ncline set -game-cmake-args "-D NCPROJECT_DATA_DIR=/path/to/data-directory"

#### CMake section

With the `-ninja` and `-no-ninja` options you can choose whether to choose Ninja as a CMake generator or not, providing it is available.
This option works together with the `-ninja-exe <executable>` option to specify a particular file as the Ninja executable.
If left unspecified the default would be `-no-ninja`.

On Windows you can pass the `-mingw` and `-no-mingw` options to choose whether to build for MinGW/MSYS or not.
If left unspecified the default would be `-no-mingw`.

On Windows you can also pass the `-vs2017`, `-vs2019`, or `-vs2022` options to build (or download artifacts) for a specific Visual Studio version.
If left unspecified the default would be `-vs2022`.

On macOS you can pass the `-macos12` or `-macos14` options to build (or download artifacts) for a specific macOS version.
If left unspecified the default would be `-macos12`.

If you need to specify a `CMAKE_PREFIX_PATH` when configuring the engine compilation you can use the `-prefix-path <path>` option.
The option is automatically set after each successful compilation or download of the libraries.

#### Android section

When cross-compiling for Android you can specify some options to fine-tune the process.

In order to choose which architecture to support you can pass one of the three options: `-armeabi-v7a`, `-arm64-v8a` or `x86_64`.
If left unspecified the default would be `-arm64-v8a`.

For the cross-compilation to be successful you also need to specify the Android SDK and NDK directories with the `-sdk-dir <path>` and `-ndk-dir <path>` options.

For the game projects to be able to assemble an APK you need to specify the Gradle directory with the `-gradle-dir <path>` option.

#### Executables section

You can specify the path to some important executables used during the configuration and building process like: Git, CMake, Ninja, emcmake and Doxygen.

The relative options are: `-git-exe <executable>`, `-cmake-exe <executable>`, `-ninja-exe <executable>`, `-emcmake-exe <executable>` and `-doxygen-exe <executable>`.

On Windows **ncline** will search for Git and CMake in the installed programs while on macOS it will look for CMake among the installed applications.

### Download command

The `download` command will download the source code from GitHub.

It has three targets: `libs`, `engine` and `game`.

You can invoke it like this:

	ncline download engine

To checkout a specific branch after downloading a source repository you can use the `set -branch` option.

If you set the Android platform with `set -android` the `libs` target will download the building script sources for the Android libraries.

Before using the `game` target you need to set the game name with `set -game <name>` and it has to be one of the official nCine projects.

The `download` command also accepts an `-artifact` option to download a binary archive from the C.I. artifacts repositories instead of sources.

Which artifact is going to be downloaded depends on the host platform and on additional `set` options like: `-desktop|-android|-emscripten`, `-gcc|-clang`, `-mingw|-no-mingw`, `-vs2017|-vs2019`, `-armeabi-v7a|-arm64-v8a|x86_64` or `-branch`.

### Conf command

The `conf` command will run CMake to configure the project using a generator to write the input files for a native build system.

Just like the `download` command it has three targets: `libs`, `engine` and `game`.

It expects the source code repository to be already present, that is why you would usually invoke a `conf` command after a `download` one.

If the generator is a multi-configuration one, like Visual Studio, then you should invoke the command like this:

	ncline conf libs

In all other cases the generator would support only a single build type at configuration time, meaning that you need to invoke it like this:

	ncline conf libs release

There is an exception, when the target is Android you will be able to specify the build type for the Android libraries at configuration time even when using the Visual Studio generator.
This also means you will be able to compile the Android and the native libraries using two different build types.

The `conf` command is affected by many `set` options, like `-desktop|-android|-emscripten`, `-gcc|-clang`, `-mingw|-no-mingw` or `-vs2017|-vs2019`, `-armeabi-v7a|-arm64-v8a|x86_64`, `-ncine-dir <path>`, `-cmake-args <args>`, `-prefix-path <path>` or `-game`.
It will also be affected by the executables section of the settings.

### Build command

The `build` command will run CMake in build mode in order to compile a project.

It supports the same three `conf` targets: `libs`, `engine` and `game`.

It expects a configured build directory to be already present, that is why you would usually invoke a `build` command after a `conf` one.

If the generator is a multi-configuration one, like Visual Studio, this is the time to specify a build type:

	ncline build engine release

In all other cases the build type is already specified in the configured build directory and you only need to invoke it like this:

	ncline build engine

### Dist command

The `dist` command is independent from the `conf` and `build` ones and will try to build a package with CPack.

It supports only two targets: `engine` and `game`.

It expects the source code repository to already be present, that is why you would usually invoke a `dist` command after a `download` one.

It compiles with the release build type and for this reason it is invoked like this:

	ncine dist game

It is only affected by the executables section of the settings and by the `-game` option.
