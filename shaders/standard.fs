uniform sampler2D tex;
uniform sampler2D normalMap;

uniform bool useNormalMap;
uniform float lightPower;

varying vec4 vertexColor;
varying mat3 tbn;
varying vec3 tsLightDir;
varying vec3 tsEyeDir;
varying float lightDist;

void main()
{
	vec4 diffuseColor = texture2D(tex, gl_TexCoord[0]) * vertexColor;

	if (useNormalMap)
	{
		// Get texture normal in tangent space
		vec4 normalMapValue = texture2D(normalMap, gl_TexCoord[0]) * 2.0 - 1.0;
	
		// Diffuse light	
		float NdotL = clamp(dot(normalMapValue.rgb, tsLightDir), 0, 1);
		diffuseColor += diffuseColor * vec4(lightPower) * NdotL * lightDist;
		
		// Specular light
		vec3 reflectDir = reflect(-tsEyeDir, normalMapValue.rgb);
		float EdotR = clamp(dot(-tsLightDir, reflectDir), 0, 1);
		diffuseColor += diffuseColor * vec4(lightPower) * pow(EdotR, 5) * lightDist;
	}
	
	gl_FragData[0] = diffuseColor;
	gl_FragData[1] = vec4(0);
}