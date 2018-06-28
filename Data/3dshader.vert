#version 410
uniform mat4 world;
in vec3 inPos;
in vec3 inNormal;

const vec3 lightdir = vec3(0.7,0.3,0);
const vec3 ambient = vec3(0.1,0.1,0.1);
smooth out vec3 outColor;
void main(){
	gl_Position = world*vec4(inPos,1.0f);
	
	outColor = vec3(1.0f,1.0f,1.0f)*dot(lightdir,inNormal)+ambient;
}