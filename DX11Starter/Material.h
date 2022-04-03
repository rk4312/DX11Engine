#pragma once

#include <DirectXMath.h>
#include <memory>			// Used for smart pointers
#include <unordered_map>
#include "SimpleShader.h"

class Material
{
public:
	Material(DirectX::XMFLOAT4 color, float roughness, std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps);
	~Material();
	DirectX::XMFLOAT4 GetColor();
	float GetRoughness();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	void SetColor(DirectX::XMFLOAT4 newColor);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> newVS);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> newPS);
	void AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void PrepareMaterial();

private:
	DirectX::XMFLOAT4 color;					// Color tint
	float roughness;							// Roughness that determines specularity
	std::shared_ptr<SimpleVertexShader> vs;		// Shared pointer for Vertex Shader
	std::shared_ptr<SimplePixelShader> ps;		// Shared pointer for Pixel Shader

	// Hash Maps for SRVs and sampler states
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> specularSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
};

