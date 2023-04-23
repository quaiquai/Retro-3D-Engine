#version 430 core
out vec4 FragColor;

#define CAMERA_FILTER 0

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float iTime;

const float f = 1.0;
#define INTENSITY 1.0
vec3 scanline(vec2 coord, vec3 screen){
    const float scale = 1.66;
    const float amt = 0.02;// intensity of effect
    const float spd = 0.01;//speed of scrolling rows transposed per second
    
	screen.rgb += sin((coord.y / scale - (iTime * spd * 6.28))) * amt;
	return screen;
}

vec2 fisheye(vec2 uv, float str )
{
	float aspect = 1600.0 / 900.0;
	float strength = sin(2.0) * 0.02;

	vec2 intensity = vec2(strength * aspect, strength * aspect);

	vec2 coords = (uv -0.5) * 2.0;

	vec2 realCoordOffs;
	realCoordOffs.x = (1.0 - coords.y * coords.y) * intensity.y * (coords.x); 
    realCoordOffs.y = (1.0 - coords.x * coords.x) * intensity.x * (coords.y);
	
    return uv - realCoordOffs;	     
}

vec3 channelSplit(sampler2D tex, vec2 coord){
    const float spread = 0.003;
	vec3 frag;
	//radial blur
	vec2 blurOffset = ( coord - vec2( 0.5 ) ) * 0.001;;
	for ( int iSample = 1; iSample < 16; ++iSample ){
		frag.r += texture(tex, vec2(coord.x - spread * sin(iTime), coord.y) - blurOffset * float( iSample ) ).r;
		frag.g += texture(tex, vec2(coord.x, 					  coord.y) - blurOffset * float( iSample ) ).g;
		frag.b += texture(tex, vec2(coord.x + spread * sin(iTime), coord.y) - blurOffset * float( iSample ) ).b;
	}
	frag/=16.0;
	
	return frag;
}

vec2 biqgrad(sampler2D channel, vec2 res, vec2 uv) {
    vec2 q = fract(uv * res);
    vec2 c = (q*(q - 1.0) + 0.5) / res;
    vec2 w0 = uv - c;
    vec2 w1 = uv + c;
    vec2 cc = 0.5 / res;
    vec2 ww0 = uv - cc;
    vec2 ww1 = uv + cc;
    float nx0 = texture(channel, vec2(ww1.x, w0.y)).r - texture(channel, vec2(ww0.x, w0.y)).r;
    float nx1 = texture(channel, vec2(ww1.x, w1.y)).r - texture(channel, vec2(ww0.x, w1.y)).r;
    float ny0 = texture(channel, vec2(w0.x, ww1.y)).r - texture(channel, vec2(w0.x, ww0.y)).r;
    float ny1 = texture(channel, vec2(w1.x, ww1.y)).r - texture(channel, vec2(w1.x, ww0.y)).r;
	return vec2(nx0 + nx1, ny0 + ny1) / 2.0;
}

void main()
{	
	vec2 uv = gl_FragCoord.xy / vec2(1600.0,900.0);
	uv = floor(uv*400)/400;

#if CAMERA_FILTER
	vec3 rgb = texture(screenTexture, uv).rgb;
    vec2 edg = biqgrad(screenTexture, vec2(1600.0,900.0), uv);
    vec3 col = vec3(1. / (-rgb + 1.) / 6.); // Invert, then filter as quotient
	col = vec3(col.xy - edg / 2.0, col.z); // Apply Edge Deterioration
	vec3 color = col;
#else
	vec2 fisheyeUV = fisheye(uv, 0.3);
	vec3 color = channelSplit(screenTexture, fisheyeUV);
	vec2 screenSpace = fisheyeUV * vec2(1600.0,900.0);
	color = scanline(screenSpace, color);

	float vignette = uv.x * uv.y * ( 1.0 - uv.x ) * ( 1.0 - uv.y );
    vignette = clamp( pow( 16.0 * vignette, 0.3 ), 0.0, 1.0 );
    color *= vignette;
    //vec3 col = texture(screenTexture, TexCoords).rgb;
#endif
    FragColor = vec4(color, 1.0);
}