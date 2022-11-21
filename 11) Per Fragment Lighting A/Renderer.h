#pragma once

#include "../nclgl/OGLRenderer.h"

class Camera;
class Mesh;
class HeightMap;
class Light;
class MeshAnimation;
class MeshMaterial;

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	HeightMap* heightMap;
	Camera * camera;
	Mesh * mesh;
	Shader * shader;
	MeshAnimation * anim;
	MeshMaterial * material;
	Light* light;
	GLuint texture;


	vector < GLuint > matTextures;
	GLuint terrainTex;

	int currentFrame;
	float frameTime;
};
