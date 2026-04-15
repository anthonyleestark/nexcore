# NexCore

**NexCore** is a low-level foundation library that defines the core primitives of the Nex-ecosystem.  
Built with performance, clarity, and long-term scalability in mind, NexCore provides essential building blocks such as fundamental types, utilities, and system abstractions — all designed to be lightweight, consistent, and dependency-free.

It is not a framework, nor a product — but the ground upon which everything else is built.  
NexCore exists to unify, simplify, and stabilize the core of every system that depends on it.

---

## Overview

NexCore provides a cohesive set of cross-platform primitives and abstractions that serve as the foundation layer for all projects in the Nex-ecosystem. It eliminates inconsistencies across platforms, compilers, and standard library implementations while providing modern C++ conveniences through a carefully designed API.

The library is organized into modules:

- **Base Module**: Fundamental types, containers, platform detection, compiler abstraction, and low-level utilities
- **Core Module**: Higher-level primitives such as strings, time handling, geometry, UUIDs, error handling, and more

---

## Requirements

- **C++ Standard**: C++20 or later (required)
- **Supported Platforms**:
  - Windows (x86, x64, ARM)
  - Linux (x86, x64, ARM)
  - macOS (x64, ARM64)
- **Supported Compilers**:
  - GCC 10+ (with C++20 support)
  - Clang 10+ (with C++20 support)
  - MSVC 2019+ (with C++20 support)

---

## Installation

### Building from Source

```bash
# Clone the repository
git clone https://github.com/anthonyleestark/nexcore.git
cd nexcore

# Initialize dependency submodules
git submodule update --init --recursive

# Build instructions will be added when build system is configured
```

---

## Design Philosophy

### Principles

NexCore follows a simple philosophy: keep the core small, stable, and uncompromising — so everything above it can grow without friction.

1. **Clarity**: Code should be self-documenting and easy to understand
2. **Performance**: Zero-cost abstractions wherever possible
3. **Consistency**: Unified naming conventions and API patterns across the entire ecosystem
4. **Portability**: Write once, compile everywhere (Windows, Linux, macOS)
5. **Safety**: Leverage modern C++ features for type safety and memory safety
6. **Minimalism**: Include only what is essential; no bloat

### Positioning

NexCore is not designed to solve problems at the application level.  
It exists to make solving those problems possible — efficiently, consistently, and at scale.

---

## Contributing

Contributions are welcome! Please ensure your code follows the project's coding standards and design philosophy.

1. Fork the repository
2. Create a feature branch
3. Commit your changes with clear commit messages
4. Submit a pull request

---

## License

NexCore is licensed under the **BSD-3-Clause License**.  
Copyright (c) 2025-2026 Anthony Lee Stark. All rights reserved.

See [LICENSE](LICENSE) for the full license text.
