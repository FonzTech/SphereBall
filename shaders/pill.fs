uniform sampler2D tex;
uniform vec3 position;

varying vec4 positionToWorld;

#define PI 3.1416

void main()
{
	vec4 m = gl_TexCoord[0];
	vec4 c = texture2D(tex, gl_TexCoord[0]);
	
	float f = clamp(distance(positionToWorld.xyz, position) / 10.0, 0.0, 1.0);
	c.rgb = mix(c.rgb, vec3(0), f);
	
    gl_FragColor = c;
}