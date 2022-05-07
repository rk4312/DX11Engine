#include "GameEntity.h"
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

GameEntity::GameEntity(shared_ptr<Mesh> meshPtr, std::shared_ptr<Material> matPtr)
{
	mesh = meshPtr;
	mat = matPtr;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

Transform* GameEntity::GetTransform()
{
	return &transform;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return mat;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> newMatPtr)
{
	mat = newMatPtr;
}

void GameEntity::Draw(std::shared_ptr<Camera> p_camera, DirectX::XMFLOAT3 p_ambient, std::vector<Light> lightsVector)
{
	std::shared_ptr<SimpleVertexShader> vs = mat->GetVertexShader();

	vs->SetMatrix4x4("world", transform.GetWorldMatrix());
	vs->SetMatrix4x4("worldInvTranspose", transform.GetWorldInverseTransposeMatrix());
	vs->SetMatrix4x4("view", p_camera->GetViewMatrix());
	vs->SetMatrix4x4("projection", p_camera->GetProjectionMatrix());

	vs->CopyAllBufferData();

	std::shared_ptr<SimplePixelShader> ps = mat->GetPixelShader();

	mat->PrepareMaterial();

	ps->SetFloat4("colorTint", mat->GetColor());
	ps->SetFloat("roughness", mat->GetRoughness());
	ps->SetFloat3("cameraPosition", p_camera->GetTransform()->GetPosition());
	ps->SetFloat3("ambient", p_ambient);
	ps->SetFloat("numLights", lightsVector.size());

	for (int i = 0; i < lightsVector.size(); i++)
	{
		ps->SetData("lightsArray", &lightsVector[0], sizeof(Light) * (int)lightsVector.size());
	}

	ps->CopyAllBufferData();

	mat->GetVertexShader()->SetShader();
	mat->GetPixelShader()->SetShader();

	// Calling draw on custom meshes
	mesh->Draw();
}
