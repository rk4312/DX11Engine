#include "Material.h"

using namespace DirectX;

Material::Material(DirectX::XMFLOAT4 p_color, float p_roughness, std::shared_ptr<SimpleVertexShader> p_vs, std::shared_ptr<SimplePixelShader> p_ps)
{
	color = p_color;
	roughness = p_roughness;
	vs = p_vs;
	ps = p_ps;
}

Material::~Material()
{
}

DirectX::XMFLOAT4 Material::GetColor()
{
	return color;
}

float Material::GetRoughness()
{
	return roughness;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vs;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return ps;
}

void Material::SetColor(DirectX::XMFLOAT4 newColor)
{
	color = newColor;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> newVS)
{
	vs = newVS;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> newPS)
{
	ps = newPS;
}

void Material::AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ name, srv });
}

void Material::AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({ name, sampler });
}

void Material::PrepareMaterial()
{
	for (auto& t : textureSRVs) { ps->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : samplers) { ps->SetSamplerState(s.first.c_str(), s.second); }
}
