uniform sampler2D tex;
uniform sampler2D normalMap;
uniform bool useNormalMap;
uniform float lightPower;
uniform vec3 lightDir;

varying vec4 vertexColor;
varying mat3 tbn;
varying float lightDist;
 
void main()
{
	vec4 diffuseColor = texture2D(tex, gl_TexCoord[0]) * vertexColor;

	if (useNormalMap)
	{
		vec4 normalMapValue = texture2D(normalMap, gl_TexCoord[0]) * 2.0 - 1.0;
	
		vec3 tsLightDir = normalize(tbn * lightDir);
		float NdotL = clamp(dot(normalMapValue, tsLightDir), 0, 1);
		
		diffuseColor += diffuseColor * vec4(lightPower) * NdotL * lightDist;
	}
	
	gl_FragData[0] = diffuseColor;
	gl_FragData[1] = vec4(0);
}