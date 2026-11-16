# AGENTS.md — Arduino Project Guidelines

## Project Overview

This project is an **Arduino-based** library that allows access to multiple sensors through a unified interface that manages sensor and CPU power, data storage, and data publication to online repositories.

The goal is to produce **clean, well-documented, reliable, and maintainable** embedded C++ code for Arduino.

The library should follow an object-oriented structure.

The majority of the users of this library are expected to be students and environmental scientists.
It is expected that users will have very minimal coding experience.
Public APIs and examples should therefore favor simple, discoverable usage.
Avoid requiring users to understand templates, the STL, RTTI, smart pointers, type traits, or other advanced C++ features unless there is no practical alternative.

---

## General Agent Instructions

* The local environment is a Windows environment.
  * Run commands using Windows Powershell 7, which is located at "C:\Program Files\PowerShell\7\pwsh.exe"
  * Use Windows PowerShell commands, not Linux shell commands.
* Make the **smallest change necessary** to accomplish the requested task, prioritizing the fewest files and functions touched, while still implementing a complete and correct fix.
* Always favor simple elegant coding structures with as little complexity as possible.
* Do not refactor unrelated code, rename unrelated symbols, reorganize files, or "clean up" existing code unless requested.
* Preserve existing behavior unless the requested change specifically requires changing it.
* Preserve existing inline comments and debugging printouts when modifying or refactoring code.
* Before changing a public class, function, macro, constructor, or other API, search the repository for existing uses and update affected examples and tests as necessary.
* Avoid breaking the public API unless the user explicitly requests a breaking change.
* Follow existing project conventions when adding new code.
  When conventions differ between files, follow the convention used in the file most similar to the one being modified.
* Do not introduce modern C++ features that are incompatible with the project's C++11 requirements.
* Do not introduce STL dependencies, RTTI, exceptions, or other facilities that are unavailable or undesirable on the supported Arduino platforms.
* Source and header files should directly include the headers required for the symbols they use rather than relying on transitive includes from unrelated headers.
* If a requested change conflicts with an existing project convention, explain the conflict and use the approach that best satisfies the user's explicit request.
* If a change would require a potentially breaking API decision and the user's intent is unclear, ask before proceeding.
* If testing cannot be performed, say so explicitly.
* Never claim that code was tested when it was not.
* Do not manually edit generated files unless the user explicitly requests modifications to the generated output itself.

### Instruction Priority

When instructions conflict, use the following priority:

1. The user's explicit request.
2. Correctness and compatibility with the project's supported platforms.
3. The requirements in this file.
4. Existing implementation conventions.

When possible, preserve existing behavior and API compatibility.

---

## Workspace Structure

* The library source code is in the `src/` folder and sub-folders.
  * The primary classes are in the top-level `src/` folder.
  * The most important user-facing class is the `Logger` class.
  * Sensor implementations derived from the `Sensor` class are in the `src/sensors/` folder.
  * Modem implementations derived from the `loggerModem` class are in the `src/modems/` folder.
  * Data publisher implementations derived from the `dataPublisher` class are in the `src/publishers/` folder.
  * Processor-specific watchdog implementations are in the `src/WatchDogs/` folder.

* Library dependencies needed to build and test the library are in the `lib/` folder and sub-folders.
  * **Do not modify files in `lib/` unless the user explicitly requests it.**

* Examples for users are in sub-folders of the `examples/` folder.
  * Each example must be in its own folder.
  * The example folder and `.ino` file must have identical base names.
  * Example folders should also contain a `ReadMe.md` file.

* Local compilation tests are in the `tests/` folder.
  * These are not committed to the repository.
  * Test files may be modified as necessary when developing or testing changes.

* Helpers for continuous integration testing are in the `continuous_integration/` folder.
* Extra documentation and helpers for building the documentation with Doxygen are in the `docs/` folder.

* Library dependencies needed to build and test the library are in the `lib/` folder and sub-folders.
  * **Do not modify files in `lib/` unless the user explicitly requests it.**
  * This folder is not committed to the repository.

