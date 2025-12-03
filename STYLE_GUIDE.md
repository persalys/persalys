# Persalys C++ Style Guide

This document consolidates and clarifies the Persalys coding style, based on existing code and previous guides. When in doubt, follow the style of nearby code and the rules below.

## 1. Scope and Goals

- Ensure internal consistency across C++ core, Qt UI, CMake and Python bindings.
- Match the established patterns from the current code base and OpenTURNS.
- Keep code easy to read, maintain and review.

The guide focuses on C++/Qt and OpenTURNS integration, with short notes on CMake and Python.

---

## 2. Files, Layout and Includes

- **File extensions**
  - Public headers: `.hxx` (typically in `lib/include/persalys`).
  - Implementation: `.cxx` (typically in `lib/src/**`).
  - One main class per header/source pair when possible (`Foo.hxx` / `Foo.cxx`).

- **Header guards**
  - Use classic include guards in all headers.
  - Pattern: `PERSALYS_<UPPER_SNAKE_NAME>_HXX`.
  - Example:
    ```cpp
    #ifndef PERSALYS_STUDY_HXX
    #define PERSALYS_STUDY_HXX
    ...
    #endif
    ```

- **File header / license**
  - Every `.hxx` and `.cxx` file starts with:
    - Emacs mode line: `//                                               -*- C++ -*-`
    - Doxygen brief block documenting the file/class.
    - Standard LGPL license header (copy from a recent file such as `Study.hxx`).

- **Include order (in .cxx)**
  1. Matching Persalys header (with `persalys/` prefix when public), e.g. `#include "persalys/MainWidget.hxx"`.
  2. Other Persalys headers.
  3. Third‑party headers in logical groups: OpenTURNS, Qt, standard library, other libs.
  4. Separate groups with a blank line.

- **Include usage**
  - Headers should include only what they strictly need; prefer forward declarations to reduce dependencies.
  - Move heavy includes (Qt widgets, STL containers, OpenTURNS algorithms…) to `.cxx` whenever possible.

---

## 3. Namespaces and Modules

- All Persalys code lives in the `PERSALYS` namespace:
  ```cpp
  namespace PERSALYS
  {
  ...
  }
  ```

- OpenTURNS types and functions use the `OT` namespace.
  - In source files where OT is heavily used, it is common to add:
    ```cpp
    using namespace OT;
    ```
  - Avoid `using namespace OT;` in public headers; qualify types (`OT::String`, `OT::Sample`, …) instead.

- Do not introduce new top‑level namespaces; extend `PERSALYS` and reuse existing sub‑directories to structure modules (`base`, `model`, `view`, …).

---

## 4. Naming Conventions

- **Namespaces**: `PERSALYS`, `OT` (OpenTURNS), etc. – UPPER_CASE for Persalys.

- **Classes, structs, enums**: PascalCase.
  - Examples: `Study`, `StudyImplementation`, `MainWidget`, `DataModelTableModel`.

- **Methods and free functions**: lowerCamelCase.
  - Examples: `buildInterface`, `getAvailableAnalysisName`, `hasDataModelNamed`.

- **Data members**: lowerCamelCase with trailing underscore.
  - Examples: `modified_`, `dataModel_`, `graphSettingDockWidget_`.

- **Local variables and parameters**: lowerCamelCase.
  - Examples: `studyName`, `graph`, `errorMessage`.

- **Booleans**: start with `is`, `has`, `can`, etc., when they represent predicates.
  - Examples: `isRunning_`, `hasPhysicalModelNamed`.

- **Constants and macros**:
  - Macros and compile‑time options: ALL_CAPS with underscores (`PERSALYS_HAVE_PARAVIEW`).
  - Enum values: Prefer PascalCase; keep acronyms in upper‑case when present (`PDF`, `CDF`). Match nearby enums for consistency.

- **Qt members**: keep the Qt type as part of the name where it helps clarity.
  - Examples: `graphSettingDockWidget_`, `subWindowsStackedWidget_`, `studyTree_`.

