#version 330

in vec3 the_color;
out vec4 output_color; 

void main() {
	output_color=vec4(the_color, 1.0f);
} 
