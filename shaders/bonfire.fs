uniform sampler2D tex;

void main()
{
    gl_FragData[0] = texture2D(tex, gl_TexCoord[0]);
    gl_FragData[1] = vec4(1, 0, 0, 1);
}