---

## 5. Formatting and Style

The style is close to OpenTURNS.

- **Indentation**
  - Use 2 spaces per indentation level.
  - Do not use tabs.

- **Braces (Allman style)**
  - Opening brace on a new line for classes, functions and control flow:
    ```cpp
    void MainWidget::buildInterface()
    {
      if (condition)
      {
        ...
      }
      else
      {
        ...
      }
    }
    ```

- **Spaces**
  - Around binary operators: `a + b`, `i < n`.
  - No space before `(` in function calls: `foo(bar)`.
  - In declarations, follow the predominant existing pattern and keep `&` / `*` close to the type for references and pointers:
    ```cpp
    void setName(const OT::String& name);
    StudyImplementation* clone() const;
    ```

- **Line length**
  - Aim for 80–100 characters; break lines when readability suffers.

- **Blank lines**
  - Separate logical blocks with a blank line.
  - Leave one blank line between method definitions.

- **Member initializer lists**
  - Use member initializer lists for constructors and put each member on its own line:
    ```cpp
    MainWidget::MainWidget(QWidget* parent)
      : QWidget(parent)
      , studyTree_(new StudyTreeView(this))
      , subWindowsStackedWidget_(new SubWindowsStackedWidget(this))
      , graphSettingDockWidget_(new QDockWidget(this))
      , actions_(new Actions(this))
    {
      buildInterface();
    }
    ```
  - When possible, prefer initializing member variables in the header.
  ```cpp
  class PERSALYS_VIEW_API CouplingResourceFileWidget : public QWidget
  {
    Q_OBJECT

  public:
    CouplingResourceFileWidget(CouplingPhysicalModel * model, 
      const int indStep, 
      QWidget *parent = nullptr
    );

  private:
    CouplingPhysicalModel * model_        = nullptr;
    int                   indStep_        = 0;
    QTableWidget          * tableWidget_  = nullptr;
  };```

- **Null pointers**
  - Use `nullptr` in new or modified code, except when constrained by third‑party APIs expecting `0` / `NULL`.

---

## 6. Classes, Inheritance and OpenTURNS Integration

- **Export macros**
  - Use the appropriate export macro on public classes in headers (see `PersalysPrivate.hxx`):
    - `PERSALYS_BASE_API`, `PERSALYS_MODEL_API`, `PERSALYS_VIEW_API`, …
  - Use the corresponding `_LOCAL` macro for non‑exported internal classes when needed.

- **OpenTURNS persistent objects**
  - For OpenTURNS‑style persistent objects, follow the existing pattern (`StudyImplementation`, analyses, results…):
    - In the header:
      ```cpp
      class PERSALYS_BASE_API StudyImplementation
        : public OT::PersistentObject
        , public Observable
      {
        CLASSNAME
        ...
      };
      ```
    - In the source:
      ```cpp
      using namespace OT;

      namespace PERSALYS
      {

      CLASSNAMEINIT(StudyImplementation)

      static Factory<StudyImplementation> Factory_StudyImplementation;
      ```

  - Implement `clone()` as a virtual constructor returning `new ClassName(*this);`.

- **Interface / implementation split**
  - High‑level handles such as `Study` are typically `OT::TypedInterfaceObject<Implementation>` wrappers.
  - When adding new such classes, mirror the existing split:
    - `FooImplementation` derives from `OT::PersistentObject` (and possibly `Observable` / `Observer`).
    - `Foo` wraps `FooImplementation` and exposes the public API.

- **Method ordering**
  - In headers: group `public`, `protected`, then `private` members.
  - In sources: keep method definitions in the same order as in the header.
  - Group related getters/setters and related operations (e.g. `add`, `remove`, `has...`, `getAvailable...`).

---

## 7. Qt and UI Code

- **QObject classes**
  - Add `Q_OBJECT` in the class definition when using signals/slots or Qt meta‑object features.
  - Use `signals`, `public slots`, `protected slots` sections explicitly.

