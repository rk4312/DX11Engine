#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include "Camera.h"
#include <memory>
#include <d3d11.h>
#include <wrl/client.h>
#include "Material.h"
#include "Lights.h"

class GameEntity
{
public:
	GameEntity(std::shared_ptr<Mesh> meshPtr, std::shared_ptr<Material> matPtr);
	std::shared_ptr<Mesh> GetMesh();
	Transform* GetTransform();
	std::shared_ptr<Material> GetMaterial();
	void SetMaterial(std::shared_ptr<Material> newMatPtr);
	void Draw(std::shared_ptr<Camera> p_camera, float totalTime, DirectX::XMFLOAT3 p_ambient, std::vector<Light> lightsVector);
	

private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> mat;
};

