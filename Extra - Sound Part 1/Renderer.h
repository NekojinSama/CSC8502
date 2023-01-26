/******************************************************************************
Class:Renderer
Implements:OGLRenderer
Author:Rich Davison
Description: A simple, SceneNode-enabled OpenGL renderer. This is entirely 
based off of the Scene Management tutorial, with the slight change of being
able to accept a new 'root' SceneNode, and a 'CameraNode'. 

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////


#pragma once

#include "../Framework/OGLRenderer.h"
#include "../Framework/cameraNode.h"
#include "../Framework/SceneNode.h"
#include "../Framework/Frustum.h"
#include "../Framework/CubeRobot.h"

#include <algorithm>

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void	SetCamera(CameraNode* n);

	void	SetRootNode(SceneNode * r);

protected:
	void	BuildNodeLists(SceneNode* from);
	void	SortNodeLists();
	void	ClearNodeLists();
	void	DrawNodes();
	void	DrawNode(SceneNode*n);

	CameraNode*		camera;
	SceneNode*		root;

	Frustum			frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};

