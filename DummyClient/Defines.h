#pragma once

/*---------------
    Singleton
---------------*/

#define DECLARE_SINGLETON(ClassName)                \
public:                                             \
    static ClassName& Instance() {                  \
        static ClassName instance;                  \
        return instance;                            \
    }                                               \
private:                                            \
    ClassName() = default;                          \
    ~ClassName() = default;                         \
    ClassName(const ClassName&) = delete;           \
    ClassName& operator=(const ClassName&) = delete;
