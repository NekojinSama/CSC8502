#include "../NCLGL/window.h"
#include "Renderer.h"





int main()	{
	Window w("Make your own project!", 720, 500, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}
	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		float timeElapsed = w.GetTimer()->GetTotalTimeSeconds();
		renderer.CameraMovement(timeElapsed, w.GetTimer()->GetTimeDeltaSeconds());
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_C)) {
			renderer.ToggleCameraMov();
		}
	}
	return 0;
}