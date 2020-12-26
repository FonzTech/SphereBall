uniform sampler2D tex;
uniform int time;

varying float dalpha;

void main()
{
    gl_FragData[0] = vec4(1, 1, 1, dalpha * 0.25);
    gl_FragData[1] = vec4(0.04, 0, dalpha, 1);
}