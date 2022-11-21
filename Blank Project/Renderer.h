#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Frustum.h"

class Camera;
class MeshAnimation;
class MeshMaterial;
class SceneNode;
class Light;
class SpotLight;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;
	void ToggleCameraMov();
	void CameraMovement(float dt, float ds);

protected:
	Camera *camera;
	Mesh* portal1mesh;
	Mesh* portal2mesh;
	Mesh* portal3mesh;
	Mesh* portal4mesh;
	Mesh* portal5mesh;

	Mesh* character;
	Light* light;
	SpotLight* spotlight;
	Light* torchlight;

	Shader *TerShader;
	Shader *shader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* AnimShader;

	MeshAnimation *anim;

	MeshMaterial* portal1mat;
	MeshMaterial* portal2mat;
	MeshMaterial* portal3mat;
	MeshMaterial* portal4mat;
	MeshMaterial* portal5mat;
	MeshMaterial* spaceMaterial;

	HeightMap *heightMap;
	GLuint terrainTex;
	GLuint tileTex;
	GLuint sandTex;
	GLuint waterTex;
	GLuint bumpmap;

	vector < GLuint > portal1tex;
	vector < GLuint > portal2tex;
	vector < GLuint > portal3tex;
	vector < GLuint > portal4tex;
	vector < GLuint > portal5tex;
	vector < GLuint > matTexturesAnim;

	void DrawHeightmap();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	void DrawSkybox();
	void DrawWater();
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();

	Frustum frameFrustum;

	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;

	SceneNode* root;
	Mesh* quad;
	Mesh* cube;
	GLuint texture;
	GLuint cubeMap;

	int currentFrame;
	float frameTime;

	float waterRotate;
	float waterCycle;

	bool val = true;
};