#include "Renderer.h"
#include <iostream>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	/*triangle = Mesh::GenerateTriangle();
	
	texture[0] = SOIL_load_OGL_texture(TEXTUREDIR "brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, GL_LINEAR_MIPMAP_NEAREST);
	texture[1] = SOIL_load_OGL_texture(TEXTUREDIR "stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (!texture[0] || !texture[1]) { return; }

	matrixShader = new Shader("MatrixVertex.glsl", "colourFragment.glsl");
	if (!matrixShader -> LoadSuccess()) {
		return;}

	init = true;
	SwitchToOrthographic();

	init = true;
	shader = new Shader("TexturedVertex.glsl", "texturedfragment.glsl");
		if (!shader -> LoadSuccess()) {
			return;
		}
	filtering = true;
	repeating = false;
	init = true;*/

	meshes[1] = Mesh::GenerateTriangle();
	meshes[0] = Mesh::GenerateQuad();

	
	textures[0] = SOIL_load_OGL_texture(TEXTUREDIR "brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[1] = SOIL_load_OGL_texture(TEXTUREDIR "stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	if (!textures[0] || !textures[1]) {
		return;
	}

	positions[0] = Vector3(0, 0, -5); //5 units away from the viewpoint
	positions[1] = Vector3(0, 0, -5);

	shader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	
	if (!shader -> LoadSuccess()) {
		return;
	}

	usingDepth = false;
	usingAlpha = false;
	blendMode = 0;
	modifyObject = true;
	DepthMode = -1;

	projMatrix = Matrix4::Perspective(1.0f, 100.0f, (float)width / (float)height, 45.0f);
	init = true;
}

Renderer::~Renderer(void) {
	/*delete triangle;
	delete matrixShader;
	delete shader;
	glDeleteTextures(2, texture);*/

	delete meshes[0];
	delete meshes[1];
	delete shader;
	glDeleteTextures(2, textures);
}

/*void Renderer::SwitchToPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
}

void Renderer::SwitchToZoomPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 30.0f);
}

void Renderer::SwitchToOrthographic() {
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f, width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}*/

void Renderer::RenderScene() {
	/*glClear(GL_COLOR_BUFFER_BIT);
	BindShader(matrixShader);
	glUniformMatrix4fv(glGetUniformLocation(matrixShader -> GetProgram(), "projMatrix"), 1, false, projMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(matrixShader -> GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);

	for (int i = 0; i < 3; ++i) {
		Vector3 tempPos = position;
		tempPos.z += (i * 500.0f);
		tempPos.x -= (i * 100.0f);
		tempPos.y -= (i * 100.0f);

		modelMatrix = Matrix4::Translation(tempPos) * Matrix4::Rotation(rotation, Vector3(0, 1, 0)) * Matrix4::Scale(Vector3(scale, scale, scale));

		glUniformMatrix4fv(glGetUniformLocation(matrixShader -> GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
		triangle->Draw();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader -> GetProgram(), "diffuseTex1"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex2"), 1);
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	triangle -> Draw();*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader -> GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	for (unsigned int i = 0; i < 2; ++i) {
		glUniformMatrix4fv(glGetUniformLocation( shader -> GetProgram(), "modelMatrix"), 1, false, (float*)&Matrix4::Translation(positions[i]));
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		meshes[i] -> Draw();
	}
}


/*void Renderer::UpdateSceneCamr(float dt) {
	camera = new Camera();
	camera -> UpdateCamera(dt);
	viewMatrix = camera -> BuildViewMatrix();
}

void Renderer::UpdateTextureMatrix(float value) {
	Matrix4 push = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 pop = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pop * rotation * push;
}

void Renderer::ToggleRepeating() {
	repeating = !repeating;
	SetTextureRepeating(texture[1], repeating);
	SetTextureRepeating(texture[0], repeating);
}

void Renderer::ToggleFiltering() {
	filtering = !filtering;
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}*/

void Renderer::ToggleObject() {
	modifyObject = !modifyObject;
}

void Renderer::MoveObject(float by) {
	positions[(int)modifyObject].z += by;
}

void Renderer::ToggleBlendMode() {
	blendMode = (blendMode + 1) % 4;
	switch (blendMode) {
		case (0): glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
		case (1): glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR); break;
		case (2): glBlendFunc(GL_ONE, GL_ZERO); break;
		case (3): glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
	};
	std::cout << "Blend Mode: " << blendMode << std::endl;
}

void Renderer::ToggleDepth() {
	usingDepth = !usingDepth;
	usingDepth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	std::cout << "Depth: " << usingDepth << std::endl;
}

void Renderer::ToggleAlphaBlend() {
	usingAlpha = !usingAlpha;
	usingAlpha ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	std::cout << "Alpha: " << usingAlpha << std::endl;
}

void Renderer::ToggleDepthMask() {
	usingDepthMask = !usingDepthMask;
	glDepthMask(usingDepthMask);
	std::cout << "Depth Mask: " << usingDepthMask << std::endl;
	std::cout << "Depth Mode: " << DepthMode << std::endl;
}

void Renderer::ToggleDepthFunc() {
	DepthMode = (DepthMode + 1) % 8;
	switch (DepthMode)
	{
	case 0:	glDepthFunc(GL_NEVER); break;
	case 1:	glDepthFunc(GL_LESS); break;
	case 2:	glDepthFunc(GL_EQUAL); break;
	case 3:	glDepthFunc(GL_LEQUAL); break;
	case 4:	glDepthFunc(GL_GREATER); break;
	case 5:	glDepthFunc(GL_NOTEQUAL); break;
	case 6:	glDepthFunc(GL_GEQUAL); break;
	case 7:	glDepthFunc(GL_ALWAYS); break;
	}
	std::cout << "Depth Mode: " << DepthMode << std::endl;
}