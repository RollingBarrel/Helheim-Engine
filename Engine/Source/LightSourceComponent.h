#include "Component.h"
#include <list>

typedef struct PointLight {
	float pos[4] = {0.f, 0.f, 0.f, 1.f}; //w is radius
	float col[4] = {1.f, 1.f, 1.f, 1.2f}; //w is Intensity
	//PointLight(const PointLight& other)  = delete;
}PointLight;

class LightSourceComponent : public Component {

	LightSourceComponent(GameObject* owner, std::list<PointLight>::iterator light);
	~LightSourceComponent();

private:
	std::list<PointLight>::iterator mData;
};