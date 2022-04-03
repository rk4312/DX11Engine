#include "ShaderInclude.hlsli"

Texture2D SurfaceTexture	: register(t0);  // "t" registers for textures
Texture2D SpecularMap		: register(t1);
Texture2D NormalMap			: register(t2);  
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
	float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
	float3 tintedSurfaceColor = surfaceColor * colorTint;

	// Sampling specular map
	float surfaceSpec = SpecularMap.Sample(BasicSampler, input.uv).r;

	// Sampling normal map
	float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
	
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

	float3 diffuseAndSpecOfAllLights = float3(0, 0, 0);

	// Looping through all lights
	for (int i = 0; i < 5; i++)
	{
		diffuseAndSpecOfAllLights += HandleLight(lightsArray[i], input.normal, V, roughness, surfaceSpec, input.worldPosition);
	}

	// Calculating final color
	float3 finalColor = (ambient + diffuseAndSpecOfAllLights) * tintedSurfaceColor;

	return float4(finalColor, 1);
}