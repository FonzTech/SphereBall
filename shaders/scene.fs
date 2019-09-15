uniform sampler2D tex;
uniform float time;
uniform float strength;

vec2 sineWave(vec2 p)
{
    // Wave distortion
    float x = cos(20.0 * p.x + 40.0 * p.y + time * 0.005) * strength;
    float y = sin(40.0 * p.x + 20.0 * p.y + time * 0.005) * strength;
    return vec2(clamp(p.x + x, 0.001, 0.999), clamp(p.y + y, 0.001, 0.999));
}

void main()
{
	// Apply wave effect
	vec2 coord;
	if (strength < 0.00001)
	{
		coord = gl_TexCoord[0];
	}
	else
	{
		coord = sineWave(gl_TexCoord[0]);
	}

	// Apply texture for the fragment
	vec4 color = texture2D(tex, coord);
	
	// Set fragment color
    gl_FragColor = color;
}