- **Ownership**
  - Use Qt parent ownership consistently: pass the parent pointer to child widgets (`new Widget(this)`), so manual deletion is generally not needed.

- **Models and views**
  - For `QAbstractItemModel` subclasses, implement at least `rowCount`, `columnCount`, `data`, `setData`, `flags`, and `headerData` as required, following `DataModelTableModel` as reference.
  - Use `beginResetModel()` / `endResetModel()` when resetting data.

- **Signals and slots syntax**
  - Follow the syntax already used in the file (Qt5/Qt6 style with function pointers or lambda connections). Prefer the newer syntax for new code.

- **Translations / i18n**
  - Wrap user‑visible UI strings in `tr("...")`.
  - Do not build long sentences by concatenation; prefer formatted strings or smaller translatable units when possible.

- **Styling**
  - Reuse centralized colors and styles (e.g. `ApplicationColor[...]`) instead of hardcoding colors widely.
  - Apply styles at container level when possible using `setStyleSheet`.

---

## 8. Error Handling, Logging and Messages

- **Exceptions**
  - Prefer OpenTURNS exceptions for logic errors and invalid arguments:
    ```cpp
    throw InvalidArgumentException(HERE)
      << "The given name " << dataModelName
      << " does not correspond to a data model of the study.";
    ```

- **Pre‑condition checks**
  - Use `hasXxxNamed` or `canBeLaunched`‑style helpers to validate state before running expensive operations.

- **Logging and diagnostics**
  - Prefer OpenTURNS logging facilities (e.g. `OT::LOGDEBUG`) over `std::cerr` in new code.
  - Keep user‑facing messages translatable and concise.

---

## 9. Python Integration and Scripts

- The `persalys` Python module is generated via SWIG and mirrors the C++ API.
  - Public class and method names remain in PascalCase / camelCase for API consistency with C++ and OpenTURNS.

- **Python tests**
  - Live under `python/test/`, with names `t_*.py` (often suffixed by `_std`).
  - Use `openturns.testing.assert_almost_equal` for numerical comparisons.
  - Use deterministic seeds (`ot.RandomGenerator.SetSeed(...)`).

- When generating Python scripts from C++ (e.g. `getPythonScript` methods), reuse the existing helpers (`OSS`, `Parameters::GetOT*Str(...)`, etc.) and return `OT::String`.

---

## 10. CMake and Build System

- Minimum required CMake version is set at the top level; respect and do not lower it.

- **General style**
  - Follow spacing and casing already present in nearby CMakeLists (both upper/lowercase commands exist; keep consistency within a file).
  - Use project‑scoped variables with `PERSALYS_` prefix (`PERSALYS_DEFINITIONS`, `PERSALYS_INCLUDE_PATH`, …).

- **Options and configuration**
  - Options use upper‑snake‑case names like `BUILD_GUI`, `USE_PARAVIEW` with values `ON` / `OFF`.
  - Use `message(STATUS ...)` for informative messages and `message(FATAL_ERROR ...)` for missing critical dependencies.

- **Installation and packaging**
  - Use `GNUInstallDirs` and existing macros (`ot_add_source_file`, `ot_install_header_file`, …) rather than re‑inventing installation logic.

---

## 11. Tests and Contributions

- C++ tests live under `lib/test`. When adding new public features, extend the closest existing test or add a new one beside similar tests.

- When modifying a file:
  - Match the style already used in that file (even if slightly inconsistent with this guide) unless you perform an explicit local cleanup.
  - Keep changes focused and avoid large, unrelated refactors.

- For new features exposed to Python:
  - Ensure at least one Python test covers the new functionality.
  - Keep random behavior deterministic in tests.

---

## 12. General Principles

- Prefer clarity over cleverness; simple, explicit code is easier to maintain.
- Favor consistency with existing patterns, especially for OpenTURNS integration.
- When in doubt about style, look at similar classes in the same module (e.g. other analyses, widgets, or models) and mirror them.
