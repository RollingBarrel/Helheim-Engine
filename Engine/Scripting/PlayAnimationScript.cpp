#include "PlayAnimationScript.h"
#include "GameObject.h"
#include "AnimationComponent.h"


CREATE(PlayAnimationScript)
{
    CLASS(owner);
    SEPARATOR("PLAYS ANIMATION OF GAMEOBJECT ON START");
    END_CREATE;
}

void PlayAnimationScript::Start()
{
	AnimationComponent* animComp = static_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
	animComp->SetIsPlaying(true);
}
