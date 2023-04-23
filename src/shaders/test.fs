#version 430 core
out vec4 FragColor;
in vec2 TexCoord;
uniform vec3 viewPos;


// texture samplers
layout(binding = 0) uniform sampler2D texture1;
layout(binding = 1) uniform sampler2D texture2;

struct Light {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal;  

uniform Light light;

float near = 0.1; 
float far  = 10.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	float depth = LinearizeDepth(gl_FragCoord.z)/far;
	// linearly interpolate between both textures (80% container, 20% awesomeface)

	vec4 ambient = vec4(light.ambient, 1.0) * texture(texture1, TexCoord);
	//vec4 ambient = vec4(light.ambient, 1.0) * vec4(0.5,0.5,0.5,1.0);

	vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * texture(texture1, TexCoord);  
	//vec4 diffuse = vec4(light.diffuse,1.0) * diff * vec4(0.5,0.5,0.5,1.0); 

	// spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse.rgb  *= intensity;

	// attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient.rgb  *= attenuation; 
    diffuse.rgb   *= attenuation;
    
	vec4 result = ambient + diffuse;
	FragColor = mix(result, vec4(0.5, 0.5, 1.0, 1.0), depth);
}