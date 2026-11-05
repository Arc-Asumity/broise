# Broise UI Runtime — Architecture

**Document status:** Initial architecture assessment and target direction  
**Repository reviewed:** `Arc-Asumity/broise`, default branch `main`  
**Review date:** 2026-11-05  
**Language target:** C++23  

> This document separates the code that was observable in the reviewed repository snapshot from the architecture Broise is intended to grow into. It is not a claim that every proposed component already exists. The local working copy may contain changes made after the reviewed `main` snapshot; inspect it before editing.

## 1. Architectural summary

Broise should be a reusable desktop UI Runtime with three major responsibilities:

1. **Desktop and graphics foundation:** application lifecycle, native window/context, event loop, and rendering.
2. **UI composition foundation:** geometry, widgets, containers, input dispatch, pages/workspaces, and shared styling.
3. **Consumer integration:** a stable public C++ API and CMake target through which applications and feature modules provide their own widgets.

The most important dependency rule is:

**Consumer applications and feature modules depend on Broise. Broise must never depend on concrete consumer applications or their widgets.**

A page is a workspace composition surface. It may contain several widgets from one feature area or combine widgets from different feature modules (for example, a Proof Checker editor widget beside an Email status widget). The Runtime provides the common widget, layout, event, rendering, and theme mechanisms that make this composition possible.

### Terminology clarification

- A **Part** is a Runtime-provided UI building block, such as a container, button, text element, or image. Parts compose into Part Trees; the Runtime provides their management, composition, update, and render infrastructure.
- **RootPart** is the conceptual root of a Part Tree. It is not a software-level Widget, and whether it needs a distinct concrete class remains open.
- A **Widget** is an application- or feature-provided functional software component. It may use multiple Parts to build its UI, but it is not a Part or a Part Tree node.
- The Runtime provides Parts and remains independent of application-specific Widgets. The software-level Widget interface, lifecycle, manager, and ownership system have not been implemented and should not be designed prematurely. Workspace and Page architecture remain future design work.
- The historical source snapshot used `IWidget` and `MainWidget` for the UI-level Part concept. The current source is being aligned to `IPart` and `RootPart` terminology.

## 2. Observed repository snapshot

The reviewed `main` branch is a small early skeleton. The repository currently has the following shape:

```text
broise/
├── CMakeLists.txt
├── cmake/
│   └── Deps.cmake
├── include/
│   └── self/
│       ├── core/
│       │   ├── Application.hpp
│       │   ├── Base.hpp
│       │   ├── IWidget.hpp
│       │   ├── Rect.hpp
│       │   ├── Renderer.hpp
│       │   └── Window.hpp
│       └── widgets/
│           └── MainWidget.hpp
└── src/
    ├── CMakeLists.txt
    ├── Main.cpp
    ├── core/
    │   ├── CMakeLists.txt
    │   ├── Application.cpp
    │   ├── Rect.cpp
    │   ├── Renderer.cpp
    │   └── Window.cpp
    └── widgets/
        ├── CMakeLists.txt
        └── MainWidget.cpp
```

### 2.1 Build and dependencies currently present

- The root CMake file sets `CMAKE_CXX_STANDARD` to 23 and declares project `Broise`.
- `cmake/Deps.cmake` fetches GLFW 3.4 and GLAD 2.0.8, generates a GL 4.6 core loader, and requires OpenGL.
- The root build adds `src/`.
- `src/CMakeLists.txt` declares an executable target named `Broise`, then adds the `core` and `widgets` subdirectories.
- The source currently declares CMake library targets `Broise::Core` and `Broise::Widgets`.

The current target layout is not yet a clean reusable-library design. The executable is the only obvious application entry point, while the library boundaries are incomplete and cyclic (see Section 3). In addition, `src/CMakeLists.txt` links the executable to `Broise::Core` before `add_subdirectory(core)` defines that alias, and `src/core/CMakeLists.txt` refers to `Broise::Widgets` before `add_subdirectory(widgets)` defines that alias. Depending on the active CMake policy/version, these forward references may be treated as missing `::` targets and fail configuration/generation; define targets before referring to them or restructure the subdirectory order.

