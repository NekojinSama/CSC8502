//#include "../imgui/imgui.h"
//#include "../imgui/imgui_impl_opengl3.h"
//#include "../imgui/imgui_impl_opengl3_loader.h"


//IMGUI_CHECKVERSION();
//ImGui::CreateContext();
//ImGuiIO& io = ImGui::GetIO(); (void)io;
//ImGui::StyleColorsDark();
//ImGui_implGlfw_InitForOpenGL(window, true);
//ImGui_ImplOpenGL3_Init("#version 330");

#include "Renderer.h"
#include "../nclgl/Shader.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/SpotLight.h"
#include "../nclgl/MeshAnimation.h"
#include <algorithm>

float ini = 0.0f;

void Renderer::ToggleCameraMov() {
	val = !val;
}

void Renderer::CameraMovement(float dt,float ds) {
	if(val) { camera->cameraTrack(ds,dt); }
}

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	heightMap = new HeightMap(TEXTUREDIR "hTex/Terrain_heightmap.png");
	camera = new Camera(-40, 270, Vector3());

	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	camera->SetPosition(heightmapSize * Vector3(0.5, 2, 0.5));
	light = new Light(heightmapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), heightmapSize.x * 3.5f);
	

	TerShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	shader = new Shader("BlankVertex.glsl", "BlankFragment.glsl");
	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl ");
	if (!TerShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !shader->LoadSuccess() || !reflectShader->LoadSuccess()) {
		return;
	}

	terrainTex = SOIL_load_OGL_texture(TEXTUREDIR "hTex/Terrain_splatmap.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tileTex = SOIL_load_OGL_texture(TEXTUREDIR "hTex/albedo.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	sandTex = SOIL_load_OGL_texture(TEXTUREDIR "hTex/diffuse.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR "hTex/Vol_36_5_Base_Color.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpmap = SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg", TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg", TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	quad = Mesh::GenerateQuad();

	if (!terrainTex || !tileTex || !sandTex || !waterTex || !cubeMap) {
		return;
	}

	SetTextureRepeating(terrainTex, true);
	SetTextureRepeating(tileTex, true);
	SetTextureRepeating(sandTex, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(bumpmap, true);

	root = new SceneNode();

	SceneNode* portal1 = new SceneNode();
	portal1mesh = Mesh::LoadFromMeshFile("portal1.msh");
	portal1mat = new MeshMaterial("portal1.mat");

	root->AddChild(portal1);
	portal1->SetMesh(portal1mesh);
	portal1->SetBool(false);
	portal1->SetMaterial(portal1mat);
	portal1->SetTransform(Matrix4::Translation(Vector3((heightmapSize.x / 2) +  1430, (heightmapSize.y / 2) - 110, (heightmapSize.z / 2) - 1285)));
	portal1->SetModelScale(Vector3(110, 110, 110));
	portal1->SetValue("portal1tex");
	//a->SetTransform(Matrix4::Translation(Vector3(heightmapSize.x * ini, heightmapSize.y * ini, heightmapSize.z * ini)));
	for (int i = 0; i < portal1mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry1 = portal1mat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry1->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		portal1tex.emplace_back(texID);
	}
	portal1->SetGLUint(portal1tex);

	SceneNode* portal2 = new SceneNode();
	portal2mesh = Mesh::LoadFromMeshFile("portal2.msh");
	portal2mat = new MeshMaterial("portal2.mat");

	root->AddChild(portal2);
	portal2->SetMesh(portal2mesh);
	portal2->SetBool(false);
	portal2->SetMaterial(portal2mat);
	portal2->SetModelScale(Vector3(170, 170, 170));
	portal2->SetTransform(Matrix4::Translation(Vector3((heightmapSize.x / 2) + 1405, (heightmapSize.y / 2) + 125, (heightmapSize.z / 2) - 235)));
	portal2->SetValue("portal2tex");
	//a->SetTransform(Matrix4::Translation(Vector3(heightmapSize.x * ini, heightmapSize.y * ini, heightmapSize.z * ini)));
	for (int i = 0; i < portal2mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry2 = portal2mat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry2->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		portal2tex.emplace_back(texID);
	}
	portal2->SetGLUint(portal2tex);

	SceneNode* portal3 = new SceneNode();
	portal3mesh = Mesh::LoadFromMeshFile("portal3.msh");
	portal3mat = new MeshMaterial("portal3.mat");

	root->AddChild(portal3);
	portal3->SetMesh(portal3mesh);
	portal3->SetBool(false);
	portal3->SetMaterial(portal3mat);
	portal3->SetModelScale(Vector3(170, 170, 170));
	portal3->SetTransform(Matrix4::Translation(Vector3((heightmapSize.x / 2) + 550, (heightmapSize.y / 2) + 165, (heightmapSize.z / 2) - 420)));
	portal3->SetValue("portal3tex");
	//a->SetTransform(Matrix4::Translation(Vector3(heightmapSize.x * ini, heightmapSize.y * ini, heightmapSize.z * ini)));
	for (int i = 0; i < portal3mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry3 = portal3mat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry3->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		portal3tex.emplace_back(texID);
	}
	portal3->SetGLUint(portal3tex);

	SceneNode* portal4 = new SceneNode();
	portal4mesh = Mesh::LoadFromMeshFile("portal4.msh");
	portal4mat = new MeshMaterial("portal4.mat");

	root->AddChild(portal4);
	portal4->SetMesh(portal4mesh);
	portal4->SetBool(false);
	portal4->SetMaterial(portal4mat);
	portal4->SetTransform(Matrix4::Translation(Vector3((heightmapSize.x/2)+500,(heightmapSize.y/2)+120,(heightmapSize.z/2)-1350)));
	portal4->SetModelScale(Vector3(170, 170, 170));
	portal4->SetValue("portal4tex");
	
	for (int i = 0; i < portal4mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry4 = portal4mat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry4->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		portal4tex.emplace_back(texID);
	}
	portal4->SetGLUint(portal4tex);

	SceneNode* portal5 = new SceneNode();
	portal5mesh = Mesh::LoadFromMeshFile("portal5.msh");
	portal5mat = new MeshMaterial("portal5.mat");

	root->AddChild(portal5);
	portal5->SetMesh(portal5mesh);
	portal5->SetBool(false);
	portal5->SetMaterial(portal5mat);
	portal5->SetTransform(Matrix4::Translation(Vector3((heightmapSize.x / 2) - 500, (heightmapSize.y / 2) + 20, (heightmapSize.z / 2) - 1150)) * Matrix4::Rotation(25.0,Vector3(0.0f,1.0f,0.0f)));
	portal5->SetModelScale(Vector3(60, 60, 60));
	portal5->SetValue("portal5tex");
	
	for (int i = 0; i < portal5mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matentry5 = portal5mat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matentry5->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		portal5tex.emplace_back(texID);
	}
	portal5->SetGLUint(portal5tex);

	spotlight = new SpotLight(Vector3(1.0f,1.0f,1.0f), 
		Vector4(1, 1, 1, 1),
		3.5f, 
		20.0f,
		Vector3(0.5f, 0.0f, 0.5f));

	SceneNode* spotportal = new SceneNode();
	portal5->AddChild(spotportal);

	SceneNode* a = new SceneNode();
	character = Mesh::LoadFromMeshFile("Role_T.msh");
	anim = new MeshAnimation("Role_T.anm");
	spaceMaterial = new MeshMaterial("Role_T.mat");
	AnimShader = new Shader("AnimVertex.glsl", "AnimFragment.glsl");

	root->AddChild(a);
	a->SetMesh(character);
	a->SetBool(true);
	a->SetMaterial(spaceMaterial);
	a->SetTransform(Matrix4::Translation(Vector3((heightmapSize.x / 2) - 250, (heightmapSize.y / 2) - 10, (heightmapSize.z / 2) - 950)) * Matrix4::Rotation(320.0, Vector3(0.0f, 1.0f, 0.0f)));
	a->SetModelScale(Vector3(45, 45, 45));
	
	a->SetAnim(anim);
	for (int i = 0; i < character->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntryAnim = spaceMaterial->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntryAnim->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTexturesAnim.emplace_back(texID);
	}

	waterRotate = 0.0f;
	waterCycle = 0.0f;

	currentFrame = 0;
	frameTime = 0.0f;

	/*SceneNode* s = new SceneNode();
	mesh = Mesh::LoadFromMeshFile("portal1.msh");
	material = new MeshMaterial("portal1.mat");
	shader = new Shader("BlankVertex.glsl", "BlankFragment.glsl");

	root->AddChild(s);
	s->SetMesh(mesh);
	s->SetMaterial(material);
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}*/

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}

Renderer ::~Renderer(void) {
	delete root;
	delete camera;
	delete portal1mesh;
	delete portal2mesh;
	delete portal3mesh;
	delete portal4mesh;
	delete portal5mesh;
	delete portal1mat;
	delete portal2mat;
	delete portal3mat;
	delete portal4mat;
	delete portal5mat;
	delete character;
	delete spaceMaterial;
	delete anim;
	delete TerShader;
	delete shader; 
	delete AnimShader;
	delete reflectShader;
	glDeleteTextures(1, &texture);
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(dt);
	frameTime -= dt;
	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.25f;
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	light->orbit(dt, heightmapSize.x * 2.0f,heightmapSize.z * 1.0f);
	while (frameTime < 0.0f && anim != 0) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0f / anim->GetFrameRate();
	}
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

	for (vector < SceneNode* >::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::DrawNode(SceneNode* n) {
	
	if (n->GetMesh()) {
		if(!n->GetBool()) {
			BindShader(shader);

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			UpdateShaderMatrices();
			glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

			for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, n->GetGLUint()[i]);
				n->GetMesh()->DrawSubMesh(i);
			}
		}

		else{
			BindShader(AnimShader);
			vector <Matrix4> frameMatrices;

			const Matrix4* invBindPose = n->GetMesh()->GetInverseBindPose();
			const Matrix4* frameData = anim->GetJointData(currentFrame);

			for (unsigned int i = 0; i < n->GetMesh()->GetJointCount(); ++i) {
				frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
			}

			int j = glGetUniformLocation(AnimShader->GetProgram(), "joints");
			glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			UpdateShaderMatrices();
			glUniformMatrix4fv(glGetUniformLocation(AnimShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(AnimShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
			
			for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, matTexturesAnim[i]);
				n->GetMesh()->DrawSubMesh(i);
			}
		}

		//portal5tex.clear();
		//matTextures.clear();
	}
}

void Renderer::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox();
	DrawHeightmap();
	SetShaderLight(*light);
	//SetSpotLight(*spotlight);
	DrawNodes();
	DrawWater();
}

void Renderer::DrawWater() {
	BindShader(reflectShader);
	
	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = heightMap->GetHeightmapSize();

	modelMatrix = Matrix4::Translation(Vector3(hSize.x * 0.5f , hSize.y * 0.1f, hSize.z * 0.5f)) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));
	UpdateShaderMatrices();
	
	SetShaderLight(*light);
	quad->Draw();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap() {
	BindShader(TerShader);
	glUniform1i(glGetUniformLocation(TerShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTex);
	glUniform1i(glGetUniformLocation(TerShader->GetProgram(), "diffuseTex1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sandTex);
	glUniform1i(glGetUniformLocation(TerShader->GetProgram(), "diffuseTex2"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterTex);
	glUniform1i(glGetUniformLocation(TerShader->GetProgram(), "diffuseTex3"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tileTex);
	glUniform1i(glGetUniformLocation(TerShader->GetProgram(), "bumpTex"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bumpmap);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	heightMap->Draw();
}

