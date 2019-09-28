uniform sampler2D colorRtt;
uniform sampler2D bloomRtt;

uniform float time;
uniform float waveStrength;
uniform vec3 ripplePoint;

vec2 sineWave(vec2 tc)
{
    // Wave distortion
    float x = cos(20.0 * tc.x + 40.0 * tc.y + time * 0.005) * waveStrength;
    float y = sin(40.0 * tc.x + 20.0 * tc.y + time * 0.005) * waveStrength;
	
	// Clamp to avoid bad effects on edges
    return vec2(clamp(tc.x + x, 0.001, 0.999), clamp(tc.y + y, 0.001, 0.999));
}

vec2 ripple(vec2 tc)
{
	// Get texture coordinates to a [-1, 1] range
	vec2 p = tc * 2.0 - 1.0;
	
	// Get ripple point to a [-1, 1] range
	vec2 p2 = vec2(ripplePoint.x, 1.0 - ripplePoint.y) * 2.0 - 1.0;
	
	// Get difference between texture coordinates and ripple point
	// so we can move the origin point of the ripple effect.
	vec2 fp = p - p2;
	
	// Calculate the length of the above vector
	float len = length(fp);
	
	// Normalize coordinates vector and apply phase shift
	vec2 x = tc + (fp / len) * sin(len * 12.0 - time * 0.005) * sin(ripplePoint.z) * 0.1;
	
	// Clamp to avoid bad effects on edges
	return clamp(x, 0.001, 0.999);
}

void main()
{
	// Apply wave effect
	vec2 coord;
	if (waveStrength < 0.00001)
	{
		coord = gl_TexCoord[0].xy;
	}
	else
	{
		coord = sineWave(gl_TexCoord[0].xy);
	}
	
	// Apply ripple
	coord = ripple(coord);

	// Apply texture for the fragment
	vec4 color = texture2D(colorRtt, coord);
	
	// Set fragment colorx
    gl_FragColor = color;
}