### 2.2 Existing source responsibilities

| Existing type/file | What the current code shows | Current limitation |
|---|---|---|
| `Application` | Creates/owns a `Window`, owns a `MainWidget` root and a `Renderer`, and has a main render loop. | It is coupled to a concrete `MainWidget`; the root render call does not match the current widget interface; `Update()` is not called in the loop. |
| `Window` | Wraps `GLFWwindow`, initializes GLFW, creates a GL 4.6 context, loads GLAD, tracks framebuffer width/height, polls events, swaps buffers, and handles close state. | GLFW lifetime is managed per window; multi-window/global initialization policy is not defined. GLFW types leak through the public header. |
| `Renderer` | Owns an OpenGL shader program, VAO, and VBO; compiles a simple shader and draws a point polygon with a solid `Color`. It is move-only. | It is a very small low-level renderer, not yet a complete UI renderer; alpha blending, clipping, text, batching, and a broader drawing API are absent. |
| `Point`, `PointSet`, `Color` | Basic geometry/color data in `Base.hpp`. | Coordinate semantics and color/alpha conventions are not documented. |
| `Rect` | Rectangle value type with horizontal/vertical splitting, relative translation, and rectangle composition. | It is not yet integrated into a complete widget layout system; coordinate units need to be specified. |
| `IWidget` | Abstract interface with `Render()` and `Update()`. | No bounds, parent/child model, visibility, enabled state, input, or renderer argument/context is defined. |
| `MainWidget` | Declared as an `IWidget` implementation. | Its constructor, `Render()`, and `Update()` are declared but not implemented in the current `.cpp`. |
| `Main.cpp` | Contains the program entry point. | It currently returns immediately and does not create an `Application`. |

### 2.3 What does not exist yet

The reviewed snapshot does not yet demonstrate a working widget tree, container ownership, general widget factories, a page manager, workspace switching, input routing, focus management, a shared theme system, a reusable consumer-facing Runtime target, or a complete demo application.

These should be treated as planned capabilities, not current functionality.

## 3. Concrete issues to address before expanding features

The following issues are visible in the reviewed source. Re-check the local checkout before changing them.

### 3.1 Circular CMake dependency

- `src/core/CMakeLists.txt` links `Broise_Core` publicly to `Broise::Widgets`.
- `src/widgets/CMakeLists.txt` links `Broise_Widgets` publicly to `Broise::Core`.

This creates a Core ↔ Widgets dependency cycle. The intended direction should be reversed: generic Runtime/core code defines the widget API and infrastructure; feature/demo widgets depend on that API. Core must not link back to a concrete widget library.

### 3.2 Widget render interface mismatch

- `IWidget.hpp` declares `virtual void Render() = 0`.
- `MainWidget.hpp` overrides `Render()` with no arguments.
- `Application.cpp` calls `root_.Render(render_)`.

These declarations and the call do not agree. Choose one intentional rendering contract and apply it consistently. For the first version, passing the Runtime renderer (or a small rendering context) into widget rendering is a straightforward option; do not introduce a large command-buffer abstraction without need.

### 3.3 Incomplete MainWidget implementation

`MainWidget.cpp` currently only includes its header. The declared constructor and virtual methods have no definitions in the reviewed snapshot. The application currently also hard-codes `MainWidget` as its root type, which conflicts with the goal of a generic Runtime root/container.

### 3.4 Application lifecycle is incomplete

`Application::Run()` polls window events, clears the framebuffer, calls the root renderer, and swaps buffers. It does not call the widget update lifecycle, route input, or dispatch Runtime events. `Main.cpp` does not construct or run the application.

### 3.5 Other source-level concerns to verify

