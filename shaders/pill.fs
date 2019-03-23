uniform sampler2D tex;
uniform vec3 position;

varying vec4 positionToWorld;

void main()
{
	vec4 color = texture2D(tex, gl_TexCoord[0]);
	
	{
		float coeff = clamp((positionToWorld.y - position.y) / 5.0, 0.0, 1.0);
		color.rgb = mix(color.rgb, vec3(1), coeff);
	}
	
	{
		float coeff = clamp((position.y - positionToWorld.y) / 5.0, 0.0, 1.0);
		color.rgb = mix(color.rgb, vec3(0), coeff);
	}
	
    gl_FragColor = color;
}