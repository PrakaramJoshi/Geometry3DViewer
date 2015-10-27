#version 440 core
//in vec3 colour;
//out vec4 frag_colour;

//void main () {
  //frag_colour = vec4 (colour, 1.0);
//}

// Output
layout (location = 0) out vec4 color;

// Input from vertex shader
in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
	vec3 CLR;
} fs_in;

// Material properties
uniform vec3 diffuse_albedo = vec3(0.5, 0.5, 0.5);
uniform vec3 specular_albedo = vec3(0.7);
uniform float specular_power = 100;

void main(void)
{
    // Normalize the incoming N, L and V vectors
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);

    // Calculate R locally
    vec3 R = reflect(-L, N);

    // Compute the diffuse and specular components for each fragment
    vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo;
    vec3 specular = pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;

    // Write final color to the framebuffer
    color = vec4(fs_in.CLR + diffuse + specular, 1.0);
}