uniform sampler2D tex;
uniform vec4 color;

void main()
{
	vec4 texColor = texture2D(tex, gl_TexCoord[0]);
    gl_FragColor = vec4(color.rgb, texColor.a * color.a);
}