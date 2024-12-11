# hello-opengl

> vcpkg is a free and open-source C/C++ package manager maintained by Microsoft and the C++ community.


## Install vcpkg

```
$ sudo apt install build-essential pkg-config cmake curl zip unzip tar
$ sudo apt install libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev

$ git clone https://github.com/microsoft/vcpkg.git
$ cd vcpkg
$ ./bootstrap-vcpkg.sh
$ echo 'export VCPKG_ROOT=$HOME/vcpkg' >> ~/.bashrc
$ echo 'export PATH=$PATH:$VCPKG_ROOT' >> ~/.bashrc
$ source ~/.bashrc
$ vcpkg integrate install
```

## Compiling

```
$ cd hello-opengl
$ cmake -B build --preset vcpkg
$ cmake --build build
$ ./build/hello-opengl
```

```
$ cd hello-triangle
$ cmake -B build --preset vcpkg
$ cmake --build build
$ ./build/hello-triangle
```

## Create new projects

```
$ vcpkg new --application
$ vcpkg add port fmt glfw3 glad
$ vcpkg x-update-baseline --add-initial-baseline
```

C++ dependencies are defined in `vcpkg.json`:

```
{
  "dependencies": [
    "fmt",
    "glfw3",
    "glad"
  ]
}
```
