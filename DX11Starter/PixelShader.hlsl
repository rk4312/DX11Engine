#include "ShaderInclude.hlsli"

Texture2D Albedo			: register(t0);  // "t" registers for textures
//Texture2D SpecularMap		: register(t1);
Texture2D NormalMap			: register(t1);  
Texture2D RoughnessMap		: register(t2);
Texture2D MetalnessMap		: register(t3);
SamplerState BasicSampler	: register(s0); // "s" registers for samplers

// Declaring constant buffer
cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float roughness;
	float3 cameraPosition;
	float3 ambient;

	Light lightsArray[5];
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Sampling texture
	float3 surfaceColor = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f);
	float3 tintedSurfaceColor = surfaceColor * colorTint;

	// Sampling specular map
	//float surfaceSpec = SpecularMap.Sample(BasicSampler, input.uv).r;

	// Sampling normal map
	float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
	
	// Sampling roughness map
	float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;

	// Sampling metalness map
	float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;

	// Specular color determination -----------------
	// Assume albedo texture is actually holding specular color where metalness == 1
	// Note the use of lerp here - metal is generally 0 or 1, but might be in between
	// because of linear texture sampling, so lerp the specular color to match
	float3 specularColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metalness);

	// Feel free to adjust/simplify this code to fit with your existing shader(s)
	// Simplifications include not re-normalizing the same vector more than once!
	float3 N = normalize(input.normal); // Must be normalized here or before
	float3 T = normalize(input.tangent); // Must be normalized here or before
	T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);
	input.normal = mul(unpackedNormal, TBN);

	// Calculating view vector
	float3 V = normalize(cameraPosition - input.worldPosition);

	float3 finalColor = ambient * surfaceColor.rgb;

	// Looping through all lights
	for (int i = 0; i < 5; i++)
	{
		finalColor += HandleLightPBR(lightsArray[i], input.normal, V, roughness, metalness, specularColor, input.worldPosition, surfaceColor);
	}

	return float4(pow(finalColor, 1.0f / 2.2f), 1);
}