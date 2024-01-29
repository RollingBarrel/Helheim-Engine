#include "Component.h"
#include <list>

typedef struct PointLight {
	float pos[4]; //w is radius
	float col[4]; //w is Intensity
}PointLight;

class LightSourceComponent : public Component {

	LightSourceComponent(GameObject* owner, const PointLight& light);
	~LightSourceComponent();

private:
	std::list<PointLight>::iterator mData;
};