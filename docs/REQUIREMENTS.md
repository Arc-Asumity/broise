# Broise UI Runtime — Product Requirements

**Document status:** Initial product and engineering requirements  
**Language target:** C++23  
**Build system:** CMake  
**Initial platform/rendering stack:** GLFW + GLAD + OpenGL 4.6  

## 1. Product vision

Broise is a reusable desktop UI Runtime intended to provide a common foundation for multiple desktop applications developed within the Orbise ecosystem.

Broise should provide the shared window lifecycle, rendering foundation, widget composition model, input dispatch, page/workspace model, and visual design system. Individual applications should provide their own domain logic and feature-specific widgets while relying on the same Runtime conventions and visual language.

The Runtime must not become a Proof Checker-specific UI library. Proof Checker is one possible consumer. Email and other future applications or feature modules are also possible consumers.

A key product idea is that a page/workspace can contain independent widgets from different feature areas. For example, a user may place a Proof Checker writing/editor widget and an Email status widget on the same page. The page is therefore a composition surface, not necessarily a full-screen representation of one application. The Runtime should make this possible without making its core depend on either application.

### Terminology clarification

- A **Part** is a Runtime-provided UI building block. Parts can be nested into Part Trees and represent structural or visual elements such as containers, buttons, text, and images. The Runtime provides the infrastructure for managing, composing, updating, and rendering Parts.
- **RootPart** is the conceptual root node of a Part Tree. It is not an application-level Widget. Whether RootPart needs a distinct concrete class remains open.
- A **Widget** is an application- or feature-provided software component, such as a Proof Checker Widget or Email Widget. A Widget may use multiple Parts to construct its UI, but it is not itself a Part or a node in a Part Tree.
- The Runtime owns the Part system and remains independent of application-specific Widgets. The software-level Widget system, its integration, Workspace, and Page architecture are future design work.
- The former UI-level `IWidget` and `MainWidget` identifiers referred to the Part concept; their names must not be used to infer a software-level Widget system.

## 2. Product principles

1. **Shared foundation:** common desktop behavior and visual conventions should be implemented once and reused by multiple applications.
2. **Composable widgets:** useful UI components should be independently placeable and reusable. A page may compose widgets from more than one feature module.
3. **Persistent workspaces:** switching pages should not unnecessarily destroy the inactive page or reset the state of its widgets.
4. **Consistent visual language:** the default visual direction is modern, restrained, grey, and translucent. Applications should use shared theme tokens rather than independently hard-coding unrelated styles.
5. **Native desktop behavior:** the Runtime should manage real desktop windows and input, rather than being a web page embedded in a desktop shell.
6. **Simple architecture:** solve current requirements directly. Avoid unnecessary abstraction layers, speculative plugin systems, and premature multi-backend support.
7. **Application independence:** the Runtime provides generic UI infrastructure; consumer applications own their business logic and domain-specific widgets.

## 3. Target users and consumers

- Desktop application developers building software on top of Broise.
- Orbise feature modules that provide widgets to a shared desktop environment.
- End users who want a consistent interface and the ability to arrange or retain widgets from different feature areas in shared pages/workspaces.

## 4. Functional requirements

### FR-01 — Application lifecycle

The Runtime shall provide an `Application` entry point that coordinates initialization, the main loop, and orderly shutdown.

The application lifecycle shall coordinate the Window, Renderer, active page/workspace, input processing, UI updates, and frame presentation. Initialization failures must be reported rather than leaving a partially initialized application in an undefined state.

### FR-02 — Window management

The Runtime shall provide a Window abstraction that owns the native window and its associated graphics context.

At minimum it shall support:

- Window creation with a title and configurable initial dimensions.
- Window close-state inspection.
- Event polling and buffer presentation.
- Framebuffer-size updates and resize handling.
- Safe native-window and graphics-context teardown.

The public Runtime API should not require consumer widgets to call GLFW directly.

### FR-03 — Rendering

The Runtime shall provide a Renderer abstraction for drawing UI content.

