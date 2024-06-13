#include "Sanity.h"

#include "GameObject.h"

CREATE(Sanity)
{
    CLASS(owner);
    SEPARATOR("Cards");
    MEMBER(MemberType::GAMEOBJECT, mCardGO1);
    MEMBER(MemberType::GAMEOBJECT, mCardGO2);
    MEMBER(MemberType::GAMEOBJECT, mCardGO3);
    END_CREATE;
}

Sanity::Sanity(GameObject* owner) : Script(owner)
{
}

Sanity::~Sanity()
{
}

void Sanity::Start()
{
    // For each card
    // Find children text
    // Find Image
    // Find Text
    // Get Buttn cmponent
}

void Sanity::Update()
{
}

// Click & Hover functions for each card

// Function to consume buff

// Function to create selection based on arena.