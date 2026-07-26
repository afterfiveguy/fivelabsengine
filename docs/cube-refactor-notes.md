# Refactoring the cube into a class — annotated notes

Beginner-oriented notes on turning the inline cube code in `App::run()` into a `Cube`
class: what was broken, what each piece of C++/OpenGL syntax means, and why.

> **Note on versions.** Sections 1–8 describe the version of `cube.h` / `cube.cpp`
> written during the refactor. The files have since been replaced with a different
> design (constructor takes `(position, size)` only, `make_mesh(l, w, h)` builds the
> mesh from a `std::vector<float>`, no namespace, texture loaded inline from
> `../img/paper.jpg`). The C++ and OpenGL *concepts* below all still apply, but the
> code quotes won't match what's on disk. See **Section 9** for what currently
> doesn't line up between `cube.*` and `app.cpp`.

---

## 1. What was actually blocking the build

There were four problems, and only one of them was the visible one.

### (a) `loadTexture` wasn't part of the class

The original was:

```cpp
unsigned int loadTexture(const char *path, ...)   // ← free function
```

Without `Cube::` in front, that's a plain standalone function that happens to live in
the `fivelabsengine` namespace. It is *not* the `loadTexture` declared in `cube.h`.
Since it isn't a member of `Cube`, it has no `this` pointer, so `texture` doesn't
exist as far as it's concerned — that's the `use of undeclared identifier 'texture'`
error.

Meanwhile `make_texture()` calls `loadTexture(...)`, and C++ looks **inside the class
before** looking at the namespace. So it finds `Cube::loadTexture` — declared in the
header but with no body anywhere — producing a *linker* error too.

Fix: add the `Cube::` prefix.

### (b) Default arguments can only be written once

The defaults (`= true`, `= GL_REPEAT`, …) appeared in *both* the header and the `.cpp`.
That was legal only by accident, because the `.cpp` function was a different function.
The moment you add `Cube::`, repeating them is a hard error.

**Rule: defaults go in the header declaration only.** The definition is bare:

```cpp
unsigned int Cube::loadTexture(const char *path, bool flipVertically,
                               GLenum wrapS, GLenum wrapT, ...)
```

### (c) Three declared functions had no bodies

`~Cube()`, `update()`, and `draw()` were declared in the header but never defined in
the `.cpp`. The compiler is happy with that — the **linker** is not.

> `undefined symbol` errors always mean: *you promised a function exists and never
> wrote it.*

### (d) `size` was thrown away

The constructor took `glm::vec3 size` and never used it, because there was no `size`
member to store it in.

---

## 2. Reading the constructor's initializer-list syntax

Before:

```cpp
Cube::Cube(glm::vec3 position, glm::vec3 size) {
  this->position = position;      // default-construct, then overwrite
}
```

After:

```cpp
Cube::Cube(glm::vec3 position, glm::vec3 size, glm::vec3 eulers)
    : position{position}, size{size}, eulers{eulers},
      VAO{0}, VBO{0}, EBO{0}, texture{0}
{ ... }
```

The part after the `:` is a **member initializer list**. It initializes members
*directly* instead of default-building them and then assigning over the top. Same
result for `glm::vec3`, but it's the idiomatic form and it's the *only* way to
initialize `const` members or references. `window.cpp` already uses this style.

The `VAO{0}, VBO{0}, ...` part matters for a concrete reason: an uninitialized
`unsigned int` in C++ holds **garbage**, and handing garbage to `glDeleteBuffers` is a
bug. Zeroing them makes the object safe to destroy even if construction fails partway.

`glm::vec3 eulers = glm::vec3(0.0f)` in the header is a **default argument** — the
`= ...` makes the parameter optional, so `Cube(pos, size)` still compiles. It exists so
different cubes can start at different angles.

---

## 3. The destructor, and the copy/move rules

This is the part that looks like the most "extra stuff." Here's why it's needed.

A `Cube` owns **OpenGL handles** (`VAO`, `VBO`, `EBO`, `texture`). These are just
integers — IDs referring to memory the graphics driver holds on your behalf. Someone
must hand them back:

