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
	ambientLight = XMFLOAT3(0.1f, 0.1f, 0.25f);

	// Red directional light
	Light directionalLight1 = {};
	directionalLight1.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight1.Direction = XMFLOAT3(1, 0, 0);
	directionalLight1.Color = XMFLOAT3(0.4, 0.4, 0.4);
	directionalLight1.Intensity = 1.0f;

	lightsVector.push_back(directionalLight1);

	// Green directional light
	Light directionalLight2 = {};
	directionalLight2.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.Direction = XMFLOAT3(0, -1, 0);
	directionalLight2.Color = XMFLOAT3(0.4, 0.4, 0.4);
	directionalLight2.Intensity = 1.0f;

	lightsVector.push_back(directionalLight2);

	// Blue directional light
	Light directionalLight3 = {};
	directionalLight3.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight3.Direction = XMFLOAT3(-1, 1, -0.5);
	directionalLight3.Color = XMFLOAT3(0.4, 0.4, 0.4);
	directionalLight3.Intensity = 1.0f;

	lightsVector.push_back(directionalLight3);

	// White point light
	Light pointLight1 = {};
	pointLight1.Type = LIGHT_TYPE_POINT;
	pointLight1.Color = XMFLOAT3(0.4, 0.4, 0.4);
	pointLight1.Intensity = 1.0f;
	pointLight1.Range = 10.0f;
	pointLight1.Position = XMFLOAT3(0.0f, 0.0f, 2.0f);

	lightsVector.push_back(pointLight1);

	// Red point light
	Light pointLight2 = {};
	pointLight2.Type = LIGHT_TYPE_POINT;
	pointLight2.Color = XMFLOAT3(0.4, 0.4, 0.4);
	pointLight2.Intensity = 1.0f;
	pointLight2.Range = 10.0f;
	pointLight2.Position = XMFLOAT3(0.0f, 0.0f, -2.0f);

	lightsVector.push_back(pointLight2);

	// Loading Textures and Specular Maps
	CreateWICTextureFromFile(device.Get(), 
		context.Get(), 
		GetFullPathTo_Wide(L"../../Assets/Textures/WhitePlasterTexture.png").c_str(), 
		nullptr, 
		whitePlasterTextureSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/WhitePlasterSpec.png").c_str(),
		nullptr,
		whitePlasterSpecSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/WhitePlasterNormal.png").c_str(),
		nullptr,
		whitePlasterNormalSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/BrickTexture.png").c_str(),
		nullptr,
		brickTextureSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/BrickSpec.png").c_str(),
		nullptr,
		brickSpecSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/Textures/BrickNormal.png").c_str(),
		nullptr,
		brickNormalSRV.GetAddressOf());

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
	brickMat = std::make_shared<Material>(white, 0.15f, vertexShader, pixelShader);
	brickMat->AddTextureSRV("SurfaceTexture", brickTextureSRV);
	brickMat->AddTextureSRV("SpecularMap", brickSpecSRV);
	brickMat->AddTextureSRV("NormalMap", brickNormalSRV);
	brickMat->AddSampler("BasicSampler", basicSampler);

	whitePlasterMat = std::make_shared<Material>(white, 0.15f, vertexShader, pixelShader);
	whitePlasterMat->AddTextureSRV("SurfaceTexture", whitePlasterTextureSRV);
	whitePlasterMat->AddTextureSRV("SpecularMap", whitePlasterSpecSRV);
	whitePlasterMat->AddTextureSRV("NormalMap", whitePlasterNormalSRV);
	whitePlasterMat->AddSampler("BasicSampler", basicSampler);

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

	// Arranging entities in a line
	for (int i = 0; i < gameEntitiesVector.size(); i++)
	{
		gameEntitiesVector[i].GetTransform()->MoveAbsolute(i * 0.8f - 2.35f, 0.0f, 0.0f);
	}

	// Rotating entities with 2d mesh so its visible from default camera angle
	gameEntitiesVector[0].GetTransform()->Rotate(0, 0, 0.75f);
	gameEntitiesVector[6].GetTransform()->Rotate(0, 0, 0.75f);

	// Initialzing skybox
	skybox = std::make_shared<Sky>(meshVector[1], basicSampler, device, skyPixelShader, skyVertexShader, skyCubeMapSRV);
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

	gameEntitiesVector.push_back(GameEntity(meshVector[5], whitePlasterMat));
	gameEntitiesVector.push_back(GameEntity(meshVector[6], brickMat));
	gameEntitiesVector.push_back(GameEntity(meshVector[2], whitePlasterMat));
	gameEntitiesVector.push_back(GameEntity(meshVector[0], brickMat));
	gameEntitiesVector.push_back(GameEntity(meshVector[1], whitePlasterMat));
	gameEntitiesVector.push_back(GameEntity(meshVector[3], brickMat));
	gameEntitiesVector.push_back(GameEntity(meshVector[4], whitePlasterMat));
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

	// Applying a transformation to each entity
	for (int i = 0; i < gameEntitiesVector.size(); i++)
	{
		gameEntitiesVector[i].GetTransform()->Rotate(0, deltaTime * 0.1f, 0);
	}
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

	// Call draw on all game entities before drawing skybox
	for (int i = 0; i < gameEntitiesVector.size(); i++)
	{
		gameEntitiesVector[i].Draw(camera, totalTime, ambientLight, lightsVector);
	}

	// Drawing skybox after all other game entities
	skybox->Draw(context, camera);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);


	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}