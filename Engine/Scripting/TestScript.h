#pragma once
#ifdef SCRIPTING_EXPORTS
#define SCRIPTING_API __declspec(dllexport)
#else
#define SCRIPTING_API __declspec(dllimport)
#endif

#include "Script.h"


extern "C" class SCRIPTING_API TestScript : public Script {

public:

    TestScript();
    ~TestScript() {}
    void Start() override;
    void Update() override;

    char h[99999999];

private:

    int num = 0;

};


extern "C" SCRIPTING_API Script* CreateTestScript() { return new TestScript(); }

//extern "C" SCRIPTING_API void fibonacci_init2(const unsigned long long a, const unsigned long long b);