```cpp
Cube::~Cube() { release(); }   // calls glDeleteVertexArrays / glDeleteBuffers / glDeleteTextures
```

### The danger

C++ generates a copy constructor for you automatically, and it copies members **bit for
bit**:

```cpp
Cube a(pos, size);
Cube b = a;          // b.VAO == a.VAO — the same integer!
```

Both destructors run. Both call `glDeleteVertexArrays` on the same ID. That's a
**double free** — the second delete acts on an ID the driver may have already recycled
for something else. Silent, and painful to debug.

### The fix: forbid copying

```cpp
Cube(const Cube &) = delete;
Cube &operator=(const Cube &) = delete;
```

`= delete` means *"this function does not exist."* Try to copy a `Cube` and you get a
clear compile error instead of a runtime mystery. `Window` and `Shader` in this repo
already do exactly this.

### But `std::vector` needs to relocate elements

So it needs *some* way to shuffle a `Cube` around. That's what **moving** is: transfer
the handles, then blank out the original so its destructor becomes harmless.

```cpp
Cube::Cube(Cube &&o) noexcept : VAO{o.VAO}, ... {
  o.VAO = o.VBO = o.EBO = o.texture = 0;   // ← the crucial line
}
```

`Cube &&` (double ampersand) means *"an object that's about to be discarded, so it's
fine to gut it."* After the move, `o` is still destroyed, but its handles are `0`, and
`glDelete*(0)` is defined as a **no-op**. Nothing gets double-freed.

Move-assignment (`operator=`) needs one extra guard:

```cpp
if (this != &o) { ... }
```

Without it, `x = std::move(x)` would `release()` the handles and then copy the
now-zeroed values onto itself, silently destroying the object.

### The general principle

This is the **rule of five**: if you write a destructor that frees something, you must
also decide what copy and move mean for your type. Ignoring them is exactly what
produces double frees.

---

## 4. `update` and `draw`

### Delta time

```cpp
void Cube::update(float dt) {
  eulers += glm::vec3(1.0f, 0.3f, 0.5f) * 25.0f * dt;
  eulers = glm::mod(eulers, glm::vec3(360.0f));
}
```

`dt` is **delta time** — seconds elapsed since the last frame. Multiplying by `dt`
means rotation speed is expressed in degrees *per second*, so cubes spin at the same
visible rate on a 60 Hz laptop and a 165 Hz monitor. Using `glfwGetTime()` directly
also works, but it can't be paused, reversed, or varied per object.

`glm::mod` wraps angles back into 0–360 so they don't grow without bound. Not strictly
necessary at these speeds, but a float left climbing for hours loses precision.

### Transform order

```cpp
glm::mat4 model = glm::mat4(1.0f);
model = glm::translate(model, position);
model = glm::rotate(model, glm::radians(eulers.x), {1,0,0});   // then y, then z
model = glm::scale(model, size);
```

`glm::mat4(1.0f)` is the **identity matrix** — GLM's constructor fills the diagonal
with the value you pass, and a 1-diagonal matrix means "no transform."

Note the order: **translate, then rotate, then scale.** Matrix multiplication is not
commutative, and because each call multiplies on the *right*, the transform written
**last** is applied to the vertex **first**. So a vertex is scaled, then rotated, then
moved — which is what you want.

> Swap translate and rotate and your cubes **orbit the origin** instead of spinning in
> place. This is the single most common source of "why is my object flying around."

### Euler angles vs. a single arbitrary axis

The original code rotated once around the arbitrary axis `(1.0, 0.3, 0.5)`. Storing
*euler angles* instead means three independent angles, so it becomes three rotations
around X, Y, and Z. Visually similar, slightly different tumble.

### The draw call

```cpp
glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES, 0, 36);
```

**36 = 6 faces × 6 vertices.** Each face is 2 triangles, and in this vertex layout the
two triangles don't share vertices, so it's 6 per face rather than 4.

