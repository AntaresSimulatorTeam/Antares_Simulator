# Error Handling & Logging

## Logging Levels

Use `Antares::logs` with appropriate levels:

```cpp
Antares::logs.error()   // Errors causing simulation to stop
Antares::logs.warning() // Non-critical issues, simulation continues
Antares::logs.info()    // Information for users
Antares::logs.debug()   // Debugging info for developers
```

**Important:** DO NOT use `logs.fatal()` - use `logs.error()` instead.

## Exception Handling

Define custom exceptions derived from `std::exception`:

```cpp
class MyException : public std::exception {
    const char* what() const noexcept override {
        return "Description of error";
    }
};
```
