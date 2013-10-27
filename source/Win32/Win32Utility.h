#pragma once

#include <assert.h>

#define CheckMemory() {                                  \
        int flag=  _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);  \
        flag |= _CRTDBG_LEAK_CHECK_DF;                   \
        flag &= ~_CRTDBG_CHECK_CRT_DF;                   \
        _CrtSetDbgFlag(flag);                            \
    }    

void LOG(const char* format, ...);

template <typename T>
class TSingleton
{
protected:
    static T *ms_singleton;

public:
	TSingleton() {ms_singleton = ((T *)this);}
	virtual ~TSingleton() {assert(ms_singleton); ms_singleton = 0;}

private:
	TSingleton(const TSingleton& cpy) {}
	TSingleton &operator = (const TSingleton& rhs) {return *this;}
};

#define SINGLETON_DECLARE(name)                       \
    public:                                           \
    static name& getSingleton(void);                  \
    static name* getSingletonPtr(void);

#define SINGLETON_IMPLEMENT(name)                       \
    template<> name* TSingleton<name>::ms_singleton= 0; \
    name& name::getSingleton(void) {                    \
        assert(ms_singleton);                           \
        return *ms_singleton;                           \
    }                                                   \
    name* name::getSingletonPtr(void){                  \
        return ms_singleton;                            \
    }