# C++ Language Guidelines

## Language Standard

- **C++20** strictly required
- All C++17 and C++20 features are available

## Type Preferences

- **Smart pointers:** Prefer `std::unique_ptr`, `std::shared_ptr`
- **Enum classes:** Preferred over C-style enums (implicit conversions are dangerous)
- **C++20 types:** Prefer over Boost counterparts
- **Boost libraries:** Try to limit libraries requiring linking (prefer C++20 standard)

## Best Practices

- **Classes:** Mark non-derived classes as `final`
- **Exceptions:** Use and derive from `std::exception`
- **Legacy code:** Avoid Yuni framework for new code (legacy dependency)
- **Threading:** Avoid global/static variables (threading issues)
- **TODO comments:** Format as `// TODO(DeveloperName): need to fix it`

## Testing Requirements

- Write unit tests covering new functionality
- See examples in `src/tests/libs/`
- Code must compile without warnings on clang++, g++, MSVC
