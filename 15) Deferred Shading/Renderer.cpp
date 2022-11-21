#include "Renderer.h"
#include "../nclgl/HeightMap.h"
#include<algorithm>

#define SHADOWSIZE 2048

const int LIGHT_NUM = 32;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR"noise.png");

	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);

	camera = new Camera(-45.0f, 0.0f, Vector3());

	Vector3 dimensions = heightMap->GetHeightmapSize();
	camera->SetPosition(dimensions * Vector3(0.5f, 5.0f, 0.5f));

	pointLights= new Light[LIGHT_NUM];

	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
		l.SetPosition(Vector3(rand() % (int)dimensions.x, 150.0f, rand() % (int)dimensions.z));
		l.SetColour(Vector4(0.5f+(float)(rand()/(float)RAND_MAX), 0.5f + (float)(rand() / (float)RAND_MAX), 0.5f + (float)(rand() / (float)RAND_MAX),1));
		l.SetRadius(dimensions.x*0.2 + (rand()%250));
	}

	sceneShader = new Shader("BumpVertex-TUT15.glsl", "bufferFragment.glsl");
	pointLightShader= new Shader("pointLightVert.glsl", "pointLightFrag.glsl");
	combineShader = new Shader("combiningVert.glsl", "combineFrag.glsl");




	if (!sceneShader->LoadSuccess() || !pointLightShader->LoadSuccess() || !combineShader->LoadSuccess()) {
		return;
	}

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	GLenum buffers[2] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);



	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex , 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);


	init = true;

}


Renderer::~Renderer(void) {

	

	delete camera;
	delete sceneShader;
	delete combineShader;
	delete pointLightShader;
	delete heightMap;
	delete sphere;
	delete quad;
	delete[] pointLights;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteTextures(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);

}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	/*sceneTime += dt;

	for (int i = 1; i < 4; ++i) {
		Vector3 t = Vector3(-10 + (5 * i), 2.0f + sin(sceneTime * i), 0);
		sceneTransforms[i] = Matrix4::Translation(t) * Matrix4::Rotation(sceneTime * 10 * i, Vector3(1, 0, 0));
	}*/


	/*viewMatrix = camera->BuildViewMatrix();

	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.25f;*/

	/*if (oscillate) {
		time += 10.0f * cos(dt);
		oscillate = time > 1000 ? false : true;
	}

	else {
		time -= 10.0f * cos(dt);
		oscillate = time < -1000 ? true : false;
	}*/
	//frameFrustum.FromMatrix(projMatrix * viewMatrix);
	//root->Update(dt);
}

//void Renderer::DrawShadowScene() {
//	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
//
//	glClear(GL_DEPTH_BUFFER_BIT);
//	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
//	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
//
//	BindShader(shadowShader);
//
//	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
//
//	projMatrix = Matrix4::Perspective(1, 100, 1, 45);
//	shadowMatrix = projMatrix * viewMatrix;
//
//	for (int i = 0; i < 4; ++i) {
//		modelMatrix = sceneTransforms[i];
//		UpdateShaderMatrices();
//		sceneMeshes[i]->Draw();
//	}
//
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//	glViewport(0, 0, width, height);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//}
//
//
//void Renderer::DrawMainScene() {
//	BindShader(sceneShader);
//	SetShaderLight(*light);
//	viewMatrix = camera->BuildViewMatrix();
//	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
//		(float)width / (float)height, 45.0f);
//
//	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
//	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);
//	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "shadowTex"), 2);
//
//	glUniform3fv(glGetUniformLocation(sceneShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, sceneDiffuse);
//
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, sceneBump);
//
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, shadowTex);
//
//	for (int i = 0; i < 4; i++) {
//		modelMatrix = sceneTransforms[i];
//		UpdateShaderMatrices();
//		sceneMeshes[i]->Draw();
//	}
//
//
//}


