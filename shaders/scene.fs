uniform sampler2D guiRtt;
uniform sampler2D colorRtt;
uniform sampler2D ppRtt;

uniform vec2 resolution;
uniform float time;
uniform float waveStrength;
uniform vec3 ripplePoint;
uniform float blurFactor;

// Here goes the basic functions

// "glsl-bezier-curve" by "yiwenl"
vec3 cubicBezier(vec3 A, vec3 B, vec3 C, vec3 D, float t)
{
	vec3 E = mix(A, B, t);
	vec3 F = mix(B, C, t);
	vec3 G = mix(C, D, t);

	vec3 H = mix(E, F, t);
	vec3 I = mix(F, G, t);
	
	vec3 P = mix(H, I, t);

	return P;
}

vec2 sineWave(vec2 tc, float factor, float strength, float amplify)
{
    // Wave distortion
    float x = cos(19.2 * factor * tc.x + 42.7 * factor * tc.y + time * 0.005) * strength;
    float y = sin(41.5 * factor * tc.x + 22.1 * factor * tc.y + time * 0.005) * strength;
	
	// Clamp to avoid bad effects on edges
    return vec2(clamp(tc.x + x * amplify, 0.001, 0.999), clamp(tc.y + y * amplify, 0.001, 0.999));
}

vec2 ripple(vec2 tc, vec3 point, float factor)
{
	// Get texture coordinates to a [-1, 1] range
	vec2 p = tc * 2.0 - 1.0;
	
	// Get ripple point to a [-1, 1] range
	vec2 p2 = vec2(point.x, 1.0 - point.y) * 2.0 - 1.0;
	
	// Get difference between texture coordinates and ripple point
	// so we can move the origin point of the ripple effect.
	vec2 fp = p - p2;
	
	// Calculate the length of the above vector
	float len = length(fp);
	
	// Normalize coordinates vector and apply phase shift
	vec2 x = tc + (fp / len) * sin(len * 12.0 - time * 0.005) * factor;
	
	// Clamp to avoid bad effects on edges
	return clamp(x, 0.001, 0.999);
}

float getHeatWaveFactor(sampler2D mask, vec2 coord, float radius)
{
	float center = texture2D(mask, coord).r;
	float up = texture2D(mask, clamp(coord + vec2(0.0, radius), vec2(0), vec2(1))).r;
	float down = texture2D(mask, clamp(coord - vec2(0.0, radius), vec2(0), vec2(1))).r;
	float left = texture2D(mask, clamp(coord - vec2(radius, 0), vec2(0), vec2(1))).r;
	float right = texture2D(mask, clamp(coord + vec2(radius, 0), vec2(0), vec2(1))).r;
	return (center + up + down + left + right) / 5.0;
}

// "glsl-fast-gaussian-blur" by "Jam3"
vec4 blur13(sampler2D image, vec2 uv, vec2 direction)
{
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.411764705882353) * direction;
	vec2 off2 = vec2(3.2941176470588234) * direction;
	vec2 off3 = vec2(5.176470588235294) * direction;
	color += texture2D(image, uv) * 0.1964825501511404;
	color += texture2D(image, uv + (off1 / resolution)) * 0.2969069646728344;
	color += texture2D(image, uv - (off1 / resolution)) * 0.2969069646728344;
	color += texture2D(image, uv + (off2 / resolution)) * 0.09447039785044732;
	color += texture2D(image, uv - (off2 / resolution)) * 0.09447039785044732;
	color += texture2D(image, uv + (off3 / resolution)) * 0.010381362401148057;
	color += texture2D(image, uv - (off3 / resolution)) * 0.010381362401148057;
	return color;
}

// Here goes game-specific functions

float getTimeRippleFactor(float t)
{
	float tr;
	if (t > 0.8)
	{
		tr = 1.0 - (t - 0.8) * 5.0;
	}
	else
	{
		tr = t * 1.25;
	}
	return cubicBezier(vec3(0), vec3(1, 0.03, 0), vec3(0.38, 0.04, 0), vec3(1), tr).y;
}

// Here goes the main method

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
		coord = sineWave(gl_TexCoord[0].xy, 1.0, waveStrength, 1.0);
	}
	
	// Apply ripple
	{
		float factor = getTimeRippleFactor(ripplePoint.z);
		coord = ripple(coord, ripplePoint, factor);
	}
	
	// Get post processing mask for the fragment
	vec4 pp = texture2D(ppRtt, coord);
	
	// Pre-scene processing
	{
		float factor = getHeatWaveFactor(ppRtt, coord, 0.005) * 0.25;
		coord = sineWave(coord, 2.0, 0.01, factor);
	}

	// Apply texture for the fragment
	vec4 color = texture2D(colorRtt, coord);
	
	// Apply pause-screen blur
	if (blurFactor > 0.0001)
	{
		color = blur13(colorRtt, coord, vec2(blurFactor));
	}
	
	// Overlap GUI
	vec4 gui = texture2D(guiRtt, coord);
	vec4 finalColor = mix(color, gui, gui.a);
	
	// Set fragment colorx
    gl_FragColor = finalColor;
}