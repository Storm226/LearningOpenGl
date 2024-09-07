#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
	// if you want textures enable
   // FragColor = (mix(texture(texture1, TexCoord), texture (texture2, TexCoord), .8) *  vec4(lightColor * objectColor, 1.0)) ;
   //
  


   // Lighting Calculations , generally done with normalized vectors (mostly interested in angles, not magnitudes)
   // --------------------------------------------------
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize (lightPos - FragPos);

   // Ambient Lighting
   float ambientStength = 0.1;
   vec3 ambient = ambientStength * lightColor;

   // Diffuse Lighting
   // if theta between v & w > 90degrees, dot(v,w) < 0, resulting 
   // in negative colors. Negastive colors are not generally defined. 
   float diff = max (dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;
   vec3 result = (ambient+diffuse) * objectColor;

   FragColor = vec4(result, 1.0);


}

