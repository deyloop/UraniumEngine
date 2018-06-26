#version 410
uniform mat4 world;
in vec3 inPos;
in vec3 inColor;

smooth out vec3 outColor;
void main(){
	gl_Position = world*vec4(inPos,1.0f);
	outColor = inColor;
}