#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();
	
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
	init = true;
}

Renderer::~Renderer(void) {
	delete triangle;
	delete matrixShader;
	delete shader;
	glDeleteTextures(2, texture);
}

void Renderer::SwitchToPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
}

void Renderer::SwitchToZoomPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 30.0f);
}

void Renderer::SwitchToOrthographic() {
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f, width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT);
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
	triangle -> Draw();
}


void Renderer::UpdateSceneCamr(float dt) {
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
}