- `Base.hpp` defines a stream insertion overload for `std::vector<Point>` that streams `points` back into the same overload. That appears recursively self-calling and should be corrected or removed before use.
- `Rect.hpp` uses `std::array` and `std::pair` but does not directly include the corresponding standard headers in the reviewed snapshot. Headers should include the standard declarations they use.
- `Renderer.cpp` writes to `std::cerr`; verify that it includes `<iostream>` directly rather than relying on transitive includes.
- The Renderer releases GL objects in its destructor. The Window/context must still be alive and current at that time. Keep this lifetime rule explicit when changing Application member order or ownership.
- `Window::Create()` performs GLFW initialization and each Window destructor terminates GLFW. That may be acceptable for a single-window prototype, but it is not yet a sound shared lifetime policy for multiple windows.
- `Window.hpp` includes GLAD and GLFW in a public header. Consider reducing graphics/platform leakage as the API stabilizes, but avoid a broad header refactor before it is needed.

No build was performed as part of this repository review; the items above are source-level observations, not a report from a compiler run.

## 4. Target logical architecture

The recommended target is a small set of cooperating layers. Keep these as conceptual boundaries first; split them into more CMake libraries only when that improves build or API boundaries.

```text
Consumer Application
│
├── Feature Module: Proof Checker
│   └── Proof Checker widgets
│
├── Feature Module: Email
│   └── Email widgets
│
└── Broise Runtime
    ├── Application lifecycle
    ├── Window / platform integration (initially GLFW)
    ├── Event dispatch and input routing
    ├── Page / Workspace manager
    │   └── Page
    │       └── Widget composition tree
    │           ├── Proof Checker editor widget
    │           └── Email status widget
    ├── Geometry and layout
    ├── Shared theme / style values
    └── Renderer (initially OpenGL 4.6)
```

### 4.1 Application

`Application` is the top-level Runtime coordinator. It should:

- Initialize the platform/window and renderer in a defined order.
- Own or coordinate the page/workspace manager.
- Run the main loop.
- Process native events and dispatch Runtime events.
- Update the active page/widget tree.
- Render the active page/widget tree.
- Present the frame.
- Tear down UI objects and graphics resources before destroying the graphics context.

`Application` should depend on generic Runtime abstractions, not on `MainWidget` or any application-specific feature class.

### 4.2 Window and platform layer

`Window` is responsible for native desktop window behavior and OpenGL context creation. GLFW is the initial implementation detail. Window events should be adapted into Runtime-level events so generic widgets do not need to know about GLFW.

A future multiple-window capability should be possible, but it is not necessary to implement a general multi-window manager in the first milestone. GLFW global initialization/termination should nevertheless have one well-defined owner/lifetime so that the design does not accidentally become unsafe when a second window is added.

### 4.3 Renderer

The Renderer owns graphics-backend resources and exposes the drawing operations needed by the UI layer. The initial implementation is OpenGL 4.6.

Recommended initial responsibilities:

- Initialize and own shader/program/buffer/vertex-array resources.
- Convert Runtime UI coordinates to backend coordinates.
- Draw simple colored geometry.
- Enable and correctly configure alpha blending for translucent UI elements.
- Respect the current window/framebuffer dimensions.

Widgets should describe what they need drawn through the Runtime renderer API. They should not create their own OpenGL programs or manipulate GLFW/OpenGL global state directly.

Do not design a fully generic renderer plugin architecture yet. Keep OpenGL-specific implementation behind the current Renderer boundary; introduce a backend interface only when a real second backend or testing requirement justifies it.

### 4.4 Widget model

The Runtime needs a generic widget abstraction and one or more container implementations.

A widget should have a documented lifecycle and at least:

- Bounds/geometry.
- Visibility and enabled state.
- Update behavior.
- Render behavior through the Runtime renderer/context.
- Optional event-handling hooks as the input system is introduced.

A container owns its child widgets and controls their update/render order. Child-to-parent links and lookup results are non-owning. Do not make a feature widget a special case in Runtime core.

For fixed, known members, prefer direct value ownership. For a heterogeneous child collection of polymorphic widgets, select a safe owning representation and keep it hidden behind the container API. Never use raw owning pointers. Avoid introducing `std::unique_ptr` mechanically, but explain and justify the ownership mechanism when dynamic polymorphism makes value ownership unsuitable.