`draw(unsigned int shader)` takes the raw program ID rather than a `Shader&`, which
keeps `Cube` from needing to know about the `Shader` class. The shader must already be
`use()`d (or `draw` must call `glUseProgram` itself).

---

## 5. Why the EBO was dead weight

The original created an `EBO` and filled it with 6 indices:

```cpp
unsigned int indices[] = { 0,1,3, 1,2,3 };
```

Those are leftovers from the single-textured-quad exercise:

- An EBO only does anything if you call **`glDrawElements`**, and this code calls
  **`glDrawArrays`**.
- Even if you switched, 6 indices only describe **one face**, not a cube.

Safe to delete `EBO`, `indices`, and the two lines that fill it. (The current
`cube.cpp` has already dropped it — correct call.)

---

## 6. What changed in `app.cpp`

The vertex array, VBO/EBO setup, attribute pointers, container-texture load, and the
`glDelete*` cleanup all moved into `Cube`. What replaced them:

```cpp
std::vector<Cube> cubes;
cubes.reserve(std::size(cubePositions));
for (unsigned int i = 0; i < std::size(cubePositions); i++)
  cubes.emplace_back(cubePositions[i], glm::vec3(1.0f), glm::vec3(20.0f * i));
```

- **`reserve`** allocates room up front so the vector never grows and relocates. Not
  required (moves handle that correctly), it just avoids pointless work.
- **`emplace_back`** constructs the `Cube` *directly inside* the vector, forwarding your
  arguments to the constructor. `push_back(Cube(...))` would build a temporary and then
  move it; `emplace_back` skips that step. It's also the natural fit for a
  non-copyable type.
- `glm::vec3(20.0f * i)` gives each cube a different starting angle, preserving the
  staggered look the old `angle = 20.0f * i` produced.

### A real bug that got fixed

The old loop was:

```cpp
for (unsigned int i = 0; i < 10; i++)   // ← 10
    ... cubePositions[i] ...
```

but `cubePositions` has only **9** entries. Iteration `i == 9` read **past the end of
the array** — undefined behaviour. It appeared to "work" because it read adjacent stack
memory and produced a cube at a nonsense position. `std::size(cubePositions)` asks the
array for its real length, so this class of bug can't come back.

> `std::size` needs `#include <iterator>`.

### The render loop

```cpp
for (Cube &cube : cubes) {
  cube.update(dt);
  cube.draw(ourShader.ID);
}
```

The `&` in `Cube &cube` is essential — it makes `cube` a **reference** to the real
element, so `update` mutates the stored cube. Without the `&` you'd get a copy of each
cube every frame, and the rotation would never persist.

With the copy constructor deleted, forgetting the `&` becomes a **compile error**
instead of a silent no-op. That's the deleted-copy safety net doing its job.

### Smaller cleanups

- `glEnable(GL_DEPTH_TEST)` moved out of the loop — it's sticky state, so setting it
  60×/sec accomplishes nothing.
- Dropped a duplicate `glClearColor`.
- `texture2` (the smiley) stayed in `app.cpp` on unit 1, because `simple.fs` `mix`es
  two textures and the second one is shared by every cube.

---

## 7. The build file, and header guards

### `CMakeLists.txt`

`src/cube.cpp` **must** be added to `add_executable`. A `.cpp` that isn't listed there
never gets compiled, so every `Cube::` function would be an `undefined symbol` at link
time no matter how correct the code is.

`src/cube.h` is listed too. Headers don't get compiled — they're listed so clangd (and
your IDE) resolve them with the right compile flags.

### `#pragma once`

Headers get **textually pasted** into every file that includes them. Without a guard,
two includes means the compiler sees `class Cube` twice and rejects the file. You don't
notice while only one `.cpp` includes it — you notice the moment a second one does.

Every header in this repo should start with `#pragma once`. `config.h`, `window.h`, and
`app.h` do.

---

## 8. A design problem worth knowing about

With this design, each `Cube` builds its **own** VBO and loads its **own** copy of the
texture. Nine cubes means nine identical vertex buffers and nine identical textures on
the GPU, plus nine JPEG decodes at startup. The old inline code uploaded each exactly
once.

