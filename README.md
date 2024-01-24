# Graphics Coursework
This project uses a graphics engine built from scratch utilising OpenGL. This project was developed as part of the university project. This project helped me learn the concepts of OpenGL.

## Features Implemented:
### Shaders
- [SplatMap](Shaders/TexturedVertex.glsl) 
- [Bump Texture](Shaders/BumpVertex.glsl)
- [Imported Mesh](Blank%20Project/Renderer.cpp#L75)
- [Directional Light](Blank%20Project/Renderer.cpp#L284)
- [Water Texture](Shaders/reflectVertex.glsl)
- [CubeMaps](Shaders/skyboxVertex.glsl)
### Scene 
- [Camera Rotation around the scene](Blank%20Project/Renderer.cpp#L24)
- [Scene Graph Management](Blank%20Project/Renderer.cpp#L291)

## Basic Controls:

| Control | Description |
| :---: | :--- |
W    | Forward
S   | Backward
A    | Left
D    | Right
C | Toggle Camera Animation
SHIFT | Move Camera Up
SPACE | Move Camera Down

## Project Highlights:
The project enabled me to accumulate a substantial amount of knowledge, which was both impressive and provided a comprehensive introduction to various facets of the graphics pipeline. For example generating textures using splat maps, implementation of lights and engaging with a graphics API about which I had no prior familiarity. The key insight I've gained from this project is that although I acknowledge the potential for further implementation, my awareness of the improvements I would introduce reflects my personal growth.

## Problems faced during the project:
Problems which I faced during this project would be creating a proper rendering pipeline for different meshes, and porting meshes from Unity to msh files. Learning to make a proper pipeline function took lots of time which could've been used to polish other aspects like having lights implemented on the imported assets.

## Project Showcase:
> Click/tap on the image for a YouTube video showing the gameplay:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/pwkJQybwczg/0.jpg)](https://www.youtube.com/watch?v=pwkJQybwczg)
