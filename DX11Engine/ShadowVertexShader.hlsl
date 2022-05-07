#include "ShaderInclude.hlsli"

// Declaring constant buffer
cbuffer ExternalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

struct ShadowVertexToPixel
{
	float4 screenPos : SV_POSITION;
};

ShadowVertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	ShadowVertexToPixel output;

	// creating world view projection matrix
	matrix wvp = mul(mul(projection, view), world);
	output.screenPos = mul(wvp, float4(input.localPosition, 1.0f));

	return output;
}