* The `archive/` folder contains outdated and non-working code.
  * Do not use archived code as a model for new code unless specifically instructed to do so.
  * Never modify archived files unless specifically requested to.
  * Archived code is not committed to the repository.
  * Files in `archive/` are out of scope for reviews unless the user explicitly requests review of archived code.

* The `planning/` folder contains planning documents and notes for the project.
  * This is where all planning and design discussions for the project are documented.
  * When making new plans or design decisions that materially affect project architecture, APIs, roadmap items, or implementation strategy, update the relevant planning documents.
  * New design documents should be added to this folder.
  * `planning/finished` contains planning documents that have been completed and are no longer active.
  * `planning/shelved` contains planning documents that have been shelved and are no longer active.
  * The files in the `planning` parent folder are planning documents that are currently active and being worked on.
  * This folder is not committed to the repository.

* These files and subfolders are auto-generated by CI or other processes in other workflows and do not need to be evaluated:
  * `continuous_integration_artifacts/`
  * `./keywords.txt`
  * `.history/`

---

## Coding Standards

NOTE:  Planning documents and preliminary tests for files and folders ignored by the project's `.gitignore` do not need to follow these coding standards.
Markdown files created for planning and reviews do not need to comply with Markdownlint or any other standard.

### Language

* Arduino C++ compatible with Arduino IDE 2.x.
* C++11.
* **Do not use the C++ standard library/STL.**
* **Do not use RTTI.**
* Code must be suitable for embedded Arduino platforms with limited memory and processing resources.
* Avoid unnecessary dynamic memory allocation.
* Do not introduce exceptions unless specifically requested and supported by the target platforms.

### Object-Oriented Design

* Follow the existing object-oriented design of the library.
* Keep implementation-specific details out of user-facing APIs whenever practical.
* Prefer simple, concrete user-facing classes over exposing unnecessary implementation complexity.

### Templates and Type Erasure

Templates may be used internally when they provide a useful compile-time abstraction, but template parameters should not unnecessarily leak into user-facing APIs.

When multiple template specializations need to be accessed through a common interface, use a **non-templated polymorphic base class** as the common interface.

For example:

```cpp
class templatedClassBase {
public:
    virtual ~templatedClassBase() {}
};

template <typename T>
class templatedClass : public templatedClassBase {
    // Implementation specific to T.
};
```

This allows code outside the template hierarchy to use:

```cpp
templatedClassBase* object;
```

without knowing which template specialization is being used.

Do not require library users to provide template arguments merely to interact with a common interface.

Do not use RTTI, `dynamic_cast`, `typeid`, STL type-erasure facilities, or other advanced mechanisms to solve this problem.

### Documentation

* Unique functions, defines, classes, and parameters must be documented with Doxygen comments in the header files.
* Doxygen comments should be clear and follow Doxygen conventions.
* Use Java-style `@commands` rather than Qt-style `\commands`.
* Override functions do not need Doxygen comments in the header files unless they have unique parameters or behavior that needs to be documented.
* Documentation should be in header (`*.h`) files, not implementation (`*.cpp`) files.
* New source-code and example directories must include a `ReadMe.md` file unless an equivalent ReadMe.md` already exists at a higher level covering that directory.
* New function implementations should include
inline comments where needed to explain non-obvious behavior, algorithms, assumptions, hardware interactions, or design decisions.
* Inline function implementation comments should not be exposed for Doxygen.

### Formatting

* C++ code files should be formatted according to the `.clang-format` file in the repository.
* Python files for documentation and continuous integration should be formatted with Black.
* Markdown files should follow markdownlint rules using the configuration `.markdownlint-cli2.yaml`.
  * Each sentence should be on its own line with no maximum line length.
* Do not reformat unrelated code merely because it does not match the desired formatting.

### Memory

* Be conscious of RAM, flash, and processor usage.
* Use the existing project configuration mechanisms for compile-time configuration values.
* For configuration values that are intended to be preprocessor macros, use the project's `#define` configuration mechanism rather than introducing unnecessary variables.
* Follow existing use of `F()` within the modified file.
* When no convention exists, prefer `F()` for string literals longer than a single character.
* Do not introduce widespread `F()` changes unrelated to the requested task.
* Do not replace ordinary typed constants with macros solely for style.

---

## Conventions

### Files

