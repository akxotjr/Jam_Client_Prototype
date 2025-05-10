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

//#define DECLARE_SINGLETON(ClassType)                    \
//public:                                                 \
//    static void PreInit()                               \
//    {                                                   \
//        _instance = std::make_unique<ClassType>();      \
//    }                                                   \
//    static void Shutdown()                              \
//    {                                                   \
//        _instance.reset();                              \
//    }                                                   \
//    static ClassType& Instance()                        \
//    {                                                   \
//        return *_instance;                              \
//    }                                                   \
//protected:                                                \
//    static std::unique_ptr<ClassType> _instance;        \
//    ClassType();
//
//#define DEFINE_SINGLETON(ClassType)                     \
//    std::unique_ptr<ClassType> ClassType::_instance;

//#define DECLARE_SINGLETON(ClassType)                    \
//public:                                                 \
//    static void PreInit();                              \
//    static void Shutdown();                             \
//    static ClassType& Instance();                       \
//protected:                                               \
//    static std::unique_ptr<ClassType> _instance;        \
//    ClassType();
//
//#define DEFINE_SINGLETON(ClassType)                                 \
//    std::unique_ptr<ClassType> ClassType::_instance;               \
//    void ClassType::PreInit() { _instance = std::make_unique<ClassType>(); } \
//    void ClassType::Shutdown() { _instance.reset(); }              \
//    ClassType& ClassType::Instance() { return *_instance; }

