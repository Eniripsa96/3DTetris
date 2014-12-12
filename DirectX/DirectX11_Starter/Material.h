#ifndef MATERIAL_H
#define MATERIAL_H

#include "DirectXGame.h"
//#include "MyDemoGame.h"
//#include <DirectXMath.h>
#include <DirectXMath.h>
#include "WICTextureLoader.h"

using namespace DirectX;

class Material
{
public:
	Material(ID3D11Device*, ID3D11DeviceContext*, ID3D11VertexShader*, ID3D11PixelShader*, ID3D11SamplerState*, const wchar_t*, ID3D11GeometryShader* gs = NULL);
	~Material();

	void Draw();
	void SetShaders();

	UINT getTexWidth();
	UINT getTexHeight();

	ID3D11ShaderResourceView* resourceView;
	ID3D11SamplerState* samplerState;

	ID3D11DeviceContext* deviceContext;

	// Shaders
	ID3D11PixelShader* pixelShader;
	ID3D11GeometryShader* geometryShader;
	ID3D11VertexShader* vertexShader;

private:
	UINT texWidth;
	UINT texHeight;
};

#endif