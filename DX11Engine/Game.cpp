#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "BufferStructs.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include <memory>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true),			   // Show extra stats (fps) in title bar?
	vsync(false)
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

	camera = std::make_shared<Camera>(0.0f, 0.0f, -5.0f, (float)width/height);

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object created in Game

}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Lights
	ambientLight = XMFLOAT3(0, 0, 0);

	Light directionalLight1 = {};
	directionalLight1.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight1.Direction = XMFLOAT3(0, -1, 1);
	directionalLight1.Color = XMFLOAT3(1, 1, 1);
	directionalLight1.Intensity = 0.75f;

	lightsVector.push_back(directionalLight1);

	Light directionalLight2 = {};
	directionalLight2.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.Direction = XMFLOAT3(0, -1, 0);
	directionalLight2.Color = XMFLOAT3(1, 1, 1);
	directionalLight2.Intensity = 0.25f;

	//lightsVector.push_back(directionalLight2);

	Light directionalLight3 = {};
	directionalLight3.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight3.Direction = XMFLOAT3(-1, 0, 0);
	directionalLight3.Color = XMFLOAT3(1, 1, 1);
	directionalLight3.Intensity = 0.5f;

	//lightsVector.push_back(directionalLight3);

	Light pointLight1 = {};
	pointLight1.Type = LIGHT_TYPE_POINT;
	pointLight1.Color = XMFLOAT3(1, 1, 1);
	pointLight1.Intensity = 0.5f;
	pointLight1.Range = 3.0f;
	pointLight1.Position = XMFLOAT3(3.25f, -1.5f, -1);

	lightsVector.push_back(pointLight1);

	Light pointLight2 = {};
	pointLight2.Type = LIGHT_TYPE_POINT;
	pointLight2.Color = XMFLOAT3(1, 1, 1);
	pointLight2.Intensity = 0.5f;
	pointLight2.Range = 3.0f;
	pointLight2.Position = XMFLOAT3(-3.25f, -1.5f, -1);

	lightsVector.push_back(pointLight2);

	Light pointLight3 = {};
	pointLight3.Type = LIGHT_TYPE_POINT;
	pointLight3.Color = XMFLOAT3(1, 1, 1);
	pointLight3.Intensity = 0.5f;
	pointLight3.Range = 3.0f;
	pointLight3.Position = XMFLOAT3(0, -1.2f, 5.5f);

	lightsVector.push_back(pointLight3);

	// Loading Textures and Specular Maps
	CreateWICTextureFromFile(device.Get(), 
		context.Get(), 
		GetFullPathTo_Wide(L"../../Assets/Textures/RoughCeramic/RoughCeramic_Old_1K_albedo.tif").c_str(), 
		nullptr, 
		cerTextureSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/RoughCeramic/RoughCeramic_Old_1K_normal.tif").c_str(),
		nullptr,
		cerNormalSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/RoughCeramic/RoughCeramic_Old_1K_roughness.tif").c_str(),
		nullptr,
		cerRoughnessSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/RoughCeramic/RoughCeramic_Old_1K_metallic.tif").c_str(),
		nullptr,
		cerMetalnessSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/HighRise4/HighRise4_1K_albedo.tif").c_str(),
		nullptr,
		hr4TextureSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/HighRise4/HighRise4_1K_normal.tif").c_str(),
		nullptr,
		hr4NormalSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/HighRise4/HighRise4_1K_roughness.tif").c_str(),
		nullptr,
		hr4RoughnessSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/HighRise4/HighRise4_1K_metallic.tif").c_str(),
		nullptr,
		hr4MetalnessSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/HighRise5/HighRise5_1K_albedo.tif").c_str(),
		nullptr,
		hr5TextureSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/HighRise5/HighRise5_1K_normal.tif").c_str(),
		nullptr,
		hr5NormalSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/HighRise5/HighRise5_1K_roughness.tif").c_str(),
		nullptr,
		hr5RoughnessSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/HighRise5/HighRise5_1K_metallic.tif").c_str(),
		nullptr,
		hr5MetalnessSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/Stone/cobblestone_albedo.png").c_str(),
		nullptr,
		stoneTextureSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/Stone/cobblestone_normals.png").c_str(),
		nullptr,
		stoneNormalSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/Stone/cobblestone_roughness.png").c_str(),
		nullptr,
		stoneRoughnessSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/Stone/cobblestone_metal.png").c_str(),
		nullptr,
		stoneMetalnessSRV.GetAddressOf());

	CreateDDSTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/SunnyCubeMap.dds").c_str(),
		nullptr,
		skyCubeMapSRV.GetAddressOf());

	// Creating a sampler state
	D3D11_SAMPLER_DESC basicSamplerDescription = {};
	basicSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	basicSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	basicSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	basicSamplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;
	basicSamplerDescription.MaxAnisotropy = 16;
	basicSamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&basicSamplerDescription, basicSampler.GetAddressOf());

	// Creating Materials
	cerMat = std::make_shared<Material>(white, 0.15f, vertexShader, pixelShader);
	cerMat->AddTextureSRV("Albedo", cerTextureSRV);
	cerMat->AddTextureSRV("NormalMap", cerNormalSRV);
	cerMat->AddTextureSRV("RoughnessMap", cerRoughnessSRV);
	cerMat->AddTextureSRV("MetalnessMap", cerMetalnessSRV);
	cerMat->AddSampler("BasicSampler", basicSampler);

	hr4Mat = std::make_shared<Material>(white, 0.15f, vertexShader, pixelShader);
	hr4Mat->AddTextureSRV("Albedo", hr4TextureSRV);
	hr4Mat->AddTextureSRV("NormalMap", hr4NormalSRV);
	hr4Mat->AddTextureSRV("RoughnessMap", hr4RoughnessSRV);
	hr4Mat->AddTextureSRV("MetalnessMap", hr4MetalnessSRV);
	hr4Mat->AddSampler("BasicSampler", basicSampler);

	hr5Mat = std::make_shared<Material>(white, 0.15f, vertexShader, pixelShader);
	hr5Mat->AddTextureSRV("Albedo", hr5TextureSRV);
	hr5Mat->AddTextureSRV("NormalMap", hr5NormalSRV);
	hr5Mat->AddTextureSRV("RoughnessMap", hr5RoughnessSRV);
	hr5Mat->AddTextureSRV("MetalnessMap", hr5MetalnessSRV);
	hr5Mat->AddSampler("BasicSampler", basicSampler);

	stoneMat = std::make_shared<Material>(white, 0.15f, vertexShader, pixelShader);
	stoneMat->AddTextureSRV("Albedo", stoneTextureSRV);
	stoneMat->AddTextureSRV("NormalMap", stoneNormalSRV);
	stoneMat->AddTextureSRV("RoughnessMap", stoneRoughnessSRV);
	stoneMat->AddTextureSRV("MetalnessMap", stoneMetalnessSRV);
	stoneMat->AddSampler("BasicSampler", basicSampler);

	customPSWhiteMat = std::make_shared<Material>(white, 0.15f, vertexShader, customPixelShader);

	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Setting the scale of the entities
	for (int i = 0; i < gameEntitiesVector.size(); i++) 
	{
		gameEntitiesVector[i].GetTransform()->SetScale(0.25f, 0.25f, 0.25f);
	}

	// Setting the correct transform for the 'traveler' sphere
	gameEntitiesVector[0].GetTransform()->SetScale(1.75f, 1.75f, 1.75f);
	gameEntitiesVector[0].GetTransform()->Rotate(0, 0, 0);
	gameEntitiesVector[0].GetTransform()->SetPosition(0, 1.2f, 1.8f);

	// Setting the correct transform for the 'floor' quad
	gameEntitiesVector[1].GetTransform()->SetScale(4, 1, 4);
	gameEntitiesVector[1].GetTransform()->Rotate(0, 0, 0);
	gameEntitiesVector[1].GetTransform()->SetPosition(0, -2, 3);

	// Front right quadrant of buildings
	gameEntitiesVector[2].GetTransform()->SetScale(0.25f, 0.75f, 0.3f);
	gameEntitiesVector[2].GetTransform()->SetPosition(3.25f, -1.25f, -0.15f);

	gameEntitiesVector[3].GetTransform()->SetScale(0.25f, 0.5f, 0.3f);
	gameEntitiesVector[3].GetTransform()->SetPosition(2.5f, -1.5f, 0);

	gameEntitiesVector[4].GetTransform()->SetScale(0.25f, 0.6f, 0.3f);
	gameEntitiesVector[4].GetTransform()->SetPosition(1.75f, -1.5f, -0.2f);

	gameEntitiesVector[5].GetTransform()->SetScale(0.35f, 0.4f, 0.2f);
	gameEntitiesVector[5].GetTransform()->SetPosition(2, -1.5f, 0.6f);

	gameEntitiesVector[6].GetTransform()->SetScale(0.3f, 0.65f, 0.2f);
	gameEntitiesVector[6].GetTransform()->SetPosition(3, -1.5f, 0.6f);

	// Back right quadrant of buildings
	gameEntitiesVector[7].GetTransform()->SetScale(0.25f, 0.75f, 0.3f);
	gameEntitiesVector[7].GetTransform()->SetPosition(3.25f, -1.25f, 6);

	gameEntitiesVector[8].GetTransform()->SetScale(0.25f, 0.5f, 0.3f);
	gameEntitiesVector[8].GetTransform()->SetPosition(2.5f, -1.5f, 5.85f);

	gameEntitiesVector[9].GetTransform()->SetScale(0.25f, 0.6f, 0.3f);
	gameEntitiesVector[9].GetTransform()->SetPosition(1.75f, -1.5f, 6);

	gameEntitiesVector[10].GetTransform()->SetScale(0.35f, 0.4f, 0.2f);
	gameEntitiesVector[10].GetTransform()->SetPosition(2, -1.8, 5);

	gameEntitiesVector[11].GetTransform()->SetScale(0.3f, 0.65f, 0.2f);
	gameEntitiesVector[11].GetTransform()->SetPosition(3, -1.5f, 5);

	// Back left quadrant of buildings
	gameEntitiesVector[12].GetTransform()->SetScale(0.25f, 0.75f, 0.3f);
	gameEntitiesVector[12].GetTransform()->SetPosition(-3.25f, -1.25f, 6);

	gameEntitiesVector[13].GetTransform()->SetScale(0.25f, 0.5f, 0.3f);
	gameEntitiesVector[13].GetTransform()->SetPosition(-2.5f, -1.5f, 5.85f);

	gameEntitiesVector[14].GetTransform()->SetScale(0.25f, 0.6f, 0.3f);
	gameEntitiesVector[14].GetTransform()->SetPosition(-1.75f, -1.5f, 6);

	gameEntitiesVector[15].GetTransform()->SetScale(0.35f, 0.4f, 0.2f);
	gameEntitiesVector[15].GetTransform()->SetPosition(-2, -1.8, 5);

	gameEntitiesVector[16].GetTransform()->SetScale(0.3f, 0.65f, 0.2f);
	gameEntitiesVector[16].GetTransform()->SetPosition(-3, -1.5f, 5);
	
	// Front left quadrant of buildings
	gameEntitiesVector[17].GetTransform()->SetScale(0.25f, 0.75f, 0.3f);
	gameEntitiesVector[17].GetTransform()->SetPosition(-3.25f, -1.25f, -0.15f);

	gameEntitiesVector[18].GetTransform()->SetScale(0.25f, 0.5f, 0.3f);
	gameEntitiesVector[18].GetTransform()->SetPosition(-2.5f, -1.5f, 0);

	gameEntitiesVector[19].GetTransform()->SetScale(0.25f, 0.6f, 0.3f);
	gameEntitiesVector[19].GetTransform()->SetPosition(-1.75f, -1.5f, -0.2f);

	gameEntitiesVector[20].GetTransform()->SetScale(0.35f, 0.4f, 0.2f);
	gameEntitiesVector[20].GetTransform()->SetPosition(-2, -1.5f, 0.6f);

	gameEntitiesVector[21].GetTransform()->SetScale(0.3f, 0.65f, 0.2f);
	gameEntitiesVector[21].GetTransform()->SetPosition(-3, -1.5f, 0.6f);

	// Initialzing skybox
	skybox = std::make_shared<Sky>(meshVector[1], basicSampler, device, skyPixelShader, skyVertexShader, skyCubeMapSRV);

	// Setting up shadow map resources
	CreateShadowMapResources();
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, 
		GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context,
		GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	customPixelShader = std::make_shared<SimplePixelShader>(device, context,
		GetFullPathTo_Wide(L"CustomPS.cso").c_str());
	skyPixelShader = std::make_shared<SimplePixelShader>(device, context,
		GetFullPathTo_Wide(L"SkyPixelShader.cso").c_str());
	skyVertexShader = std::make_shared<SimpleVertexShader>(device, context,
		GetFullPathTo_Wide(L"SkyVertexShader.cso").c_str());
	shadowVertexShader = std::make_shared<SimpleVertexShader>(device, context,
		GetFullPathTo_Wide(L"ShadowVertexShader.cso").c_str());
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	meshVector.push_back(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device, context));
	meshVector.push_back(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device, context));
	meshVector.push_back(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cylinder.obj").c_str(), device, context));
	meshVector.push_back(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device, context));
	meshVector.push_back(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/quad.obj").c_str(), device, context));
	meshVector.push_back(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/quad_double_sided.obj").c_str(), device, context));
	meshVector.push_back(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/torus.obj").c_str(), device, context));

	// Main sphere
	gameEntitiesVector.push_back(GameEntity(meshVector[0], cerMat));

	// Floor
	gameEntitiesVector.push_back(GameEntity(meshVector[5], stoneMat));

	// Building Cubes
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr5Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr5Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr5Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr5Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr5Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr5Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr5Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr5Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr5Mat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], hr4Mat));
}

