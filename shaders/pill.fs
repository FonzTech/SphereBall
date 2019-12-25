varying vec4 positionToWorld;

uniform sampler2D tex;
uniform vec3 position;

void main()
{
	// Get diffuse color
	vec4 color = texture2D(tex, gl_TexCoord[0]);
	
	/*
		The method used to compute this effect is basically the same for the top
		and the bottom of the 3D model, except for factor order in the subtraction.
	*/
	
	// Fake white light on top
	{
		// Get difference between vertex position and object's vertical position in a [0,5] range
		float coeff = clamp((positionToWorld.y - position.y) / 5.0, 0.0, 1.0);
		
		// Apply desired effect
		color.rgb = mix(color.rgb, vec3(1), coeff);
	}
	
	// Fake black shadow on bottom
	{
		// Get difference between object's vertical position and vertex position in a [0,5] range
		float coeff = clamp((position.y - positionToWorld.y) / 5.0, 0.0, 1.0);
		
		// Apply desired effect
		color.rgb = mix(color.rgb, vec3(0), coeff);
	}
	
	// Assign color to fragment
    gl_FragData[0] = color;
	gl_FragData[1] = vec4(0, 0.02, 0, 1);
}