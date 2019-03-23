uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

varying vec4 positionToWorld;

void main()
{
	// Let pass the vertex in world space to fragment shader
	positionToWorld = mWorld * gl_Vertex;
	
	// Compute final vertex position
	gl_Position = mProj * mView * positionToWorld;
	
	// Assign texture coordinates
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
