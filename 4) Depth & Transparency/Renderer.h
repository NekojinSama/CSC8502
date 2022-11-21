#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	
	/*void SwitchToPerspective();
	void SwitchToOrthographic();
	void SwitchToZoomPerspective();

	inline void SetScale(float s) { scale = s; }
	inline void SetRotation(float r) { rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; }
	virtual void UpdateSceneCamr(float dt);

	void UpdateTextureMatrix(float rotation);
	void ToggleRepeating();
	void ToggleFiltering();*/

	void ToggleObject();
	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	void ToggleDepthMask();
	void MoveObject(float by);
	void ToggleDepthFunc();
	
protected:
	/*Mesh* triangle;
	Shader * matrixShader;
	float scale;
	float rotation;
	Vector3 position;
	Camera* camera;

	Shader* shader;
	GLuint texture[2];
	bool filtering;
	bool repeating;*/

	GLuint textures[2];
	Mesh * meshes[2];
	Shader * shader;
	Vector3 positions[2];

	bool modifyObject;
	bool usingDepth;
	bool usingAlpha;
	bool usingDepthMask;
	int blendMode;
	int DepthMode;
};


