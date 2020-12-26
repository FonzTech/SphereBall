uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

uniform vec3 lookAt;
uniform float fadeWhenFar;

varying float dalpha;

void main()
{
	vec4 position = mWorld * gl_Vertex;
	
	float d = clamp((distance(lookAt, position.xyz) - 20) / 100, 0, 1);
	dalpha = mix(1.0 - d, d, fadeWhenFar);
	
	gl_Position = mProj * mView * position;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
