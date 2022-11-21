#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/CubeRobot.h"
#include "../nclgl/Frustum.h"

class HeightMap;
class Light;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void UpdateScene(float dt) override;
	void RenderScene() override;

	void SwitchToPerspective();
	void SwitchToOrthographic();

	void ToggleObject();
	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	void MoveObject(float by);

	void UpdateTextureMatrix(float rotation);
	void ToggleRepeating();
	void ToggleFiltering();
	void ToggleDepthBuffer();
	void ToggleDepthBufferMode();

	void UpdateColours(float dt);

	SceneNode* GetRoot() { return root; }

	void UpdateTime(float dt);

protected:
	void DrawNode(SceneNode* n);
	void DrawNode(SceneNode* n, int xPos);
	void DrawNode(SceneNode* n, int xPos, int scale);

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();

	void DrawHeightMap();
	void DrawWater();
	void DrawSkybox();

	void DrawShadowScene();
	void DrawMainScene();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;

	HeightMap* heightMap;
	Light* light;

	GLuint texture;
	GLuint bumpmap;
	GLuint specmap;
	GLuint cubeMap;
	GLuint waterTex;
	GLuint earthTex;
	GLuint earthBump;
	float waterRotate;
	float waterCycle;

	SceneNode* root;
	Camera* camera;
	Mesh* cube;
	Mesh* quad;
	Shader* shader;

	Mesh* meshes[2];
	GLuint textures[2];
	Vector3 positions[2];

	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	bool modifyObject;
	bool usingDepth;
	bool usingAlpha;
	int blendMode;
	int depthMode;
	bool depthBuff;

	bool filtering;
	bool repeating;

	float time;
	bool oscillate;

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint sceneDiffuse;
	GLuint sceneBump;
	float sceneTime;

	Shader* sceneShader;
	Shader* shadowShader;

	vector<Mesh*> sceneMeshes;
	vector<Matrix4> sceneTransforms;

};