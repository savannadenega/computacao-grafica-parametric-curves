#version 330 core

in vec3 Normal;  
in vec3 Position;
in vec2 Mapping;

uniform vec3 Ks;
uniform vec3 Kd;
uniform vec3 Ka;
uniform float Ns;

vec3 lightPos  = vec3 (10.0, 25.0, 50.0);
vec3 Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 La = vec3 (0.2, 0.2, 0.2); // grey ambient colour

uniform sampler2D texture1;
uniform mat4 model; 
uniform mat4 view; 
uniform vec3 viewPos;

out vec4 color;

void main () {
	// Ambient
	vec3 ambient = La * Ka;

	// Diffuse
	vec3 norm = normalize(Normal);
	//vec3 light_position_eye = vec3 (view * vec4 (lightPos, 1.0));
	vec3 lightDir = normalize(lightPos - Position);
	//float dot_prod = dot(lightDir, norm);
	//dot_prod = max(dot_prod, 0.0);
	float diff = max(dot(norm,lightDir), 0.0);
	vec3 diffuse = Ld * Kd * diff;

	// Specular
    vec3 viewDir = normalize(viewPos - Position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
    vec3 specular = Ls * spec * Ks;

	//Texture
	vec4 texel = texture(texture1, Mapping);
	color = vec4(specular + diffuse + ambient, 1.0) * texel * texel.a;
}