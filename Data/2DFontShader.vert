#version 410

uniform mat4	gMVP;

in vec3 inPosition; 
in vec2 inTexCoord;
in int inVertIndices;

out vec2 outTexCoord;

void main() 
{	
	gl_Position = gMVP * vec4(inPosition,1.0f);
	

	outTexCoord = inTexCoord;
}