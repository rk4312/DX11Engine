#include "Sky.h"

using namespace DirectX;

Sky::Sky(std::shared_ptr<Mesh> p_mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> p_sampler, Microsoft::WRL::ComPtr<ID3D11Device> device, std::shared_ptr<SimplePixelShader> pixelShader, std::shared_ptr<SimpleVertexShader> vertexShader, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapTextureSRV)
{
	// Passing in shaders and texture from game class
	mesh = p_mesh;
	sampler = p_sampler;
	vs = vertexShader;
	ps = pixelShader;
	cubeMapSRV = cubeMapTextureSRV;

	// Dexcribing and creating rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;

	device->CreateRasterizerState(&rasterizerDesc, rasterizer.GetAddressOf());

	// Describing and creating depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
}

void Sky::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<Camera> camera)
{
	// Setting render states
	deviceContext->RSSetState(rasterizer.Get());
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

	// Sending data to vertex shader constant  buffer
	vs->SetMatrix4x4("view", camera->GetViewMatrix());
	vs->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	vs->CopyAllBufferData();

	// Setting active srv and sampler state
	ps->SetShaderResourceView("SkyCubeMap", cubeMapSRV);
	ps->SetSamplerState("SkySampler", sampler);

	// Setting active shaders and drawing
	vs->SetShader();
	ps->SetShader();

	mesh->Draw();

	// Resetting render states
	deviceContext->RSSetState(nullptr);
	deviceContext->OMSetDepthStencilState(nullptr, 0);
}
