#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aTangent;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{	
	
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
	vec4 positionClip = projection * view * model * vec4(aPos, 1.0);
	vec2 grid = vec2(320.0, 240.0) * 0.5;
	vec4 snapped = positionClip;
	snapped.xyz = snapped.xyz / snapped.w;
	snapped.xy = floor(grid * snapped.xy) / grid;  // This is actual grid
	snapped.xyz *= positionClip.w;
	gl_Position = snapped;
}