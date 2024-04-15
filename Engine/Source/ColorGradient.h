#pragma once

class ColorGradient
{
public:
	ColorGradient();
	ColorGradient(const ColorGradient& orig);
	~ColorGradient();

	void addColor(float time, const float4& color);
	float4 getColor(float time) const;

private:
	struct ColorKey
	{
		float time;
		float4 color;
	};

	std::vector<ColorKey> mColors;
};
