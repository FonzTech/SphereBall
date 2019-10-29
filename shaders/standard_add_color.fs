uniform sampler2D tex;

varying vec4 vertexColor;

void main()
{
    gl_FragData[0] = texture2D(tex, gl_TexCoord[0]) * vertexColor;
    gl_FragData[1] = vec4(0);
}