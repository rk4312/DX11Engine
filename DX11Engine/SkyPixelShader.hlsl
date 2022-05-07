#include "ShaderInclude.hlsli"

TextureCube SkyCubeMap		: register(t0);
SamplerState SkySampler		: register(s0);

float4 main(SkyVertexToPixel input) : SV_TARGET
{
	return SkyCubeMap.Sample(SkySampler, input.sampleDir);
}