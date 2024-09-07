#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;


struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light{
	vec3 Position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light;

uniform sampler2D texture1;
uniform sampler2D texture2;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;


void main()
{
   // Lighting Calculations , generally done with normalized vectors (mostly interested in angles, not magnitudes)
   // --------------------------------------------------
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize (lightPos - FragPos);

   // Ambient Lighting
   vec3 ambient = light.ambient * material.ambient;

   // Diffuse Lighting
   float diff = max (dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * light.diffuse * material.diffuse;

   // specular Lighting
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular =  spec * light.specular * material.specular;  

	vec3 phongLightResult = (ambient + diffuse + specular);
	FragColor = vec4(phongLightResult, 1.0);
	// if you want TEXTURES ENABLE BELOW 
	//FragColor = (mix(texture(texture1, TexCoord), texture (texture2, TexCoord), .8) *  vec4(lightColor * objectColor, 1.0)) ;

}

