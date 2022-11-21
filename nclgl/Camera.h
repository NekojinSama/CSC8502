#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Camera {
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
	};

	Camera(float pitch, float yaw, Vector3 position) {
		this -> pitch = pitch;
		this -> yaw = yaw;
		this -> position = position;
        this->trackDegree = 0;
        trackDegree = 0;
        timeElapsed = 0;
	}

	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }
	
	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }
	
	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }

    void cameraTrack(float dt, float timePassed) {
        float firstScene = 30, secondScene = 34, thirdScene = 50;
        trackDegree += dt / 10;
        timeElapsed = (int)(timePassed*100)/100 % 50;
        if (trackDegree > 2 * PI) { trackDegree -= 2 * PI; };
        float x = 0, z = 0;

        if (timeElapsed < firstScene) {
            x = 4000 * cos(trackDegree);
            z = 4000 * sin(trackDegree);
            this->SetPosition(Vector3(x + 4000, 3000, z + 2500));
            this->SetYaw(90 - trackDegree * 180 / PI);
            this->SetPitch(-20);
        }

        if (timeElapsed >= firstScene && timeElapsed < secondScene) {
            x = 1000 * sin(trackDegree);
            z = 1000 * cos(trackDegree);
            this->SetPosition(Vector3(x + 1500, 3200, z + 4665));
            this->SetYaw(90 - trackDegree * 180 / PI);
            this->SetPitch(-5);
        }

        if (timeElapsed >= secondScene && timeElapsed < thirdScene) {
            x = 2518.0f - dt;
            z = 2803.0f;
            this->SetPosition(Vector3(x, 2840, z));
            this->SetYaw(90 - trackDegree * 180 / PI);
            this->SetPitch(-30);
        }
        if (timeElapsed >= thirdScene) {
            trackDegree = 0.0f;
        }
    }
	
protected:
	float yaw;
	float pitch;
    float trackDegree;
    float timeElapsed;
	Vector3 position; // Set to 0 ,0 ,0 by Vector3 constructor ;)
};