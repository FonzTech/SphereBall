uniform sampler2D tex;
uniform vec3 position;

uniform float fireFactor;

uniform float noiseFactor;
uniform sampler2D noiseTexture;

void main()
{
	// Apply the texture
	vec4 color = texture2D(tex, gl_TexCoord[0]);
	
	// Fire effect
	color.rgb = mix(color.rgb, vec3(1, 0, 0), fireFactor);
	
	// Dissolve effect
	vec4 noiseColor = texture2D(noiseTexture, gl_TexCoord[0]);
	color.a *= step(noiseFactor, noiseColor.r);
	
	// Write to targets
    gl_FragData[0] = color;
    gl_FragData[1] = vec4(0);
}