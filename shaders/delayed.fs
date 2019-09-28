uniform sampler2D tex;
uniform sampler2D alphaMap;

uniform float time;

void main()
{
	vec4 color = texture2D(tex, gl_TexCoord[0]);
	vec4 alphaColor = texture2D(alphaMap, gl_TexCoord[0]);
	
	color.a = step(alphaColor.r, time) * 0.6;
	
    gl_FragData[0] = color;
    gl_FragData[1] = vec4(0);
}