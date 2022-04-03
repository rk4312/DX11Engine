#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(float x, float y, float z, float aspectRatio)
{
	// Set up the initial transform and matrices
	transform.SetPosition(x, y, z);
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

void Camera::Update(float deltaTime)
{
	Input& input = Input::GetInstance();

	// key press move speed
	float moveSpeed = 0.5f;

	// Pressing forward key
	if (input.KeyDown('W')) 
	{
		transform.MoveRelative(0.0f, 0.0f, moveSpeed * deltaTime);
	}

	// Pressing back key
	if (input.KeyDown('S')) 
	{
		transform.MoveRelative(0.0f, 0.0f, -moveSpeed * deltaTime);
	}

	// Pressing left key
	if (input.KeyDown('A')) 
	{
		transform.MoveRelative(-moveSpeed * deltaTime, 0.0f, 0.0f);
	}

	// Pressing right key
	if (input.KeyDown('D')) 
	{
		transform.MoveRelative(moveSpeed * deltaTime, 0.0f, 0.0f);
	}

	// Pressing up key
	if (input.KeyDown(VK_SPACE)) 
	{
		transform.MoveAbsolute(0.0f, moveSpeed * deltaTime, 0.0f);
	}

	// Pressing down key
	if (input.KeyDown('X')) {
		transform.MoveAbsolute(0.0f, -moveSpeed * deltaTime, 0.0f);
	}

	// Mouse input
	if (input.MouseLeftDown())
	{
		int cursorMovementX = input.GetMouseXDelta();
		int cursorMovementY = input.GetMouseYDelta();

		// mouse move speed
		float mouseSpeed = 0.2f;

		float xRotation = cursorMovementX * mouseSpeed * deltaTime;
		float yRotation = cursorMovementY * mouseSpeed * deltaTime;

		// using xRotation to rotate transform around y axis (yaw)
		transform.Rotate(0.0f, xRotation, 0.0f);

		// using yRotation to rotate transform around x axis (pitch)
		transform.Rotate(yRotation, 0.0f, 0.0f);

		// getting current pitch yaw roll
		XMFLOAT3 currentPYR = transform.GetPitchYawRoll();

		// clamping x rotation between negative half pi and positive half pi to prevent camera from flipping upside down
		if (currentPYR.x < -XM_PIDIV2)
		{
			transform.SetRotation(-XM_PIDIV2, currentPYR.y, currentPYR.z);
		}
		else if (currentPYR.x > XM_PIDIV2)
		{
			transform.SetRotation(XM_PIDIV2, currentPYR.y, currentPYR.z);
		}
	}

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	// need these for look at method parameters
	XMFLOAT3 pos = transform.GetPosition();
	XMFLOAT3 up = XMFLOAT3(0, 1, 0);

	XMFLOAT3 forward = transform.GetForward();

	// Build the view matrix from our position, our local forward vector and the world's up axis
	XMMATRIX view = XMMatrixLookToLH(
		XMLoadFloat3(&pos),
		XMLoadFloat3(&forward),
		XMLoadFloat3(&up)
	);
	XMStoreFloat4x4(&viewMatrix, view);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(
		XM_PIDIV4,      // fov (45 degrees currently)
		aspectRatio,    // aspect ratio of window
		0.01f,          // near plane (close to 0 but not 0)
		100.0f          // far plane (ideally not more than 1000)
	);
	XMStoreFloat4x4(&projectionMatrix, proj);
}

Transform* Camera::GetTransform()
{
	return &transform;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}
