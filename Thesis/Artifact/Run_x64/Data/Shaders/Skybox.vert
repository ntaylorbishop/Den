#version 430 core

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;

in vec3 inPosition;
in vec4 inColor;
in vec2 inUV0;
in vec3 inNormal;
in vec3 inTangent;
in vec3 inBitangent;

out vec3 passUV0;
out vec4 passColor;
out vec3 passNormal;

void main ( void ) {

	passUV0		= inPosition;
	passColor	= inColor;
	passNormal	= inNormal;

	vec3 passNormal = (vec4(inNormal, 0.f) * gModel).xyz;


	gl_Position = vec4(inPosition, 1.0f) * gModel * gView * gProj;
}