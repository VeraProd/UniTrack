# UniTrack
#### The University Activity Tracker

### About
Project is under active developing now. This section will be updated as soon as the project will leave the prototype stage.

### Lisence
Until the project is not under any specific license,
so that's my license:

- You can use the project for free for any non-commercial purposes
- You can also change the source code, if you want, for non-commercial purposes
- For use in any commercial purposes, please, contact me: `d1021976@gmail.com`
- You can not distribute changed version of this program (as source code or binary or etc.) without reference to original version and it's author

---

### Requirements:
- g++-5 or best (or clang++-3.6 or best, or any other C++11 compiler supporting std::regex, move-semantics and many more, *full support is best*)
- Boost
- MongoDB C Driver (now disabled)
- GNU Make (and a piece of shell) for building this

### Building and Installation
1. Get the sources:
	- `git clone --recursive https://github.com/DmitryKuk/UniTrack`
	- and `cd UniTrack`
2. Build and install:
	- just do `make me happy` (uses `sudo` for installing)
	- or `make` and `sudo make install`
3. Running:
	- `unitrack` (by default, `/usr/bin/unitrack`)
4. Upgrading:
	- `sudo make upgrade` for automatic uninstalling the old version, dowloading updates, building and installing the new one
5. Uninstalling:
	- `sudo make uninstall` for completely uninstalling all libs and executables from your system

### Developing
- The project consists of sub-modules with own Makefiles and (in future) own tests. You can find each of them is `src/module-name` directory.
- Please, build the project only from root directory of the project: root Makefile and Makefile.advanced contains project settings sub-Makefiles don't know about.
- You can try to run the program without installation: `make run` in the root of the project. But this might not work on some systems. If it is so, try `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path-to-project/libs`. Installed version runs normally.
- Some modules has weak dependencies on others. It means that changes in one module *may* have no effect on other module until `make clean` and `make` all project again. Dependencies between files in one module are strong: all module will be rebuilt if you change any header (.h or .hpp), or partially rebuilt if you change .cpp-file.
