#version 330 core
uniform sampler2D diffuseTex;
uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex{
	vec2 texCoord;
	//vec4 jointWeights;
} IN;

out vec4 fragColour;
void main(void) {
	fragColour = texture(diffuseTex, IN.texCoord);
}