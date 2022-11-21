#version 330 core
uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex{
	vec2 texCoord;
	vec4 colours;
} IN;

out vec4 fragColour;
void main(void) {
	fragColour = IN.colours;
	if( useTexture > 0) {
		fragColour *= texture ( diffuseTex, IN.texCoord );
	}
}