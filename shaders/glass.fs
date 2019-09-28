#define M_PI 3.1415926535897932384626433832795

uniform sampler2D tex;
uniform int time;

varying float dalpha;

void main()
{
	vec4 color = texture2D(tex, gl_TexCoord[0]);
	
	float a = float(time % 3000) / 3000.0;
	float b = a * 3.0 - 1.5;
	float c = distance(gl_TexCoord[0].x + b, gl_TexCoord[0].y) * 2.0;
	float d = 1.0 - clamp(c, 0, 1);
	
	color.rgb = mix(color.rgb, vec3(1.0), d);
	color.a = dalpha + d * dalpha * 2.0;
	
    gl_FragData[0] = color;
    gl_FragData[1] = vec4(0);
}