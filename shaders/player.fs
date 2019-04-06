uniform sampler2D tex;
uniform vec3 position;

void main()
{
	vec4 color = texture2D(tex, gl_TexCoord[0]);
    gl_FragColor = color;
}