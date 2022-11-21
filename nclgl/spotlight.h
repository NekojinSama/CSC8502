#pragma once

#include "Vector4.h"
#include "Vector3.h"
#include <math.h>

class SpotLight {
public:
	SpotLight() {} // Default constructor , we ’ll be needing this later !
	SpotLight(const Vector3& position, const Vector4& colour, float radius, float rads, const Vector3& direction) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;
		this->rads = 0;
		this->direction = direction;
	}

	~SpotLight(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	float GetAngle() const { return rads; }
	void SetAngle(float val) { rads = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }

	Vector3 GetDirc() const { return direction; }
	void SetDirc(const Vector3& val) { direction = val; }

	/*void orbit(float dt, float hyp, float z)
	{
		rads += dt * (90.0f / 360.0f);
		float x, y;
		x = hyp * sin(rads);
		y = hyp * cos(rads);
		SetPosition(Vector3(x, y, z));
	}*/

protected:
	Vector3 position;
	float radius;
	Vector4 colour;
	float rads;
	Vector3 direction;
};