The current `IWidget` can be evolved or replaced as part of a deliberate small design decision. Do not preserve its current signature merely for compatibility if it blocks the required lifecycle; equally, do not redesign the entire API without first writing down the intended contract.

### 4.5 Page and workspace model

A Page is a persistent UI workspace containing a widget composition tree. It is not necessarily owned by one feature module or tied to one application domain.

The Page/Workspace manager should:

- Own or otherwise clearly manage the lifetime of pages.
- Track the active page.
- Support switching between pages.
- Preserve inactive pages and their widget state.
- Provide controlled creation and closing of pages.

A page may contain widgets supplied by different feature modules. For example, the same page may contain a Proof Checker editor and an Email status panel. The Runtime only knows their common widget interfaces; it does not link to either module.

Start with normal in-process C++ construction/registration. Do not implement dynamic library loading, a binary plugin ABI, a cross-language C ABI, or a package system as a prerequisite for pages. If plugin support later becomes necessary, define its lifecycle and compatibility requirements separately.

### 4.6 Input and event dispatch

Recommended event flow:

```text
GLFW/native event
    ↓
Window/platform event adapter
    ↓
Runtime event representation
    ↓
Application / input dispatcher
    ↓
Active Page
    ↓
Widget hit-testing and focus routing
    ↓
Target widget and optional parent propagation
```

At minimum, plan for pointer motion, pointer buttons, keyboard input, hit testing, and focus. Hit testing should account for widget visibility, bounds, parent clipping if clipping is implemented, and z-order/drawing order.

Do not let every widget install a GLFW callback. Native events should enter the Runtime through one controlled route.

### 4.7 Geometry and layout

Keep `Point`, `Color`, and `Rect` as lightweight value types with no dependency on GLFW or OpenGL.

The current `Rect` helpers support splitting a rectangle into weighted regions and translating relative coordinates. Preserve useful behavior, but clearly document units and edge cases (zero/negative weights, normalized versus logical units).

Recommended direction:

- Use logical window coordinates for normal widget layout and pointer hit testing.
- Convert to framebuffer pixels/NDC only at the renderer/platform boundary.
- Keep relative subdivision utilities available for simple layouts.
- Add more complex layout policies only when actual widgets need them.

This is a recommendation to establish a consistent contract; the current repository does not yet define a complete coordinate system.

### 4.8 Shared theme and style system

A shared Theme provides the default visual tokens used by Runtime widgets and feature widgets. Start with values, not a large styling language:

- Neutral grey surface/background palette.
- Foreground and muted text colors.
- Alpha/opacity values.
- Border and separator colors.
- Spacing and sizing constants.
- Corner radius and focus/hover/disabled state values.
- Typography configuration when text rendering is added.

The default visual identity should be coherent across different modules. A module may expose its own content and behavior, but should consume shared tokens for common surfaces and controls. Platform-level window translucency/blur is optional and must have a legible fallback.

## 5. Main-loop and destruction order

Recommended frame lifecycle:

```text
Initialize platform
    ↓
Create Window and graphics context
    ↓
Initialize Renderer
    ↓
Create Page/Workspace manager and initial page
    ↓
while the application is running:
    1. Poll native events
    2. Translate and dispatch Runtime events
    3. Update the active page/widget tree
    4. Begin frame / clear framebuffer
    5. Render the active page/widget tree
    6. Swap/present buffers
    ↓
Destroy pages and widgets
    ↓
Destroy Renderer resources while the GL context is current
    ↓
Destroy Window and graphics context
    ↓
Terminate platform runtime
```

C++ member declaration order matters because members are destroyed in reverse declaration order. If `Application` directly owns these objects, declare/structure them so that pages/widgets are destroyed first, the Renderer next, and the Window/context last. Do not rely on accidental destruction order.

## 6. Dependency direction and CMake design

The current Core ↔ Widgets cycle must be removed.

The essential rule is:

- Core/Runtime defines widget contracts and generic containers.
- Consumer or demo widgets depend on Runtime.
- Runtime does not depend on consumer or demo widget implementations.

A possible first clean CMake arrangement is:

