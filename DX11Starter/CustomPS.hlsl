#include "ShaderInclude.hlsli"

// Declaring constant buffer
cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float totalTime;
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
	return float4(totalTime % 1.5f * 0.75f, totalTime % 1.75f * 0.5f, totalTime % 2.0f, 1.0f);
}