* Declarations and documentation should be in header files with an `.h` extension.
* Except when required for templates, implementation should be in a separate `.cpp` file.
* Pure templates that will never be accessed by external library users can use a `.tpp` extension, although `.h` is also acceptable.
* Do not put implementation into headers unless required by templates or another clear technical reason.

### File Comment Blocks

Every file in the library should have a Doxygen comment header including the file name, copyright, author, and a brief description.

For example:

```cpp
/**
 * @file LoggerModem.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the type-erased loggerModem class and the variable
 * subclasses Modem_RSSI, Modem_SignalPercent, Modem_BatteryState,
 * Modem_BatteryPercent, and Modem_BatteryVoltage - all of which are implemented
 * as "calculated" variables.
 */
```

Preserve existing author and copyright information when modifying files.

### Header Includes

All headers should have header guards at the top of the file, immediately below the Doxygen comment block.

Library headers should follow the project's standard configuration and debugging include pattern:

1. Include `ModSensorConfig.h`.
2. Include `ModSensorDebugConfig.h`.
3. Define the appropriate debugging label macros.
4. Include `ModSensorDebugger.h`.
5. Undefine the debugging label macros.

For example:

```cpp
// Header Guards
#ifndef SRC_LOGGERMODEM_H_
#define SRC_LOGGERMODEM_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_LOGGERMODEM_DEBUG
#define MS_DEBUGGING_STD "LoggerModem"
#endif
#ifdef MS_LOGGERMODEM_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "LoggerModem"
#endif

// Include the debugger
#include "ModSensorDebugger.h"

// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
```

Do not add unnecessary includes.
If a header must remain independent of the library configuration/debugging system for a specific technical reason, preserve that requirement rather than blindly applying the pattern.

### Printing and Debugging

* Use the `PRINTOUT()` macros for printing key information to the serial port.
* Use the `MS_DBG` and `MS_DEEP_DBG` macros for detailed and verbose information for tracing sensor, logger, and library errors.
* All debugging definitions should be added to the debugging configuration file:

```text
src/ModSensorDebugConfig.h
```

* Do not introduce ad-hoc debugging macros when an existing project debugging mechanism can be used.

### Configuration

* All macro variables that can be used to configure the library should be placed and documented in:

```text
src/ModSensorConfig.h
```

* Do not scatter user-configurable library macros throughout unrelated source files.

### User Examples

* All sensors, publishers, and modems should have example code in both:

  * `examples/menu_a_la_carte/menu_a_la_carte.ino`
  * `tests/logger_test/logger_test.cpp`
* These two files are intentionally very similar.
* `logger_test.cpp` contains real credentials and pin numbers and is **not committed to the repository**.
* Whenever a new sensor, modem, or publisher is created, add example code to both files.
* New test/example code should be surrounded by the appropriate defines used by the project for selecting tests.
* Keep user-facing examples simple and representative of the intended public API.

---

## Repository Safety

* Do not modify `lib/` dependencies unless explicitly requested.
* Do not modify archived code merely to make it consistent with current code.
* Do not add credentials, passwords, API keys, private tokens, or other secrets to committed files.
* Treat `tests/logger_test/logger_test.cpp` as a local-only test file containing credentials and pin numbers.
* Do not copy credentials or other secrets from local test files into examples, documentation, source code, or other committed files.
* Do not commit generated build artifacts unless the repository specifically requires them.

---

## Before Finishing a Task

A substantial change includes modifications to library source code, public APIs, build configuration, testing infrastructure, or behavior visible to users.
For all substantial changes, before considering a coding task complete, follow this checklist:

1. Verify that the requested behavior has been implemented.
2. Check for necessary includes and declarations.
3. Check that existing public APIs have not been unintentionally changed.
4. Check that relevant examples and tests have been updated.
5. Ensure new or modified public functionality has appropriate Doxygen documentation.
6. Ensure no unrelated files or code were unnecessarily changed.
7. Perform testing according to the Testing Rules section.
8. If the changes were based on planning documents, update the planning documents after completion detailing what was done.
9. Clearly report what was changed and what testing was actually performed.

For trivial changes (e.g., documentation-only, single-line fixes), this checklist may be abbreviated or skipped at your discretion, but always perform at least basic verification that the change accomplishes the requested task.

