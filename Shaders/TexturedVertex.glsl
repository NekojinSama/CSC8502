#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec4 colours;
in vec4 tangent;

out Vertex{
	vec2 texCoord;
	vec3 normal ;
	vec3 worldPos ;
	vec3 tangent ;
	vec3 binormal ;
	vec4 colours ;
}OUT;

void main(void) {
	OUT.colours = colours;
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	//OUT.normal = normalize(normalMatrix * normalize(normal));

	vec3 wTangent = normalize ( normalMatrix * normalize ( tangent . xyz ));
	vec3 wNormal = normalize ( normalMatrix * normalize ( normal ));
	OUT.tangent = wTangent ;
	OUT.normal = wNormal ;

	OUT.binormal = cross ( wTangent , wNormal ) * tangent.w ;

	mat4 mvp = projMatrix * viewMatrix * modelMatrix;

	vec4 worldPos = (modelMatrix * vec4(position, 1));
	OUT.worldPos = worldPos.xyz;

	gl_Position = (projMatrix * viewMatrix) * worldPos;
	OUT.texCoord = (textureMatrix * vec4(texCoord , 0.0, 1.0)).xy;
}