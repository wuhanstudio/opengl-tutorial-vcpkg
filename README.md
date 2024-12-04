# hello-opengl

> vcpkg is a free and open-source C/C++ package manager maintained by Microsoft and the C++ community.

```
vcpkg integrate install
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

## Compiling

```
cd hello-opengl
cmake -B build --preset vcpkg
cmake --build build
```
