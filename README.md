# LearnOpenGL — coursework copy

This repository is a clone of the official **Learn OpenGL** code repository from
[https://learnopengl.com](https://learnopengl.com) by **Joey de Vries**.

All tutorial code, shaders, and resources are his work, taken from the website and
its companion repo at
[github.com/JoeyDeVries/LearnOpenGL](https://github.com/JoeyDeVries/LearnOpenGL).
Full credit goes to learnopengl.com. Code samples are licensed **CC BY-NC 4.0**
(see [LICENSE.md](LICENSE.md)).

I am not the author of this material — this copy exists **only for learning** and
for the assignments in my graphics course. Any files I add or change for
coursework live alongside the original samples and are noted below.

## Assignments

### 1. 2D creative coding — `3.2.shaders_interpolation`

First assignment set by my professor: a 2D creative-coding piece built on top of
the **Shaders / interpolation** example.

- Source: [`src/1.getting_started/3.2.shaders_interpolation/shaders_interpolation.cpp`](src/1.getting_started/3.2.shaders_interpolation/shaders_interpolation.cpp)
- Built exe (Debug): `bin/1.getting_started/Debug/1.getting_started__3.2.shaders_interpolation.exe`

## Building (Windows / Visual Studio)

Bundled libs are in `lib/` and DLLs in `dlls/`; the CMake script finds them.

```
cmake -S . -B build -G "Visual Studio 18 2026" -A x64 -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build --config Debug
```

Adjust the generator to your Visual Studio version (`Visual Studio 17 2022`, etc.).
Executables land in `bin/<chapter>/Debug/` — run them from there so they find the
`resources/` and shader files by relative path.

For Linux and macOS build instructions, see the original repo:
[github.com/JoeyDeVries/LearnOpenGL](https://github.com/JoeyDeVries/LearnOpenGL).
