#include "ShaderInclude.hlsli"

// Declaring constant buffer
cbuffer ExternalData : register(b0)
{
	matrix view;
	matrix projection;
}

SkyVertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	SkyVertexToPixel output;

	// Removing translation from view matrix
	matrix viewNoTranslation = view;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	// Applying view and projection matrix to input position
	output.position = mul(mul(projection, viewNoTranslation), float4(input.localPosition, 1.0f));
	output.position.z = output.position.w;
	output.sampleDir = input.localPosition;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}