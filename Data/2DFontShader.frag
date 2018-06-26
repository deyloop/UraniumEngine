#version 410 

uniform sampler2D gSampler;		//from the app.

in vec2 outTexCoord;			//from the vertex shader

out vec4 outputColor; 

void main() 
{ 
	outputColor = vec4(1.0f,1.0f,1.0f,texture(gSampler,outTexCoord).s); 
}