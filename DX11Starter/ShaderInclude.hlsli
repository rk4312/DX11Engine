#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier! 
#define __GGP_SHADER_INCLUDES__ 

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2

#define MAX_SPECULAR_EXPONENT		256.0f 

// Light Struct
struct Light
{
	int Type;						// Which kind of light?  0, 1 or 2 (see above) 
	float3 Direction;				// Directional and Spot lights need a direction 
	float Range;					// Point and Spot lights have a max range for attenuation 
	float3 Position;				// Point and Spot lights have a position in space 
	float Intensity;				// All lights need an intensity 
	float3 Color;					// All lights need a color 
	float SpotFalloff;				// Spot lights need a value to define their “cone” size 
	float3 Padding;					// Purposefully padding to hit the 16-byte boundary 
};

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 localPosition	: POSITION;     // XYZ position
	float3 normal			: NORMAL;		// Normal vector
	float3 tangent			: TANGENT;		// Tangent vector
	float2 uv				: TEXCOORD;		// Texture coordinates
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition	: SV_POSITION;	// XYZW position (System Value Position)
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 worldPosition	: POSITION;
	float3 tangent			: TANGENT;
};

struct SkyVertexToPixel
{
	float4 position			: SV_POSITION;
	float3 sampleDir		: DIRECTION;
};

float Attenuate(Light light, float3 worldPos)
{
	float dist = distance(light.Position, worldPos);
	float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
	return att * att;
}

float3 Diffuse(float3 p_normal, Light light)
{
	// Normalizing and negating the direction of the light
	float3 dirToLight = normalize(-light.Direction);

	return saturate(dot(p_normal, dirToLight)) * light.Color;
}

float3 PointDiffuse(float3 p_normal, float3 p_dirToPointLight, Light light)
{
	return saturate(dot(p_normal, p_dirToPointLight)) * light.Color;
}

float3 PhongSpec(float3 p_normal, float3 p_V, float p_roughness, float specularMapScale, Light light)
{
	// Calculating reflection
	float3 R = reflect(normalize(light.Direction), p_normal);

	// Calculating exponent
	float specExponent = (1.0f - p_roughness) * MAX_SPECULAR_EXPONENT;

	float3 spec = float3(0, 0, 0);

	if (specExponent > 0.05f)
	{
		spec = pow(saturate(dot(R, p_V)), specExponent);
	}

	return spec * specularMapScale;
}

float3 PointPhongSpec(float3 p_normal, float3 p_dirToPointLight, float3 p_V, float p_roughness, float specularMapScale, Light light)
{
	// Calculating reflection
	float3 R = reflect(normalize(p_dirToPointLight), p_normal);

	// Calculating exponent
	float specExponent = (1.0f - p_roughness) * MAX_SPECULAR_EXPONENT;

	float3 spec = float3(0, 0, 0);

	if (specExponent > 0.05f)
	{
		spec = pow(saturate(dot(R, p_V)), specExponent);
	}

	return spec * specularMapScale;
}

float3 HandleLight(Light p_light, float3 p_normal, float3 p_V, float p_roughness, float specularMapScale, float3 p_pixelWorldPosition)
{
	switch (p_light.Type)
	{
	case LIGHT_TYPE_DIRECTIONAL:
		float3 diffuse = Diffuse(p_normal, p_light);
		float3 spec = PhongSpec(p_normal, p_V, p_roughness, specularMapScale, p_light);
		spec *= any(diffuse);
		return spec + diffuse;
		break;

	case LIGHT_TYPE_POINT:
		float3 dirToPointLight = normalize(p_light.Position - p_pixelWorldPosition);
		float3 diff = PointDiffuse(p_normal, dirToPointLight, p_light);
		float3 specularity = PointPhongSpec(p_normal, dirToPointLight, p_V, p_roughness, specularMapScale, p_light);
		specularity *= any(diff);
		return Attenuate(p_light, p_pixelWorldPosition) * (specularity + diff);
		break;

	case LIGHT_TYPE_SPOT:
		return float3(0, 0, 0);
		break;

	default:
		return float3(0, 0, 0);
		break;
	}
}

#endif