---

## Building and Testing

### General Testing Rules

* If changes only modify files that are ignored by the .gitignore file, no testing is needed.
* If the required environment, hardware, credentials, or another dependency prevents testing, report that limitation clearly.
* Never state or imply that hardware testing was performed if only compilation or static analysis was performed.
* If only documentation blocks or non-source files are modified, the source code does not need to be tested.
* If no doxygen documentation blocks have been modified, no new public objects have been added, no new files have been created, and the documentation configuration files in the `./docs` folder have not been modified, the documentation does not need to be checked.

### Correcting Formatting

clang-format can be run on all c++ files at once using the batch script `.\clang_format_all.bat`.

The configuration file for markdownlint is here: `.\.markdownlint-cli2.yaml`.
Run markdownlint using the markdownlint-cli2:

```powershell
npx.cmd --yes markdownlint-cli2 --fix --config ".markdownlint-cli2.yaml"  "**\*.md"
```

### Testing Source Code

* The library code should be tested using PlatformIO any time the code is modified.
* Use Windows PowerShell commands - not Bash or Linux shell commands - to obtain build and test output.
* When running PlatformIO, always assume that it is not in the PATH and call it using its full path: `C:\platformio\penv\Scripts\platformio.exe`.
If this path is invalid or PlatformIO cannot be located there, report this clearly rather than attempting alternate installations or silently skipping tests.
* PlatformIO environments for testing are in `./platformio.ini`.
  * Do not modify this file.
* After making a change in a library source file test compiling the library using the `mayfly` and `envirodiy_stonefly_m4` PlatformIO environments.
* The primary local test file is: `tests/logger_test/logger_test.cpp`
  * Within `tests/logger_test/logger_test.cpp`, what code is run can be configured using: `tests/logger_test/build_list.h`.
    * Modify the `tests/logger_test/build_list.h` file to select which tests to run.
    * The `tests/logger_test/build_list.h` can be modified without any concern for preserving dirty edits to the file.
    * The default tests that should always be enabled are:
      * `BUILD_MODEM_SIM_COM_SIM7080`
      * `BUILD_SENSOR_EVERLIGHT_ALSPT19`
      * `BUILD_SENSOR_SENSIRION_SHT4X`
      * `BUILD_PUB_MONITOR_MY_WATERSHED_PUBLISHER`
      * `BUILD_SENSOR_PROCESSOR_STATS`
* If example source code (.ino, .cpp, .h) is modified, compile that example.

An example simple local script for testing is (replace `tests/logger_test` with the directory of the file to test):

```powershell
# Compile one one example in one environment
$env:PLATFORMIO_CORE_DIR = "C:\platformio"
$env:PLATFORMIO_SRC_DIR = "tests/logger_test"
C:\platformio\penv\Scripts\platformio.exe ci --project-conf platformio.ini -e=mayfly -e=envirodiy_stonefly_m4
```

After testing, review compiler warnings and errors and correct problems they reveal.

### Testing Documentation Completeness

* To test documentation completeness, run the batch script `.\runDoxygen.bat`
* If `.\runDoxygen.bat` exits with a non-zero exit code, check the log for the step that failed in `.\docs\logs` for the source of the error.
* The logs for the documentation build in `.\docs\logs` do not need to be reviewed if the documentation build succeeds.
* The `keywords.txt` file is generated by the documentation build process and does not need to be reviewed.  Dirty edits to the `keywords.txt` file never need to be preserved.

---

## Conducting Reviews

When conducting reviews prioritize:

1. correctness
2. API compatibility
3. memory usage
4. embedded reliability

Also check for these things:

* Review for all text for grammar, spelling, and clarity.
* Ensure that Doxygen comments are clear and follow Doxygen conventions, using java-style @commands rather than Qt-style \commands.
* Check C++ and Arduino code snippets as within markdown and documentation as C++ code.
* Check the change log (ChangeLog.md) for completeness and accuracy of the changelog entries.

Ignore these:

* Credentials that are removed by clean filters in the filters directory do not need to be called out for removal when reviewing local code.

---

<!-- cSpell:ignore RTTI -->