void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(sceneShader);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	UpdateShaderMatrices();
	heightMap->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::DrawPointLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointLightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(pointLightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	
	glUniform1i(glGetUniformLocation(pointLightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(pointLightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform2f(glGetUniformLocation(pointLightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(pointLightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();
	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	
	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::RenderScene() {

	//BuildNodeLists(root);
	//SortNodeLists();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	FillBuffers();
	DrawPointLights();
	CombineBuffers();
}

void Renderer::CombineBuffers() {
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	quad->Draw();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightMap() {
	BindShader(lightShader);


	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

}


void Renderer::DrawWater() {

	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 3);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = heightMap->GetHeightmapSize();

	modelMatrix = Matrix4::Translation(hSize * 0.5f) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(5, 5, 5)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));
	UpdateShaderMatrices();

	quad->Draw();
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));
		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}
	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); i++) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);

}
void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::UpdateTime(float dt)
{
	time = 100.0f * sinf(dt);
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
		textures[1] = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[1]);

		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"), textures[1]);
		n->Draw(*this);
	}
}

//
void Renderer::DrawNode(SceneNode* n, int xPos) {
	if (n->GetMesh()) {
		Matrix4 model = Matrix4::Translation(Vector3(xPos, 0, 0)) * n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"), 0);
		n->Draw(*this);
	}

	for (auto i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); i++) {
		DrawNode(*i, xPos);
	}
}

void Renderer::DrawNode(SceneNode* n, int xPos, int scale) {
	if (n->GetMesh()) {
		Matrix4 model = Matrix4::Translation(Vector3(xPos, 0, 0)) * n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale() + Vector3(scale, scale, scale));
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"), 0);
		n->Draw(*this);
	}

	for (auto i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); i++) {
		DrawNode(*i, xPos, scale);
	}
}

void Renderer::UpdateColours(float dt) {
	meshes[0]->UpdateColours(int(dt) % 10);
}


void Renderer::UpdateTextureMatrix(float value) {
	Matrix4 push = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 pop = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pop * rotation * push;
}

void Renderer::ToggleRepeating() {
	repeating = !repeating;
	SetTextureRepeating(textures[0], repeating);
}

void Renderer::ToggleFiltering() {
	filtering = !filtering;
	SetTextureFiltering(textures[0], filtering);
}


void Renderer::ToggleObject() {
	modifyObject = !modifyObject;
}

void Renderer::MoveObject(float by) {
	positions[(int)modifyObject].z += by;
}

//void Renderer::ToggleBlendMode() {
//	blendMode = (blendMode + 1) % 4;
//	switch (blendMode) {
//	case(0):glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
//	case(1):glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR); break;
//	case(2):glBlendFunc(GL_ONE, GL_ZERO); break;
//	case(3):glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
//	}
//	std::cout << "mode::" << blendMode << "\n";
//}

void Renderer::ToggleDepthBuffer() {
	depthBuff = !depthBuff;
	glDepthMask(depthBuff);
}

void Renderer::ToggleDepthBufferMode() {
	depthMode = (depthMode + 1) % 8;
	switch (depthMode) {
	case 0: glDepthFunc(GL_NEVER); break;
	case 1: glDepthFunc(GL_LESS); break;
	case 2: glDepthFunc(GL_EQUAL); break;
	case 3: glDepthFunc(GL_LEQUAL); break;
	case 4: glDepthFunc(GL_GREATER); break;
	case 5: glDepthFunc(GL_NOTEQUAL); break;
	case 6: glDepthFunc(GL_GEQUAL); break;
	case 7: glDepthFunc(GL_ALWAYS); break;

	}

	std::cout << "Depth test:" << depthMode << "\n";
}

void Renderer::ToggleDepth() {
	usingDepth = !usingDepth;
	usingDepth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void Renderer::ToggleAlphaBlend() {
	usingAlpha = !usingAlpha;
	usingAlpha ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void Renderer::SwitchToOrthographic() {
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,
		width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}

void Renderer::SwitchToPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
}


