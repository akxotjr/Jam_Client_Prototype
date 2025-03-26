#pragma once

#define DECLARE_SINGLETON(ClassName)                                    \
public:                                                                 \
    ClassName() = default;                                              \
    ~ClassName() = default;                                             \
    ClassName(const ClassName&) = delete;                               \
    ClassName& operator=(const ClassName&) = delete;                    \
    static ClassName* GetInstance()                                     \
    {                                                                   \
        static std::shared_ptr<ClassName> instance =                    \
            std::make_shared<ClassName>();                              \
        return instance.get();                                          \
    }                                                                   \
private:                                                                \
    static std::shared_ptr<ClassName> instance;