The initial backend is OpenGL 4.6. The first useful rendering capability is solid-color geometry (at least rectangles/polygons) with alpha support suitable for translucent UI surfaces. Widget code should submit drawing through the Runtime rendering API and should not issue arbitrary OpenGL calls directly.

The rendering API should be small and practical. Text shaping, advanced effects, clipping, batching, blur, and additional rendering backends may be introduced in later phases when required; they are not reasons to build a large graphics framework now.

### FR-04 — Widget model and composition

The Runtime shall provide a generic widget model that supports:

- A common widget interface or base type.
- A widget's bounds and basic state (at least visibility and enabled/disabled state where applicable).
- Parent/child composition for container-like widgets.
- Recursive update and rendering through a well-defined lifecycle.
- A clear way to add, remove, and find child widgets.
- A clear z-order or drawing order for overlapping children.

The root UI must be generic. `Application` should not require one hard-coded `MainWidget` type as its permanent root abstraction.

A widget should be reusable outside the application that first introduced it. Feature-specific widgets may depend on Broise's public Runtime API; Broise core must not include or link against those feature-specific widgets.

### FR-05 — Geometry and layout foundations

The Runtime shall provide simple geometry types such as `Point`, `Color`, and `Rect`.

The current `Rect` splitting and relative translation helpers are useful foundations. Their coordinate semantics must be documented. The Runtime should establish one consistent UI coordinate system (recommended: logical window coordinates for layout, with conversion to OpenGL normalized device coordinates isolated inside the renderer).

The initial layout system should remain deliberately small. Basic positioning, sizing, relative subdivision, and container composition are sufficient for the first Runtime milestone. Do not build a complete CSS-like layout engine without a concrete requirement.

### FR-06 — Input and event dispatch

The Runtime shall eventually translate native window input into Runtime-level events and route those events to the appropriate widget.

At minimum, the planned system should cover pointer position, pointer buttons, keyboard input, hit testing, and a coherent focus model. Event routing should respect widget visibility, bounds, and drawing order.

Input dispatch is a Runtime responsibility. Individual feature widgets should not need to register their own raw GLFW callbacks.

### FR-07 — Pages and workspaces

The Runtime shall provide a page/workspace model.

- A workspace contains a composition of widgets.
- A workspace may contain widgets supplied by different feature modules.
- The user can switch between workspaces/pages.
- Inactive pages should retain their widget instances and state unless explicitly closed or unloaded.
- The Runtime shall expose a clear way to create, select, close, and enumerate pages, subject to the initial milestone's scope.

For the first implementation, widgets can be constructed and registered directly by a consumer application. A dynamic plugin loader is not a prerequisite for the page model.

### FR-08 — Shared theme and styling

The Runtime shall provide shared theme/style values for common UI properties, including at least colors, opacity, spacing, borders, and typography-related settings as those capabilities become available.

The default theme direction is a modern grey translucent interface. The shared theme should keep widgets from different feature modules visually coherent. It should be possible to adjust the theme centrally rather than editing every widget.

Actual operating-system-level transparent windows or backdrop blur are platform-specific enhancements. They must not be confused with alpha-blended translucent UI elements rendered inside a normal window. Provide a readable fallback when OS-level effects are unavailable.

### FR-09 — Reusable library integration

Broise shall be consumable by other CMake-based C++ applications as a library/runtime target. A demo executable may be used to exercise the Runtime, but the Runtime itself must not be architecturally tied to a single executable entry point.

Consumer applications should depend on Broise's public headers and CMake target(s), not on private source files or internal OpenGL implementation details.

### FR-10 — Error handling and lifecycle safety

Window, graphics-context, renderer, and other essential initialization failures shall have explicit error paths. Resource ownership must be clear, and graphics resources must be destroyed before the graphics context is destroyed.

### FR-11 — DPI and resizing

The Runtime shall distinguish logical UI dimensions from framebuffer pixel dimensions where needed. Resizing and high-DPI displays must not make layout geometry or input hit testing inconsistent. This may be implemented progressively, but the coordinate model must not prevent it.

