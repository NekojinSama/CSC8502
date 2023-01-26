/******************************************************************************
Class:N/A
Implements:
Author:Rich Davison
Description: Main file for the Sound tutorial. In a break from the usual style
of my tutorials, you've been provided with this file, rather than having to
type it in. The main reason for this is that most of the code is stuff you've
seen before, and even the new stuff isn't actually that exciting - not worth
bumping up the page count of the tutorial by another 3 or so pages! So, instead
I'll comment the code here, and hopefully you'll be able to keep track of what
is doing what.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "Framework.lib") //Yep, still using my awesome framework

//Audio-specific library includes. Note we don't need the latter two until
//part 2, 
#pragma comment(lib, "openal32.lib")
#pragma comment(lib, "libogg_static.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")

#include "../Framework/SoundSystem.h"	//Include our awesome SoundSystem!
#include "Renderer.h"  

//In part B, we use another two sound files, in ogg format
#define NUM_SOUNDS 7

string soundNames[NUM_SOUNDS] = {
	"../Sounds/14615__man__canon.wav",
	"../Sounds/36847__ecodtr__laserrocket2.wav",
	"../Sounds/56900__syna-max__war.wav",
	"../Sounds/75190__herbertboland__sheeheep.wav",
	"../Sounds/41579__erdie__steps-on-stone01.wav",	//Don't forget to uncomment the comma!
	"../Sounds/139320__votives__arpegthing.ogg",
	"../Sounds/142010__vosvoy__the-road-a-melody-from-the-past.ogg"
};

/*
To demonstrate The SoundSystem and SoundNodes, we're going to create a simple
scene, containing a number of SoundNodes in random positions, each with a random
position, sound, and radius. To make it easy to see where each node is, it will
also have a transparent sphere of size equal to the Sound's radius. We'll also
reuse the brick 'floor' from the shadow mapping tutorial, just to use as a 
frame of reference as the camera moves around.

*/
int main() {
	Window w("Sound using OpenAL!!", 800,600,false);
	if(!w.HasInitialised()) {
		cout << "Window Creation failed!" << endl;
		return -1; //Should have done this for all of the tutorials...
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		cout << "Renderer Creation failed!" << endl;
		return -2; //Note the different error codes...*sigh*
	}

	/*
	We MUST initialise the sound system before we add any sounds, as we
	use some OpenAL functions in the Sound class
	*/
	SoundSystem::Initialise();

	/*
	Add all of our sounds into the global sounds vector
	*/
	for(int i = 0; i < NUM_SOUNDS; ++i) {
		Sound::AddSound(soundNames[i]);
	}

	SceneNode * root = new SceneNode();	//Root node for our renderer
	renderer.SetRootNode(root);

	//Let's add a 'floor' node to the scenegraph
	SceneNode*floornode = new SceneNode();
	floornode->SetTransform(Matrix4::Rotation(90, Vector3(1,0,0)) * Matrix4::Scale(Vector3(2048,2048,1)));
	floornode->SetBoundingRadius(2048.0f);
	//Our floor node needs a mesh
	Mesh*floor = Mesh::GenerateQuad();
	floor->SetTexture(SOIL_load_OGL_texture("../Textures/brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	floornode->SetMesh(floor);
	root->AddChild(floornode);//And add it to the root

	//Each SoundNode we add is going to have a spherical mesh so we can see it. 
	Mesh*sphere = new OBJMesh("../Meshes/ico.obj");

	//Add 20 SoundNodes into the SceneGraph, each with a random sound, position,
	//and scale.
	for(int i = 0; i < 20; ++i) {
		SoundNode* s = new SoundNode();
		//Add a random sound
		s->SetSound(Sound::GetSound(soundNames[rand()%NUM_SOUNDS]));
		s->SetLooping(true); //Make sound loop forever

		s->SetMesh(sphere); //We want to be able to see it!
		s->SetColour(Vector4(1,1,1,0.6)); //Make node transparent, too

		//Make each node have a slightly random size.
		float size = 300 + (rand()%300);
		s->SetModelScale(Vector3(size,size,size));
		s->SetRadius(size);
		s->SetBoundingRadius(size);

		
		//And a random position, too
		Vector3 randpos = Vector3((rand()%10)*256, 0.0f,(rand()%10)*256);
		randpos -= Vector3((rand()%10)*256, 0.0f,(rand()%10)*256);

		s->SetTransform(Matrix4::Translation(randpos));

		//Add the node into the graph - They'll add themselves to the
		//SoundSystem when they get Updated
		root->AddChild(s);
	}
	
	//Add a CameraNode to our world. CameraNodes are just a SceneNode
	//wrapper around the Camera class you were introduced to in
	//the Graphics module. 
	CameraNode* cam = new CameraNode();
	cam->SetPosition(Vector3(-200,2000,4096));

	root->AddChild(cam); //Add it to the graph so it is updated every frame


	/*
	Make the Camera the source for both our renderer and SoundSystem
	*/
	SoundSystem::GetSoundSystem()->SetListener(cam);
	renderer.SetCamera(cam);
	
	/*
	Our main game loop
	*/
	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		float msec = w.GetTimer()->GetTimedMS();	//How many msec has passed?

		/*
		Rather than have the Renderer update the SceneGraph, we should really
		handle it separately, which we do so here. 
		*/
		root->Update(msec);	

		//This is how to trigger sounds, as introduced in part 2.
		//if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_0)) {
		//	SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound(soundNames[0]),SOUNDPRIORTY_LOW);
		//}

		/*
		Update our graphics and audio systems
		*/
		SoundSystem::GetSoundSystem()->Update(msec);
		renderer.UpdateScene(msec);

		renderer.RenderScene(); //Render the scene...
	}

	//Once we're done with everything, we should delete our new'd assets, which
	//now includes the root SceneNode, too.
	delete floor;	//floor mesh
	delete sphere;	//audio node mesh
	delete root;	//root node (deletes it's children, too!)

	//We should delete the Sound vector before the SoundSystem, as again, it 
	//relies on OpenAL, which gets destroyed by the SoundSystem destructor
	Sound::DeleteSounds();
	SoundSystem::Destroy();  

	return 0;	//All done!
}