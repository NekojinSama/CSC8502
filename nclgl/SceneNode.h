#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>
#include "MeshMaterial.h"
#include "MeshAnimation.h"

class SceneNode {
public:
	SceneNode(Mesh * m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);
	
	void SetTransform(const Matrix4 & matrix) { transform = matrix; }
	const Matrix4 & GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }
	
	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }
	
	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }
	
	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	
	void AddChild(SceneNode* s);
	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer & r);

	std::vector < SceneNode* >::const_iterator GetChildIteratorStart() {
		return children.begin();
	}

	std::vector < SceneNode* >::const_iterator GetChildIteratorEnd() {
		return children.end();
	}

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }
	
	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() const { return texture; }

	void SetMaterial(MeshMaterial* mat) { material = mat; }
	MeshMaterial* GetMaterial() const { return material; }

	void SetAnim(MeshAnimation* animation) { anim = animation; }
	MeshAnimation* GetAnim() const { return anim; }

	void SetBool(bool val) {id = val; }
	bool GetBool() const { return id; }

	void SetValue(string name) { nameVal = name; }
	string GetValue() const { return nameVal; }

	void SetGLUint(vector <GLuint> matTex) { matTextures = matTex; }
	vector <GLuint> GetGLUint() const { return matTextures; }

	/*void SetMatVec(GLuint Mat) const { matTextures.append(Mat); }
	void SetMesh(Mesh* m) { mesh = m; }*/

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a -> distanceFromCamera < b -> distanceFromCamera) ? true : false;
	}


protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector < SceneNode* > children;

	float distanceFromCamera;
	float boundingRadius;
	GLuint texture;
	MeshMaterial* material;
	MeshAnimation* anim;
	vector <GLuint> matTextures;
	bool id;
	string nameVal;
};