uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

uniform vec3 lookAt;
uniform float fadeWhenFar;

varying float dalpha;

void main()
{
	vec4 position = mWorld * gl_Vertex;
	
	float d = max(0, distance(lookAt, position.xyz) - 50);
	float x = mix(1.0 - d * 0.1, d * 0.005, fadeWhenFar);
	float y = mix(0.675, 1.0, fadeWhenFar);
	dalpha = clamp(x, 0.0, y);
	
	gl_Position = mProj * mView * position;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
