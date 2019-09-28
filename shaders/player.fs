uniform sampler2D tex;
uniform vec3 position;

uniform float noiseFactor;
uniform sampler2D noiseTexture;

void main()
{
	vec4 color = texture2D(tex, gl_TexCoord[0]);
	vec4 noiseColor = texture2D(noiseTexture, gl_TexCoord[0]);
	
	color.a = step(noiseFactor, noiseColor.r);
	
    gl_FragData[0] = color;
    gl_FragData[1] = vec4(0);
}