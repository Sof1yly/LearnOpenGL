# 7.4 Camera Class — Minecraft-Style Build

A small Minecraft-style scene made only of textured cubes: a grass field with a log-and-plank house on it. You fly around it with the `Camera` class (WASD + mouse look + scroll zoom).

## Controls

| Input | Action |
|-------|--------|
| W / A / S / D | Move forward / left / back / right |
| Mouse | Look around |
| Scroll wheel | Zoom (changes field of view) |
| Esc | Quit |

## How the code works

1. **One cube, three looks** — `vertices[]` holds a single cube (position + texture coordinate). Every block in the scene is this same cube drawn with a different model matrix and texture. The texture coordinates pick the right tile out of a 4x3 "cross layout" image, so each face of the cube gets its own part of the picture.
2. **Textures** — `Grass.jpg`, `Wood.jpg` and `WoodPrank.jpg` are loaded with `stb_image`. `GL_NEAREST` filtering keeps the pixelated Minecraft look instead of blurring it.
3. **Building the world** — the blocks are placed by loops that fill position arrays:
   - **Grass** (`grassPositions`): a flat 21x21 field, `x` and `z` from -10 to 10, at `y = -1`.
   - **Logs** (`woodPositions`): for each column `x` from -3 to 3 the height is `h = 3 - abs(x)`, so it is tallest (3) in the middle and drops by one per step to 0 at the edges. A row of 7 logs (`z` 0..6) is placed at that height, making the stepped, triangular roof.
   - **Planks** (`plankPositions`): the front wall (`z = 0`) and back wall (`z = 6`) are filled with planks from the floor up to height `h`, which gives each end wall the same stepped triangle shape. The walls sit half a block inward (`0.5` and `5.5`). The back wall skips `x = 0` for `y < 2`, leaving a 2-block-high doorway.
4. **Drawing** — each frame the code binds one texture, loops over that block type and draws each block with `glDrawArrays(GL_TRIANGLES, 0, 36)` after setting its `model` matrix. The logs are rotated 90° around the x-axis so their round end faces the front.
5. **Camera** — `Camera` (from `learnopengl/camera.h`) keeps the position, direction, yaw and pitch. It starts at `(6, 3, 15)` looking at the house. `camera.GetViewMatrix()` gives the view matrix, and `camera.Zoom` sets the perspective field of view. The mouse callback turns cursor movement into yaw/pitch, and `processInput()` moves the camera with `deltaTime`, so speed doesn't depend on frame rate.
6. **Shaders** — the vertex shader computes `projection * view * model * position` and passes the texture coordinate on. The fragment shader just outputs the texture colour.
7. **Sky** — `glClearColor(0.53, 0.75, 0.95)` gives the light-blue background, and depth testing makes closer blocks hide the ones behind.
