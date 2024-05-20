#include "PrefabTest.h"

CREATE(PrefabTest)
{
    CLASS(owner);
    END_CREATE;
}



void PrefabTest::Awake()
{
	LOG("AWAKE");

}

void PrefabTest::Start()
{
	LOG("START");

}

void PrefabTest::Update()
{
	//LOG("UPDATE");
}