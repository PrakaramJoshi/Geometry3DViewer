#version 440 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;
// Values that stay constant for the whole mesh.
layout (location = 2) in vec3 normal;
// Inputs from vertex shader
out vec3 colour;

//

// Matrices we'll need
layout (std140) uniform constants
{
    mat4 mv_matrix;
    mat4 view_matrix;
    mat4 proj_matrix;
};

// Inputs from vertex shader
out VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
	vec3 CLR;
} vs_out;

// Position of light
uniform vec3 light_pos;

void main () {

   vec4 position=vec4(vertex_position,1.0);
    // Calculate view-space coordinate
    vec4 P = mv_matrix * position;
	float x = light_pos.x;

	vec4 light_source_position = vec4(light_pos.x,light_pos.y,light_pos.z,1.0);
	vec4 light_source = mv_matrix*light_source_position;

    // Calculate normal in view-space
	//TODO fix normal
	vec3 normal_temp = vec3(vertex_position);
    vs_out.N = mat3(mv_matrix) * normal_temp;

    // Calculate light vector
    vs_out.L = light_source.xyz - P.xyz;

    // Calculate view vector
    vs_out.V = -P.xyz;

    // Calculate the clip-space position of each vertex
    gl_Position = proj_matrix*P;
	
	vs_out.CLR = vertex_colour;
}