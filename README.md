# Atlas

Atlas is a high-performance C library project designed for modular application development. It provides a comprehensive set of utilities and abstractions for building robust, cross-platform applications.

## Overview

Atlas consists of several core modules:

### Core Library (`libs/core`)
- **Memory Management**: Custom allocators (heap, bump, scratch, page)
- **Containers**: Dynamic arrays, bitsets, strings
- **String Utilities**: UTF-8 support, ASCII operations, formatting
- **System Abstractions**: Threading, file I/O, time operations
- **Cross-Platform Support**: Linux and Windows compatibility
- **Algorithms**: Sorting, random number generation, shuffling

### Job System (`libs/jobs`)
- **Task Scheduling**: Graph-based job dependency management
- **Parallel Execution**: Multi-threaded job execution
- **Work Queue**: Thread-safe work distribution
- **DOT Export**: Visualization support for job graphs

### Testing Framework (`libs/anvil`)
- **Spec-Based Testing**: BDD-style test specifications
- **Pretty Output**: Colored test result formatting
- **Result Tracking**: Comprehensive test result management

### CLI Library (`libs/cli`)
- **Argument Parsing**: Command-line argument processing
- **Validation**: Input validation and error handling
- **Help Generation**: Automatic help text generation

### JSON Library (`libs/json`)
- **RFC 7159 Compliance**: Full JSON specification support
- **Document Management**: Create, manipulate, and query JSON documents
- **Parsing**: Convert JSON text to structured data with detailed error reporting
- **Serialization**: Write JSON with compact or pretty-printed formatting
- **Deep Equality**: Compare JSON values with proper semantic equality
- **Unicode Support**: Full UTF-8 string handling with escape sequences

## Requirements

- CMake 3.15 or higher
- C11-compatible compiler (GCC, Clang, or MSVC)
- Git for version control

## Building

The project uses CMake with automatic platform and compiler detection:

```bash
# Configure the build
cmake -B build -S .

# Build the project
cmake --build build --config Release
```

The build system automatically:
- Detects your platform (Linux/Windows) and compiler (GCC/Clang/MSVC)
- Configures appropriate compile flags and warnings
- Enables sanitizers on Clang/Linux builds for debugging

## Testing

Run all tests:
```bash
cmake --build build --target test
```

Run individual library tests:
```bash
cmake --build build --target test.core
cmake --build build --target test.anvil
cmake --build build --target test.cli
cmake --build build --target test.jobs
cmake --build build --target test.json
```

## Project Structure

- `libs/core/` - Foundation library with memory management and system abstractions
- `libs/jobs/` - Task scheduling and parallel execution system
- `libs/anvil/` - BDD-style testing framework
- `libs/cli/` - Command-line argument processing
- `libs/json/` - RFC 7159 compliant JSON parsing and serialization library
- `apps/` - Example applications demonstrating library usage
- `cmake/` - Build system configuration and platform detection

## Documentation

API documentation is generated using Doxygen. The project follows Doxygen commenting conventions with public API documented in header files and implementation details in source files.

## Contributing

Please ensure all tests pass before submitting changes:
```bash
cmake --build build --target test
```