void Game::CreateShadowMapResources()
{
	shadowMapResolution = 1024;
	shadowProjectionSize = 7.0f;

	// Defining shadow description
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapResolution;
	shadowDesc.Height = shadowMapResolution;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

	// Defining depth stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(shadowTexture.Get(), &shadowDSDesc, shadowDSV.GetAddressOf());

	// Shadow shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowTexture.Get(), &srvDesc, shadowSRV.GetAddressOf());

	// Defining custom sampler for rendering shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; 
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000;
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	// View
	XMMATRIX shView = XMMatrixLookAtLH(
		XMVectorSet(0, 20, -20, 0),
		XMVectorSet(0, 0, 0, 0),
		XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&shadowViewMatrix, shView);

	// Change shadowProjectionSize to fit entire scene
	XMMATRIX shProj = XMMatrixOrthographicLH(shadowProjectionSize, shadowProjectionSize, 0.1f, 100.0f);
	XMStoreFloat4x4(&shadowProjectionMatrix, shProj);
}

void Game::RenderShadowMap()
{
	// Set up pipeline to render shadow map
	context->OMSetRenderTargets(0, 0, shadowDSV.Get());
	context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadowRasterizer.Get());
	
	// Set up viewport
	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = (float)shadowMapResolution;
	vp.Height = (float)shadowMapResolution;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	// Turn on custom shadow vertex shader
	shadowVertexShader->SetShader();
	shadowVertexShader->SetMatrix4x4("view", shadowViewMatrix);
	shadowVertexShader->SetMatrix4x4("projection", shadowProjectionMatrix);
	context->PSSetShader(0, 0, 0);

	// Loop through all objects and draw shadows
	for (int i = 0; i < gameEntitiesVector.size(); i++)
	{
		// Set world matrix for vertex shader calculation
		shadowVertexShader->SetMatrix4x4("world", gameEntitiesVector[i].GetTransform()->GetWorldMatrix());
		shadowVertexShader->CopyAllBufferData();
		
		// Draw
		gameEntitiesVector[i].GetMesh()->Draw();
	}

	// Reset render states
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
	vp.Width = (float)this->width;
	vp.Height = (float)this->height;
	context->RSSetViewports(1, &vp);
	context->RSSetState(0);
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	if (camera != nullptr) 
	{
		// Updating projection matrix on window resize
		camera->UpdateProjectionMatrix((float)this->width / this->height);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	// Calling camera update
	camera->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Render the shadow map (shadow mapping is a 'pre-process')
	RenderShadowMap();

	// Passing shadow information to shaders
	vertexShader->SetMatrix4x4("lightView", shadowViewMatrix);
	vertexShader->SetMatrix4x4("lightProj", shadowProjectionMatrix);

	pixelShader->SetShaderResourceView("ShadowMap", shadowSRV);
	pixelShader->SetSamplerState("ShadowSampler", shadowSampler);

	// Call draw on all game entities before drawing skybox
	for (int i = 0; i < gameEntitiesVector.size(); i++)
	{
		gameEntitiesVector[i].Draw(camera, ambientLight, lightsVector);
	}

	// Drawing skybox after all other game entities
	skybox->Draw(context, camera);

	// Essentially resetting the shadow map resource
	ID3D11ShaderResourceView* nullSRVs[16] = {};
	context->PSSetShaderResources(0, 16, nullSRVs);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}