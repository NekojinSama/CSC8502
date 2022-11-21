#version 400

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec2 texCoord;

out Vertex {
	vec2 texCoord ;
} OUT ;

void main (void) {
	vec4 localPos = vec4 ( position , 1.0f );
	mat4 mvp = projMatrix * viewMatrix * modelMatrix ;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = texCoord;
}
