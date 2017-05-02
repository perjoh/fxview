#version 330

layout(location=0) in vec3 position; 
layout(location=1) in vec3 normal; 
layout(location=2) in vec3 color; 
uniform mat4 model_view_projection;
uniform mat3 model_transform;
//uniform vec3 in_color;
out vec3 the_color;

void main() 
{
	gl_Position = model_view_projection*vec4(position, 1.0f);
	vec3 lightpos = vec3(20.0f, 50.0f, 10.0f);
	vec3 normal_trans = model_transform*normal;
	float shade = dot(normalize(lightpos - position), normal_trans);
	the_color = min(color + shade, vec3(1.0f, 1.0f, 1.0f));
} 
