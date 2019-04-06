uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

void main()
{
	gl_Position = mProj * mView * mWorld * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