## 5. Non-functional requirements

- **Maintainability:** modules have clear responsibilities and one-way dependencies.
- **Reusability:** no hard dependency on Proof Checker, Email, or another consumer application.
- **Portability:** Windows is the first development target; public interfaces should avoid unnecessary platform-specific types so other desktop platforms remain feasible.
- **Predictable lifetime:** ownership and destruction order are explicit, especially for widgets, pages, windows, and graphics resources.
- **Performance:** the Runtime should be responsive for ordinary desktop UI workloads. Optimize based on observed needs rather than speculative complexity.
- **Build reproducibility:** CMake dependencies and versions should be explicit; the project should be buildable through a documented configuration.
- **API discipline:** public interfaces should remain small and intentional. Changes to them should be made coherently, not through accidental leakage of implementation details.

## 6. Explicit constraints and non-goals

The following are constraints for the current direction:

- C++23 and CMake.
- GLFW + GLAD + OpenGL 4.6 as the initial platform/rendering stack.
- Object-oriented, composition-based design.
- No Qt and no Dear ImGui.
- Avoid unnecessary abstraction and premature generalization.
- Broise core must not contain application-specific business logic.

The following are **not required for the first milestone** unless explicitly requested:

- A dynamic cross-process application embedding system.
- A stable third-party binary plugin ABI or a cross-language C ABI.
- A web-based frontend or embedded browser runtime.
- A complete CSS-like layout engine.
- Advanced text shaping, rich document editing, or accessibility APIs in the first rendering pass.
- Multiple graphics backends.
- Complex GPU effects, desktop blur, or platform-specific glass effects.
- A full application launcher, package manager, or operating-system shell.

Feature modules may initially be ordinary C++ code linked into the consumer executable. A plugin/factory registration mechanism can be designed later if a real use case requires it.

## 7. Suggested implementation milestones

### Milestone 0 — Make the foundation coherent

- Remove circular CMake target dependencies.
- Make `Application`, `Window`, `Renderer`, and the root widget lifecycle compile and agree on their interfaces.
- Ensure the application actually creates and runs a window.
- Ensure OpenGL resources are destroyed while the context remains valid.
- Add a minimal build/run example and document the build command.

### Milestone 1 — Minimal usable UI Runtime

- Generic widget/container tree.
- Basic geometry and documented coordinate semantics.
- Renderer support for solid-color rectangles/polygons and alpha blending.
- Basic layout/positioning.
- Basic pointer hit testing and input routing.
- At least one consumer/demo application that constructs and renders several widgets.

### Milestone 2 — Pages and shared styling

- Page/workspace manager.
- Multiple pages with state retained when switching.
- A single page containing widgets from at least two separate feature modules in a demonstration.
- Shared grey/translucent theme tokens.

### Milestone 3 — Runtime quality and extension points

- More complete keyboard/focus behavior.
- Better DPI and resize behavior.
- Widget lifecycle, event ordering, and ownership tests.
- Refined public CMake targets and consumer integration examples.
- Decide whether a C++ module/widget factory or plugin mechanism is actually necessary.

## 8. Acceptance criteria for the first meaningful release

A first meaningful release should satisfy all of the following:

1. A consumer C++23 application can link Broise through CMake and create a native window.
2. The application runs a stable event/update/render loop and exits cleanly.
3. A generic root container can own and render multiple child widgets.
4. At least basic solid-color UI geometry can be drawn through the Runtime renderer.
5. Widgets have documented bounds and predictable coordinate semantics.
6. Basic pointer interaction can be routed to the correct widget, or the first release explicitly documents that input is deferred.
7. At least two pages can be switched without losing the inactive page's widget state.
8. A demonstration page can contain widgets originating from two distinct feature modules without the Runtime depending on either module.
9. The Runtime has a shared default theme/style source.
10. The build is reproducible and the project reports build/test results honestly.

The milestones are staged. These acceptance criteria describe the first meaningful release, not a demand to implement every item in one change.