That's the honest cost of "every cube owns its mesh." It's fine at 9 cubes, but it
doesn't scale.

**The standard fix** is to separate what's *shared* from what's *per-object*: a `Mesh`
and a `Texture` created once, with each `Cube` holding a pointer or reference to them
plus its own `position` / `eulers` / `size`. That's the next refactor, not this one.

Two smaller things left alone:

- `glGetUniformLocation` runs every frame for every cube. It's a string lookup —
  cheap, but cacheable.
- `loadTexture` exists in both `cube.cpp` and `app.cpp`. Pulling it into a shared
  `texture.h` / `texture.cpp` would be the clean fix.

---

## 9. Current mismatches between `cube.*` and `app.cpp`

The `cube.h` / `cube.cpp` now on disk use a different design from the one `app.cpp` was
adapted to. These are the specific things that don't line up yet:

### Will not compile

1. **Constructor arity.** `cube.h` declares `Cube(glm::vec3 position, glm::vec3 size)`
   — two parameters. `app.cpp` calls
   `cubes.emplace_back(cubePositions[i], glm::vec3(1.0f), glm::vec3(20.0f * i))` with
   **three**. Either add a third `glm::vec3 eulers = glm::vec3(0.0f)` parameter, or
   drop the third argument at the call site.

2. **`std::vector` without `<vector>`.** `cube.cpp` uses `std::vector<float> vertices`,
   but `cube.h` only includes `config.h`, and `config.h` doesn't include `<vector>`. It
   may compile by accident via `<iostream>`, but that's not guaranteed — add
   `#include <vector>` to `cube.cpp`.

3. **No `#pragma once` in `cube.h`.** Fine right now (only `cube.cpp` and `app.cpp`
   include it, once each), but it will break the first time a header includes it. Add
   it.

### Will compile, but won't look right

4. **Attribute location mismatch.** `cube.cpp` sends texture coordinates to attribute
   **1**:
   ```cpp
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (void *)12);
   glEnableVertexAttribArray(1);
   ```
   but `simple.vs` declares `layout (location = 2) in vec2 aTexCoord`. Location 1 is
   never read; location 2 is never written. Texture coordinates will arrive as zeros,
   so every face samples a single pixel. Change either the `1`s to `2`s or the shader
   to `location = 1`.

5. **Texture path doesn't exist.** `make_texture()` loads `"../img/paper.jpg"`. This
   repo's textures are at `./src/resources/textures/` (`container.jpg`,
   `awesomeface.png`), and paths are relative to where you *run* the binary, not where
   the source lives. `stbi_load` will return `nullptr`, and `data` is passed to
   `glTexImage2D` without a null check.

6. **Two textures expected, one provided.** `simple.fs` does
   `mix(texture(texture1, ...), texture(texture2, ...), mixValue)`. `Cube::draw` calls
   `glBindTexture` without `glActiveTexture(GL_TEXTURE0)` first, so it binds to
   whichever unit was last active — and `app.cpp` leaves unit **1** active from
   binding `texture2`. Add an explicit `glActiveTexture(GL_TEXTURE0)` before the bind
   in `draw`.

7. **Cube is twice the intended size.** `make_mesh(l, w, h)` uses vertices spanning
   `-l` to `+l`, so passing `size = glm::vec3(1.0f)` produces a 2×2×2 cube. Either
   halve the inputs or use `l * 0.5f` in the vertex data.

### Fragile, not yet broken

8. **No copy/move handling.** `~Cube()` deletes GL handles, but copy is not deleted
   (see Section 3). It happens to be safe today because `reserve` prevents the vector
   from reallocating and `emplace_back` constructs in place — so no copy is ever made.
   Remove the `reserve`, or use `push_back`, and you get a double free. Adding
   `Cube(const Cube&) = delete;` turns that latent bug into a compile error.

9. **`draw` calls `glUseProgram(shader)` every cube, every frame.** Harmless, just
   redundant — `app.cpp` already calls `ourShader.use()` once.