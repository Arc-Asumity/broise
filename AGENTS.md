# Codex Project Instructions — Broise

## 1. Project identity

Broise is intended to become a reusable, native C++ desktop UI Runtime for multiple Orbise desktop applications. It is infrastructure, not the implementation of one particular application.

Before making a non-trivial change, read:

- `docs/REQUIREMENTS.md` — product goals, functional requirements, constraints, and acceptance criteria.
- `docs/ARCHITECTURE.md` — current repository snapshot, target component boundaries, dependency direction, and evolution plan.

The documents describe both the observed code and the intended direction. Do not confuse planned capabilities with features that already exist. Always inspect the checked-out source before relying on a statement about implementation status; the local checkout may differ from the remote snapshot described in the documents.

## 2. Core engineering constraints

- Use modern C++ with **C++23** as the language target.
- Use CMake as the build system.
- Keep the design object-oriented and composition-oriented.
- Prefer straightforward, explicit designs over clever abstractions or framework-like machinery that has no present use case.
- Do not introduce Qt or Dear ImGui.
- The current initial graphics stack is GLFW + GLAD + OpenGL 4.6. Keep platform and graphics-backend details behind sensible Runtime boundaries; do not build speculative multi-backend infrastructure before a second backend is actually required.
- Windows is the current primary development environment. Avoid unnecessary Windows-only assumptions in public Runtime APIs.
- Runtime code must not depend on concrete application features such as Proof Checker or Email. Feature-specific widgets belong in consumer applications or feature modules that depend on Broise.

## 3. Ownership and lifetime

- Prefer direct value ownership for objects whose types and lifetimes are fixed (for example, an Application's Window and Renderer).
- Use references or raw pointers only as clearly documented non-owning relationships.
- Never use an owning raw pointer as a shortcut.
- Do not introduce `std::unique_ptr` mechanically for every object. For heterogeneous, dynamically owned polymorphic objects, choose an explicit safe ownership representation, explain why it is necessary, and keep it encapsulated behind the relevant container API.
- Widget/page ownership must be unambiguous. A parent/container should be the clear owner of its children; parent links, event targets, and lookup results should be non-owning.
- OpenGL resources must be released while a valid OpenGL context is still current. Preserve and document destruction order.
- GLFW initialization and termination must have a well-defined lifetime. Do not accidentally terminate GLFW while other windows still depend on it.

## 4. Coding practices

- Keep public headers focused. Include what a header needs; avoid gratuitous forward declarations and avoid creating include cycles.
- Keep implementation details in `.cpp` files where practical, especially OpenGL calls and GLFW callbacks.
- Use clear names and explicit types. Follow the established naming style unless a focused change has a clear reason to adjust it.
- Handle failure paths explicitly. Prefer the project's existing `std::expected` direction for recoverable initialization errors where appropriate; do not silently swallow failures.
- Do not add third-party dependencies without explaining the need and checking whether the existing stack can solve the problem.
- Do not implement an application-specific feature inside the Runtime merely to make a demo easier.
- Avoid premature optimization, but do not introduce obviously wasteful ownership or lifecycle patterns without reason.

## 5. Change discipline

For non-trivial tasks:

1. Inspect the relevant source files and current build configuration.
2. Summarize the current behavior and identify the files that need to change.
3. Make the smallest coherent change that satisfies the request.
4. Build and run relevant tests when the environment permits.
5. Report the exact commands run and their results. If a build or test was not run, say so explicitly.
6. Review the final diff for unrelated changes, accidental API changes, and generated files.

Do not perform broad rewrites, mass renames, or speculative redesigns unless explicitly requested. Do not delete user work. Never run destructive Git commands such as `git reset --hard`, `git clean -fd`, or force-push unless the user explicitly requests that exact operation.

Do not commit, push, publish, or create a pull request unless explicitly asked.

## 6. Working with the architecture documents

- Treat `REQUIREMENTS.md` as the product-level source of truth for intended capabilities.
- Treat `ARCHITECTURE.md` as the current architectural direction, not as permission to implement every future phase immediately.
- Implement in small, reviewable stages. If a requirement is ambiguous or conflicts with the existing code, describe the conflict and propose the smallest decision needed before making a large change.
- Keep documentation synchronized when a deliberate architectural decision changes.
- Do not claim a requirement is complete merely because a corresponding class or header exists. Verify actual behavior.

## 7. Default response style

When discussing work with the maintainer, be concise and technically precise. Distinguish facts observed in the repository from recommendations and assumptions. Prefer concrete file paths, dependency relationships, and build/test evidence over vague architectural language.
