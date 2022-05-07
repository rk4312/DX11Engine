#pragma once

#include <wrl/client.h> 
#include <WICTextureLoader.h>
#include <memory>

#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"

class Sky
{
public:
	Sky(std::shared_ptr<Mesh> p_mesh, 
		Microsoft::WRL::ComPtr<ID3D11SamplerState> p_sampler,
		Microsoft::WRL::ComPtr<ID3D11Device> device, 
		std::shared_ptr<SimplePixelShader> pixelShader, 
		std::shared_ptr<SimpleVertexShader> vertexShader, 
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapTextureSRV);
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<Camera> camera);

private:

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<SimplePixelShader> ps;
	std::shared_ptr<SimpleVertexShader> vs;
};

