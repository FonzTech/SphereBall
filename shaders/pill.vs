uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

varying vec4 positionToWorld;

void main()
{
	positionToWorld = mWorld * gl_Vertex;
	gl_Position = mProj * mView * positionToWorld;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
