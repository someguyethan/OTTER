#version 410

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

uniform sampler2D s_Diffuse;
uniform sampler2D s_Diffuse2;
uniform sampler2D s_Specular;

uniform vec3  u_AmbientCol;
uniform float u_AmbientStrength;

uniform vec3  u_LightPos;
uniform vec3  u_LightCol;
uniform float u_AmbientLightStrength;
uniform float u_SpecularLightStrength;
uniform float u_Shininess;
// NEW in week 7, see https://learnopengl.com/Lighting/Light-casters for a good reference on how this all works, or
// https://developer.valvesoftware.com/wiki/Constant-Linear-Quadratic_Falloff
uniform float u_LightAttenuationConstant;
uniform float u_LightAttenuationLinear;
uniform float u_LightAttenuationQuadratic;

uniform float u_TextureMix;

uniform vec3  u_CamPos;

uniform int u_DiffuseFactor;
uniform int u_AmbientFactor;
uniform int u_SpecularFactor;
uniform int u_ToonFactor;

out vec4 frag_color;

//Toon Shading
const int bands = 10;
const float scaleFactor = 1.0/bands;

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	// Lecture 5
	vec3 ambient = u_AmbientLightStrength * u_LightCol;

	// Diffuse
	vec3 N = normalize(inNormal);
	vec3 lightDir = normalize(u_LightPos - inPos);

	float dif = max(dot(N, lightDir), 0.0);
	vec3 diffuse = dif * u_LightCol;// add diffuse intensity

	//diffuse = floor(diffuseOut * bands) * scaleFactor;

	//Attenuation
	float dist = length(u_LightPos - inPos);
	float attenuation = 1.0f / (
		u_LightAttenuationConstant + 
		u_LightAttenuationLinear * dist +
		u_LightAttenuationQuadratic * dist * dist);

	// Specular
	vec3 viewDir  = normalize(u_CamPos - inPos);
	vec3 h        = normalize(lightDir + viewDir);

	// Get the specular power from the specular map
	float texSpec = texture(s_Specular, inUV).x;
	float spec = pow(max(dot(N, h), 0.0), u_Shininess); // Shininess coefficient (can be a uniform)
	vec3 specular = u_SpecularLightStrength * texSpec * spec * u_LightCol; // Can also use a specular color

	// Get the albedo from the diffuse / albedo map
	vec4 textureColor1 = texture(s_Diffuse, inUV);
	vec4 textureColor2 = texture(s_Diffuse2, inUV);
	vec4 textureColor = mix(textureColor1, textureColor2, u_TextureMix);

	//Outline Effect             Thickness of Line
	//float edge = (dot(viewDir, N) < 0.0000001) ? 0.0 : 1.0; //If below threshold it is 0, otherwise 1

	vec3 lightContribution = ((ambient * u_AmbientFactor) + (diffuse * u_DiffuseFactor) + (specular * u_SpecularFactor)) * attenuation;

	if(u_ToonFactor == 1)
		lightContribution = clamp(floor(lightContribution * bands) * scaleFactor, vec3(0.0), vec3(1.0));

	vec3 result = 
		(u_AmbientCol * u_AmbientStrength + lightContribution)// global ambient light
		 // light factors from our single light
		 * inColor * textureColor.rgb /* * edge */; // Object color

	frag_color = vec4(result, textureColor.a);
}