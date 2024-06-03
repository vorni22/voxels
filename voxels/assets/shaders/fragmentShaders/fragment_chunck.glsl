#version 330 core

out vec4 FragColor;

in vec3 Normal;
//in vec3 FragPos;
//in vec3 LightPos;
in vec3 objectColor;
in float opacity;

// uniform vec3 lightColor;
// uniform float ambientStrength;
// uniform float specularStrength;
// uniform int shininess;

uniform vec3 lightDir;

void main(){
	//vec3 lightColor = vec3(1.0, 1.0, 1.0);
	//float ambientStrength = 0.1f
	//float specularStrength = 0.5f
	//float shininess = 16

	// Ambient
    //vec3 ambient = 0.1f * vec3(1.0, 1.0, 10.);;

	// Diffuse
	//vec3 norm = normalize(Normal);
	//vec3 lightDir = normalize(LightPos - FragPos); 
	//float diff = max(dot(norm, lightDir), 0.0);
	//vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);;

	// Specular
	// vec3 viewDir = normalize(-FragPos);
	// vec3 reflectDir = reflect(-lightDir, norm);
	// float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
	// vec3 specular = 0.5f * spec * vec3(1.0, 1.0, 1.0);;  

	//vec3 ret = (ambient + diffuse + specular) * objectColor;

	FragColor = vec4((0.3f * vec3(1.0, 1.0, 1.0) + max(dot(Normal, lightDir), 0.0) * vec3(1.0, 1.0, 1.0)) * objectColor, opacity);
}