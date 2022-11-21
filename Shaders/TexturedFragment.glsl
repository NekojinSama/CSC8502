#version 330 core
uniform sampler2D diffuseTex;	//splatmap
uniform sampler2D diffuseTex1;	//sandTexture
uniform sampler2D diffuseTex2;	//waterTexture
uniform sampler2D diffuseTex3;	//tileTexture
uniform sampler2D bumpTex;		//bumpTexture

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex{
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
	vec3 tangent ;
	vec3 binormal ;
	vec4 colours;
} IN;

out vec4 fragColour;

void main(void){
	vec3 incident = normalize ( lightPos - IN.worldPos );
	vec3 viewDir = normalize ( cameraPos - IN.worldPos );
	vec3 halfDir = normalize ( incident + viewDir );

	mat3 TBN = mat3 ( normalize ( IN.tangent ) , normalize ( IN.binormal ) , normalize ( IN.normal ));

	float sandAmount = texture(diffuseTex, IN.texCoord / 32.0f).r;
	float bumpAmount = texture(bumpTex, IN.texCoord / 32.0f).r;
	float waterAmount = texture(diffuseTex, IN.texCoord / 32.0f).g;
	float tileAmount = texture(diffuseTex, IN.texCoord / 32.0f).b;

	//vec4 sandTex = texture(diffuseTex1, IN.texCoord) * sandAmount;
	vec4 sandBumpTex = mix(texture(diffuseTex1, IN.texCoord) * sandAmount, texture(bumpTex, IN.texCoord) * sandAmount, 0.8);
	vec4 waterTex = texture(diffuseTex2, IN.texCoord) * waterAmount;
	vec4 tileTex = texture(diffuseTex3, IN.texCoord) * tileAmount;

	vec3 bumpNormal = texture ( bumpTex , IN.texCoord ).rgb;
	bumpNormal = normalize ( TBN * normalize ( bumpNormal * 2.0 - 1.0));

	vec4 splatmap = texture(diffuseTex, IN.texCoord / 16.0f);

	fragColour= vec4(0,0,0,0);
	float lambert = max ( dot ( incident , bumpNormal ) , 0.0f );
	float distance = length ( lightPos - IN.worldPos );
	float attenuation = 1.0 - clamp ( distance / lightRadius , 0.0 , 1.0);

	float specFactor = clamp ( dot ( halfDir , bumpNormal ) ,0.0 ,1.0);
	specFactor = pow ( specFactor , 60.0 );

	vec4 surface = sandBumpTex + waterTex + tileTex;

	vec3 surfaceRep = ( surface.rgb * lightColour.rgb );
	fragColour.rgb = surfaceRep * lambert * attenuation ;
	fragColour.rgb += ( lightColour.rgb * specFactor )* attenuation *0.33;
	fragColour.rgb += surfaceRep * 0.1f; // ambient !
	fragColour.a = surface.a;
}