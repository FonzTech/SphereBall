uniform sampler2D tex;
uniform float angle;

#define PI 3.1416

void main()
{
	// Get diffuse color
	vec4 m = texture2D(tex, gl_TexCoord[0]);
	
	// Get normalized angle difference
	float r = float(int(angle) % int(degrees(PI * 2.0)));
	float f = r > 180.0 ? 0.5 - (r - 180.0) / 360.0 : r / 360.0;
	
	// Mix diffuse color with white glow
	vec3 c = mix(m, vec3(1.0), f);
	
	// Apply above computed color to fragment
    gl_FragColor = vec4(c, 1.0);
}