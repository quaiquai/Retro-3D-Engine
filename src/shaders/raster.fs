#version 430 core
out vec4 FragColor;

layout(std430, binding = 3) buffer pixels
{
    float data_SSBO[];
};

void main()
{	

	int width = 1600;
	int height = 900;
	vec2 uv = gl_FragCoord.xy - 0.5; //lowerleft of gl_fragcord starts at 0.5 (because 1st center of pixel) so need to get to 0 for first elements
	int pixel_index = int(width*uv.y + uv.x);
    FragColor = vec4(data_SSBO[pixel_index], data_SSBO[pixel_index + 1], data_SSBO[pixel_index+2], 1.0);
}