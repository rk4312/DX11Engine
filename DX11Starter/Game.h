#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>		// Used for smart pointers
#include "Mesh.h"
#include "GameEntity.h"
#include <vector>
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Lights.h"
#include "Sky.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Should we use vsync to limit the frame rate?
	bool vsync;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateBasicGeometry();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;

	// Custom Pixel Shader
	std::shared_ptr<SimplePixelShader> customPixelShader;

	// Game Entities Vector
	std::vector<GameEntity> gameEntitiesVector;

	// Mesh Vector
	std::vector<std::shared_ptr<Mesh>> meshVector;

	// Camera
	std::shared_ptr<Camera> camera;

	// Materials
	std::shared_ptr<Material> metalMat;
	std::shared_ptr<Material> stoneMat;
	std::shared_ptr<Material> customPSWhiteMat;

	// Lights
	DirectX::XMFLOAT3 ambientLight;
	std::vector<Light> lightsVector;

	// Shader Resource View for textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalTextureSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalRoughnessSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalMetalnessSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stoneTextureSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stoneNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stoneRoughnessSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stoneMetalnessSRV;

	// Sampler State
	Microsoft::WRL::ComPtr<ID3D11SamplerState> basicSampler;

	// Sky
	std::shared_ptr<Sky> skybox;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyCubeMapSRV;
	std::shared_ptr<SimplePixelShader> skyPixelShader;
	std::shared_ptr<SimpleVertexShader> skyVertexShader;
};

