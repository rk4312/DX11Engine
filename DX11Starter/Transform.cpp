#include "Transform.h"

using namespace DirectX;

Transform::Transform()
{
    SetPosition(0, 0, 0);
    SetScale(1, 1, 1);
    SetRotation(0, 0, 0);
    XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
    dirtyMatrix = false;
}

void Transform::SetPosition(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;

    // Set dirty matrix as true whenever a transformation is made
    dirtyMatrix = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
    rotationPitchYawRoll.x = pitch;
    rotationPitchYawRoll.y = yaw;
    rotationPitchYawRoll.z = roll;

    dirtyMatrix = true;
}

void Transform::SetScale(float x, float y, float z)
{
    scale.x = x;
    scale.y = y;
    scale.z = z;

    dirtyMatrix = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
    return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
    return rotationPitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
    return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
    // Build new world matrix only if matrix is dirty (has unsaved changes)
    if (dirtyMatrix) {
        XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, position.z);
        XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(rotationPitchYawRoll.x, rotationPitchYawRoll.y, rotationPitchYawRoll.z);
        XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);
        XMMATRIX worldMat = scaleMat * rotationMat * translationMat;
        XMStoreFloat4x4(&worldMatrix, worldMat);
        XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(worldMat)));
        dirtyMatrix = false;
    }
    return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
    // Build new world matrix only if matrix is dirty (has unsaved changes)
    if (dirtyMatrix) {
        XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, position.z);
        XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(rotationPitchYawRoll.x, rotationPitchYawRoll.y, rotationPitchYawRoll.z);
        XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);
        XMMATRIX worldMat = scaleMat * rotationMat * translationMat;
        XMStoreFloat4x4(&worldMatrix, worldMat);
        XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(worldMat)));
        dirtyMatrix = false;
    }
    return worldInverseTransposeMatrix;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
    position.x += x;
    position.y += y;
    position.z += z;

    dirtyMatrix = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
    // movement vector in world axes
    XMVECTOR moveVec = XMVectorSet(x, y, z, 0);
    
    // rotating movement vector to match transform's axes
    XMVECTOR rotatedMoveVec = XMVector3Rotate(
        moveVec, 
        XMQuaternionRotationRollPitchYaw(rotationPitchYawRoll.x, rotationPitchYawRoll.y, rotationPitchYawRoll.z));

    // adding rotated movement vector to current position and storing it
    XMVECTOR newPos = XMLoadFloat3(&position) + rotatedMoveVec;
    XMStoreFloat3(&position, newPos);

    dirtyMatrix = true;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
    XMVECTOR rightMathType = XMVector3Rotate(
        XMVectorSet(1, 0, 0, 0),            // The world's right vector
        XMQuaternionRotationRollPitchYaw(
            rotationPitchYawRoll.x,
            rotationPitchYawRoll.y,
            rotationPitchYawRoll.z)         // rotating worlds right vector to find transforms right vector
    );

    XMStoreFloat3(&rightVec, rightMathType);

    return rightVec;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
    XMVECTOR upMathType = XMVector3Rotate(
        XMVectorSet(0, 1, 0, 0),            // The world's up vector
        XMQuaternionRotationRollPitchYaw(
            rotationPitchYawRoll.x,
            rotationPitchYawRoll.y,
            rotationPitchYawRoll.z)         // rotating worlds up vector to find transforms up vector
    );

    XMStoreFloat3(&upVec, upMathType);

    return upVec;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
    XMVECTOR forwardMathType = XMVector3Rotate(
        XMVectorSet(0, 0, 1, 0),            // The world's forward vector
        XMQuaternionRotationRollPitchYaw(
            rotationPitchYawRoll.x, 
            rotationPitchYawRoll.y, 
            rotationPitchYawRoll.z)         // rotating worlds forward vector to find transforms forward vector
    );

    XMStoreFloat3(&forwardVec, forwardMathType);

    return forwardVec;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
    rotationPitchYawRoll.x += pitch;
    rotationPitchYawRoll.y += yaw;
    rotationPitchYawRoll.z += roll;

    dirtyMatrix = true;
}

void Transform::Scale(float x, float y, float z)
{
    scale.x *= x;
    scale.y *= y;
    scale.z *= z;

    dirtyMatrix = true;
}
