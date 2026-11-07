# AGENTS.md — Broise Project Instructions

## 1. Project Identity

Broise is a reusable, native C++ desktop UI Runtime for multiple Orbise desktop applications.

The project uses:

- C++23
- CMake
- GLFW
- GLAD
- OpenGL 4.6

Prioritize correctness, maintainability, consistency, and technical simplicity over implementation speed or code volume.

## 2. Mandatory References

Before starting any non-trivial task, read:

- `docs/UML.md` — authoritative architectural reference.
- `docs/REQUIREMENTS.md` — product requirements and functional expectations.
- `docs/ARCHITECTURE.md` — supplementary project documentation.

Always inspect the actual source code before making assumptions about implementation status.

### UML.md Restrictions

`docs/UML.md` is maintained exclusively by the project maintainer.

- You may read and reference it.
- You MUST NOT modify its contents.
- You MUST NOT rename, move, delete, regenerate, or reformat it.
- You MUST NOT modify its Mermaid diagrams.
- You MUST NOT independently change the architecture represented by it.

If you identify a problem or inconsistency in the UML, explain it and ask the maintainer.

Do not duplicate the architecture described in `docs/UML.md` in this file.

## 3. Development Responsibilities

### 3.1 Maintainer Responsibilities

The maintainer is responsible for:

- Architectural decisions.
- Creating and modifying C++ header files.
- Defining class interfaces.
- Defining class responsibilities.
- Defining ownership and lifetime semantics.
- Approving changes to interfaces and architectural boundaries.

### 3.2 AI Responsibilities

**Your primary responsibility is implementing existing interfaces in `.cpp` files.**

Strict rules:

1. Only write implementation files (`.cpp`).
2. Do NOT modify existing `.hpp` files.
3. Do NOT create new `.hpp` files.
4. Do NOT rename or move header files.
5. Do NOT independently redesign interfaces.
6. Do NOT change function signatures or class declarations.
7. Do NOT modify APIs to simplify implementation.

If an implementation requires a header change:

1. Stop and explain the problem.
2. Identify the affected header and declarations.
3. Explain why the existing interface is insufficient.
4. Propose the smallest necessary change.
5. Wait for the maintainer's approval.

**Never work around an inconvenient interface by introducing an alternative architecture.**

You may use implementation-local helper functions, types, and classes inside `.cpp` files when appropriate.

If a task explicitly requires header changes, ask for permission before proceeding.

## 4. Coding Style

Follow the Google C++ Style Guide:

https://google.github.io/styleguide/cppguide.html

Use the repository's `.clang-format` configuration.

Specific requirements:

- Use consistent Google-style naming and formatting.
- Use clear and descriptive identifiers.
- Avoid unnecessary abbreviations.
- Keep functions focused and reasonably sized.
- Keep classes cohesive.
- Avoid excessive nesting.
- Avoid redundant comments.
- Write comments to explain intent, constraints, and non-obvious behavior.
- Handle errors explicitly.
- Do not silently swallow failures.

Use C++23 features when they genuinely improve clarity, safety, or correctness.

Do not use unnecessarily complicated language features or clever techniques merely to make code appear sophisticated.

Technical elegance comes from clarity, consistency, simplicity, and correctness.

## 5. Ownership and Memory Management

Ownership must be explicit.

- Prefer direct value ownership when object types and lifetimes are fixed.
- Use references and raw pointers for clearly documented non-owning relationships.
- Never use owning raw pointers as a shortcut.
- Do NOT introduce `std::unique_ptr` mechanically for every object.
- Do NOT introduce `std::shared_ptr` merely to avoid making ownership decisions.
- Shared ownership requires a genuine architectural justification.
- Avoid unnecessary dynamic allocation.
- Do not obscure object lifetimes behind excessive wrapper classes or management layers.

For dynamically created polymorphic objects, choose an appropriate and safe ownership mechanism.

When destruction must be performed through a specific interface or factory, preserve that requirement.

If an existing interface does not provide a safe ownership or lifetime mechanism, ask the maintainer rather than independently redesigning it.

## 6. File Organization

- All C++ header files (`.hpp`) belong under `include/`.
- All C++ implementation files (`.cpp`) belong under `src/`.
- Organize files into directories according to their functional areas.
- Use consistent directory and filename conventions.
- Use project include paths rather than relative paths to reach headers.
- Do not introduce unnecessary directory layers.
- Do not create separate header directories merely to distinguish public and private implementation details.

The maintainer creates and modifies header files. These organization rules do not authorize AI to create or modify them.

## 7. Technical Constraints

- Use C++23.
- Use CMake.
- Do not introduce Qt.
- Do not introduce Dear ImGui.
- Do not introduce third-party dependencies without explaining the necessity.
- Avoid unnecessary platform-specific assumptions in interfaces.
- Keep platform-specific implementation details appropriately encapsulated.
- Do not implement application-specific features inside the Runtime merely to simplify a demonstration.
- Avoid speculative infrastructure for requirements that do not currently exist.

### Module ABI

- Use native C++ interfaces and C++ linkage.
- C ABI is explicitly prohibited.
- Do NOT introduce `extern "C"` factory functions.
- Do NOT introduce C-linkage bootstrap symbols.
- Do NOT replace the approved C++ interface with a C-compatible interface.
- Do not claim cross-toolchain compatibility where the ABI does not provide it.

## 8. Implementation Discipline

For every non-trivial task:

1. Inspect the relevant source files.
2. Inspect the relevant CMake configuration.
3. Read the applicable requirements and architectural references.
4. Summarize the existing implementation and identify the files that need changes.
5. Implement the smallest coherent change.
6. Build and run relevant tests when possible.
7. Review the final diff.

Do not:

- Perform broad rewrites without approval.
- Introduce unrelated changes.
- Rename large numbers of files without approval.
- Add abstractions without a concrete requirement.
- Duplicate existing functionality.
- Change interfaces to accommodate an implementation shortcut.
- Modify unrelated documentation.
- Delete user work.

If an implementation problem reveals a possible architectural issue, report it instead of independently redesigning the system.

## 9. Build and Testing

- Use the existing CMake build configuration.
- Do not silently change build options or dependencies.
- Build affected targets after implementation when possible.
- Run relevant tests.
- Fix warnings and errors introduced by your changes.
- Do not claim successful compilation or testing without actually performing it.

When reporting results, state:

- Files modified.
- Implementation completed.
- Build commands executed.
- Test commands executed.
- Actual results.
- Remaining issues or unverified behavior.

If the environment prevents building or testing, explicitly state that limitation.

## 10. Git Discipline

- Do not commit unless explicitly requested.
- Do not push unless explicitly requested.
- Do not create pull requests unless explicitly requested.
- Never run destructive Git commands such as `git reset --hard`, `git clean -fd`, or force-push unless the maintainer explicitly requests that exact operation.
- Never discard or overwrite user changes.
- Review the final diff for unintended modifications.

## 11. Communication

When encountering ambiguity:

- Ask rather than guess.
- Explain the technical issue precisely.
- Distinguish observed facts from assumptions.
- Provide concrete alternatives when a decision is required.

Be concise and technically precise.

Do not claim that a task is complete merely because corresponding source files have been written.

**The maintainer owns the architecture and interfaces. Your responsibility is to implement them faithfully.**