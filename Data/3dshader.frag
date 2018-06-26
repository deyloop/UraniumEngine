#version 410
in vec3 outColor;
out vec4 color;
void main(){
	color = vec4(outColor,1.0f);
	//color = vec4(1.0f,1.0f,1.0f,1.0f);
}
