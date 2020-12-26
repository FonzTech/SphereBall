uniform sampler2D tex;
uniform vec3 color;

varying vec4 vertexColor;

void main()
{
	vec4 finalColor = texture2D(tex, gl_TexCoord[0]) * vertexColor;
	finalColor.rgb = mix(finalColor.rgb, color, 1.0 - finalColor.a);
	finalColor.a = 1.0;

    gl_FragData[0] = finalColor;
	gl_FragData[1] = vec4(0, 0.02, 0, 1);
}