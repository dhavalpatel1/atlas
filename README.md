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

## Building

The project uses CMake and supports multiple compilers:

```bash
cmake -B build -S .
cmake --build build --config Release
```

## Testing

Run the test suite:

```bash
cmake --build build --target test
```

## Documentation

This documentation is generated using Doxygen and provides comprehensive API reference for all modules.