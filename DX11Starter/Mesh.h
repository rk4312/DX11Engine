#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

class Mesh
{
private:
	
	// ComPtr to hold buffers and the context used to call draw
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> contextPtr;

	// Holds the number of indices in the index buffer
	int numIndices;

public:
	
	Mesh(Vertex* vertices, int numVertices, unsigned int* indices, int numIndices, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	Mesh(const char* objFile, Microsoft::WRL::ComPtr<ID3D11Device> devicePtr, Microsoft::WRL::ComPtr<ID3D11DeviceContext> p_contextPtr);
	~Mesh();
	void CreateBufferHelper(Vertex* vertices, int numVertices, unsigned int* indices, int p_numIndices, Microsoft::WRL::ComPtr<ID3D11Device> devicePtr, Microsoft::WRL::ComPtr<ID3D11DeviceContext> p_contextPtr);
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	void Draw();
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};

