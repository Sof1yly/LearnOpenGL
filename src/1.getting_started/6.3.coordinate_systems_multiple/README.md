# 6.3 Coordinate Systems (Multiple) — Pyramid of Boxes

Draws a stepped pyramid of textured cubes and lets you fly around it with the keyboard.

## What you see

A pyramid built from 5 layers of cubes: the bottom layer is 5x5 cubes, then 4x4, 3x3, 2x2 and 1x1 on top. Every cube uses the same two textures (`container.jpg` mixed 20% with `awesomeface.png`).

## Controls

| Key | Action |
|-----|--------|
| W / S | Move camera forward / backward |
| A / D | Move camera left / right |
| Esc | Quit |

## How the code works

1. **Setup** — GLFW creates the window and OpenGL 3.3 core context, GLAD loads the GL functions, and `glEnable(GL_DEPTH_TEST)` makes nearer cubes hide farther ones.
2. **One cube, reused** — `vertices[]` holds a single cube (36 vertices, each with a position and a texture coordinate) uploaded once into a VBO/VAO. Every box on screen is this same cube drawn with a different model matrix.
3. **Pyramid layout** — the nested loops fill `cubePositions[]`. For each `row` (layer), `count = 5 - row` is the layer width. Each cube's x and z are offset by `(count - 1) / 2` so every layer is centred over the one below, and `y` goes up one unit per row. A final shift (`+1, -1, -8`) pushes the whole pyramid in front of the camera.
4. **Textures** — two textures are loaded with `stb_image` and bound to texture units 0 and 1. The fragment shader blends them: `mix(texture1, texture2, 0.2)`.
5. **The three matrices** (`6.3.coordinate_systems.vs` computes `projection * view * model * position`):
   - **model** — per cube, `translate` to its position from `cubePositions[]`.
   - **view** — the camera, built with `glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)`.
   - **projection** — `glm::perspective` with a 45° field of view, so distant cubes look smaller.
6. **Camera movement** — `processInput()` changes `cameraPos` each frame. The step is `2.5 * deltaTime`, so speed is the same on fast and slow machines. W/S move along `cameraFront`; A/D move along the vector `cross(cameraFront, cameraUp)`, which points to the camera's right. The camera always keeps looking in the same direction (there is no mouse look).
7. **Render loop** — each frame: update `deltaTime`, read input, clear the colour and depth buffers, set the view and projection, then loop over every cube, set its model matrix and call `glDrawArrays(GL_TRIANGLES, 0, 36)`.
