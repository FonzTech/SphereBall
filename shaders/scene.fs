varying vec2 texCoord;

uniform sampler2D tex;
uniform float time;
uniform float strength;

vec2 sineWave(vec2 p)
{
    // Wave distortion
    float x = sin(20.0 * p.x + 40.0 * p.y + time * 0.005) * strength;
    float y = sin(40.0 * p.x + 20.0 * p.y + time * 0.005) * strength;
    return vec2(p.x + x, p.y + y);
}

void main()
{
	// Apply wave effect
	vec2 coord = sineWave(gl_TexCoord[0]);

	// Apply texture for the fragment
	vec4 color = texture2D(tex, coord);
	
	
	// Set fragment color
    gl_FragColor = color;
}
