#pragma once
#include <DirectXMath.h>

class Transform
{
public:
	Transform();
	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();
	void MoveAbsolute(float x, float y, float z);
	void MoveRelative(float x, float y, float z);
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);

private:
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotationPitchYawRoll;
	DirectX::XMFLOAT3 rightVec;
	DirectX::XMFLOAT3 upVec;
	DirectX::XMFLOAT3 forwardVec;
	bool dirtyMatrix;
};

