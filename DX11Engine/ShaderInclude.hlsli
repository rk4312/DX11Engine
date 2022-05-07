#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier! 
#define __GGP_SHADER_INCLUDES__ 

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2

#define MAX_SPECULAR_EXPONENT		256.0f 

// The fresnel value for non-metals (dielectrics)
// Page 9: "F0 of nonmetals is now a constant 0.04"
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal

// Handy to have this as a constant
static const float PI = 3.14159265359f;

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

// Output struct that accomodates for shadow map information
struct VertexToPixelWithShadowPos
{
	float4 screenPosition	: SV_POSITION;	// XYZW position (System Value Position)
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 worldPosition	: POSITION;
	float3 tangent			: TANGENT;
	float4 shadowMapPos		: SHADOWPOS;
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


// PBR FUNCTIONS ================

// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
float DiffusePBR(float3 normal, float3 dirToLight)
{
	return saturate(dot(normal, normalize(dirToLight)));
}


// Calculates diffuse amount based on energy conservation
//
// diffuse - Diffuse amount
// specular - Specular color (including light color)
// metalness - surface metalness amount
//
// Metals should have an albedo of (0,0,0)...mostly
// See slide 65: http://blog.selfshadow.com/publications/s2014-shading-course/hoffman/s2014_pbs_physics_math_slides.pdf
float3 DiffuseEnergyConserve(float3 diffuse, float3 specular, float metalness)
{
	return diffuse * ((1 - saturate(specular)) * (1 - metalness));
}


// GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector
// n - Normal
// 
// D(h, n) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float SpecDistribution(float3 n, float3 h, float roughness)
{
	// Pre-calculations
	float NdotH = saturate(dot(n, h));
	float NdotH2 = NdotH * NdotH;
	float a = roughness * roughness;
	float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!

	// ((n dot h)^2 * (a^2 - 1) + 1)
	float denomToSquare = NdotH2 * (a2 - 1) + 1;
	// Can go to zero if roughness is 0 and NdotH is 1; MIN_ROUGHNESS helps here

	// Final value
	return a2 / (PI * denomToSquare * denomToSquare);
}


// Fresnel term - Schlick approx.
// 
// v - View vector
// h - Half vector
// f0 - Value when l = n (full specular color)
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 Fresnel(float3 v, float3 h, float3 f0)
{
	// Pre-calculations
	float VdotH = saturate(dot(v, h));

	// Final value
	return f0 + (1 - f0) * pow(1 - VdotH, 5);
}


// Geometric Shadowing - Schlick-GGX (based on Schlick-Beckmann)
// - k is remapped to a / 2, roughness remapped to (r+1)/2
//
// n - Normal
// v - View vector
//
// G(l,v)
float GeometricShadowing(float3 n, float3 v, float roughness)
{
	// End result of remapping:
	float k = pow(roughness + 1, 2) / 8.0f;
	float NdotV = saturate(dot(n, v));

	// Final value
	return NdotV / (NdotV * (1 - k) + k);
}


// Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - part of the denominator are canceled out by numerator (see below)
//
// D() - Spec Dist - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 specColor)
{
	// Other vectors
	float3 h = normalize(v + l);

	// Grab various functions
	float D = SpecDistribution(n, h, roughness);
	float3 F = Fresnel(v, h, specColor);
	float G = GeometricShadowing(n, v, roughness) * GeometricShadowing(n, l, roughness);

	// Final formula
	// Denominator dot products partially canceled by G()!
	// See page 16: http://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf
	return (D * F * G) / (4 * max(dot(n, v), dot(n, l)));
}

// Handles PBR point light
float3 HandlePointLightPBR(Light light, float3 normal, float3 viewVector, float roughness, float metalness, float3 specColor, float3 pixelWorldPosition, float3 surfaceColor)
{
	float3 toLight = normalize(light.Position - pixelWorldPosition);

	// Calculate the light amounts
	float diff = DiffusePBR(normal, toLight);
	float3 spec = MicrofacetBRDF(normal, toLight, viewVector, roughness, specColor);

	// Calculate diffuse with energy conservation
	// (Reflected light doesn't get diffused)
	float3 balancedDiff = DiffuseEnergyConserve(diff, spec, metalness);

	// Combine the final diffuse and specular values for this light
	return (balancedDiff * surfaceColor + spec) * Attenuate(light, pixelWorldPosition) * light.Intensity * light.Color;
}

// Handles PBR directional light
float3 HandleDirectionalLightPBR(Light light, float3 normal, float3 viewVector, float roughness, float metalness, float3 specColor, float3 pixelWorldPosition, float3 surfaceColor)
{
	float3 toLight = normalize(-light.Direction);

	// Calculate the light amounts
	float diff = DiffusePBR(normal, toLight);
	float3 spec = MicrofacetBRDF(normal, toLight, viewVector, roughness, specColor);

	// Calculate diffuse with energy conservation
	// (Reflected light doesn't get diffused)
	float3 balancedDiff = DiffuseEnergyConserve(diff, spec, metalness);

	// Combine the final diffuse and specular values for this light
	return (balancedDiff * surfaceColor + spec) * light.Intensity * light.Color;
}

// Handles lighting depending on type of light
float3 HandleLightPBR(Light light, float3 normal, float3 viewVector, float roughness, float metalness, float3 specColor, float3 pixelWorldPosition, float3 surfaceColor)
{
	switch (light.Type)
	{
	case LIGHT_TYPE_DIRECTIONAL:
		return HandleDirectionalLightPBR(light, normal, viewVector, roughness, metalness, specColor, pixelWorldPosition, surfaceColor);
		break;

	case LIGHT_TYPE_POINT:
		return HandlePointLightPBR(light, normal, viewVector, roughness, metalness, specColor, pixelWorldPosition, surfaceColor);
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