```text
Broise::Runtime  (reusable library target)
       ↑
       ├── Proof Checker application / feature module
       ├── Email application / feature module
       └── Broise demo application
```

Internally, the Runtime may initially contain Application, Window, Renderer, geometry, widget, page, and theme sources in one target. Split it into `Core`, `Platform`, `Renderer`, or `UI` libraries only when a real public/private boundary makes that worthwhile.

A demo executable should be separate from the reusable Runtime library. Consumer applications should link the Runtime target rather than compile Broise implementation files themselves.

## 7. Suggested evolution of the repository layout

Do not rename everything at once. Preserve the current `include/self` and `src` conventions until there is a clear reason to change them. As the project grows, a possible logical organization is:

```text
include/self/
├── core/          # Application lifecycle and shared low-level types
├── platform/      # Public platform-neutral window/event contracts, if needed
├── render/        # Renderer-facing public API
├── ui/            # Widget, container, page, layout, input, theme contracts
└── widgets/       # Only generic built-in Runtime widgets, not app-specific ones

src/
├── core/
├── platform/glfw/
├── render/opengl/
├── ui/
└── demo/          # Demo-only application and sample widgets

tests/
docs/
```

This is a future organizational option, not a demand to move the current files immediately. The first priority is clear ownership and one-way dependencies, not directory churn.

## 8. Recommended implementation order

1. **Stabilize the current skeleton:** resolve compile/interface mismatches, remove the CMake target cycle, define the application entry point, and document resource lifetime.
2. **Make rendering coherent:** establish coordinate semantics, solid rectangle/polygon drawing, and alpha blending.
3. **Create the generic widget tree:** root container, child ownership, bounds, update/render traversal, and basic z-order.
4. **Add basic input:** event adaptation, hit testing, pointer interaction, and focus basics.
5. **Add pages/workspaces:** persistent pages and a demo that mixes widgets from two separate feature modules on one page.
6. **Add shared theme tokens:** consistent grey/translucent default style used by the demo widgets.
7. **Harden the Runtime:** resize/DPI behavior, tests, error handling, documentation, and consumer CMake integration.

Implement each stage as a small reviewable change. Do not attempt a full rewrite or implement all future capabilities in one pass.

## 9. Decisions that should remain explicit

The following choices need a deliberate decision when their implementation becomes relevant:

- Exact `IWidget`/`Widget` lifecycle signatures and whether rendering receives `Renderer&` or a small context object.
- Concrete ownership container for heterogeneous child widgets.
- Whether widget layout uses only logical pixels or supports a documented mix of absolute and normalized coordinates.
- Whether page-level layout is freeform, grid-based, or initially a simple container tree.
- Whether widgets from separate feature modules are statically linked or dynamically loaded. Static/in-process C++ composition is the recommended first step.
- Whether the first version supports one window or multiple windows.
- Whether window-level transparency/backdrop blur is required, optional, or platform-specific.

Codex should not silently decide all of these during an unrelated implementation task. It should identify the decision, present a small recommendation with trade-offs, and keep the immediate implementation scoped.

## 10. Source links for the reviewed snapshot

- [Root CMakeLists.txt](https://github.com/Arc-Asumity/broise/blob/main/CMakeLists.txt)
- [Dependency configuration](https://github.com/Arc-Asumity/broise/blob/main/cmake/Deps.cmake)
- [Application interface](https://github.com/Arc-Asumity/broise/blob/main/include/self/core/Application.hpp)
- [Widget interface](https://github.com/Arc-Asumity/broise/blob/main/include/self/core/IWidget.hpp)
- [Window interface](https://github.com/Arc-Asumity/broise/blob/main/include/self/core/Window.hpp)
- [Renderer implementation](https://github.com/Arc-Asumity/broise/blob/main/src/core/Renderer.cpp)
- [Core CMake target](https://github.com/Arc-Asumity/broise/blob/main/src/core/CMakeLists.txt)
- [Widgets CMake target](https://github.com/Arc-Asumity/broise/blob/main/src/widgets/CMakeLists.txt)
