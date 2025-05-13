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
    ClassName()                                     \
	{                                               \
		std::atexit([] {                            \
			Instance().Shutdown();                  \
        });                                         \
    }                                               \
    ~ClassName() = default;                         \
    ClassName(const ClassName&) = delete;           \
    ClassName& operator=(const ClassName&) = delete;

