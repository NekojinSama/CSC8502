#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	
	void SwitchToPerspective();
	void SwitchToOrthographic();
	void SwitchToZoomPerspective();

	inline void SetScale(float s) { scale = s; }
	inline void SetRotation(float r) { rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; }
	virtual void UpdateSceneCamr(float dt);

	void UpdateTextureMatrix(float rotation);
	void ToggleRepeating();
	void ToggleFiltering();
	
protected:
	Mesh * triangle;
	Shader * matrixShader;
	float scale;
	float rotation;
	Vector3 position;
	Camera* camera;

	Shader* shader;
	GLuint texture[2];
	bool filtering;
	bool repeating;
};


