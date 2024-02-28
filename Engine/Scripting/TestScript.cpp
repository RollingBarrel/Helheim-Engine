#include "pch.h" 
#include <utility>
#include <limits.h>
#include "TestScript.h"

#include "Globals.h"


TestScript::TestScript() : Script()
{
}

void TestScript::Start()
{
   
}

void TestScript::Update()
{
	num++;
   LOG("hola %d" , num);
}

