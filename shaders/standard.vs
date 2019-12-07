uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

uniform vec3 eyePos;

varying vec4 vertexColor;
varying mat3 tbn;
varying float lightDist;

void main()
{
	vec4 modelSpaceVertex = mWorld * gl_Vertex;
	mat4 modelView = mView * mWorld;

	gl_Position = mProj * mView * modelSpaceVertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;	
	vertexColor = gl_Color;
	
	vec3 vertexNormal = normalize((modelView * vec4(gl_Normal, 0)).xyz);
	vec3 vertexTangent = normalize((modelView * gl_MultiTexCoord1).xyz);
	vec3 vertexBitangent = normalize((modelView * gl_MultiTexCoord2).xyz);
	
	tbn = mat3(
		vertexTangent.x, vertexBitangent.x, vertexNormal.x,
		vertexTangent.y, vertexBitangent.y, vertexNormal.y,
		vertexTangent.z, vertexBitangent.z, vertexNormal.z
	);
	
	lightDist = distance(eyePos, modelSpaceVertex.xyz) / 150.0;
	lightDist = pow(lightDist, 4);
	lightDist = 1.0 - clamp(lightDist, 0.0, 1.0);
}
