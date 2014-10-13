#include "Material.h"

Material::Material(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, const wchar_t* texPath)
{
	// Set dx11 variables
	deviceContext = context;
	vertexShader = vShader;
	pixelShader = pShader;

	// Create SRV
	//HRESULT result1 = device->CreateShaderResourceView(NULL , NULL, &resourceView);	// Error from having a NULL resource is intentional

	// Create sampler description
	D3D11_SAMPLER_DESC* samplerDesc = new D3D11_SAMPLER_DESC();
	samplerDesc->Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc->AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc->AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc->AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	HRESULT result1 = device->CreateSamplerState(samplerDesc, &samplerState);

	// Create texture
	HRESULT result2 = CreateWICTextureFromFile(device, context, texPath, 0, &resourceView);

	delete samplerDesc;
}

Material::~Material()
{
	ReleaseMacro(samplerState);
	//ReleaseMacro(resourceView);	// This causes error
	resourceView->Release();
}

void Material::Draw()
{
	// Set the current vertex and pixel shaders
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetShaderResources(0, 1, &resourceView); // Pass in the entity’s material’s shader resource view (the texture)
	
	deviceContext->PSSetSamplers(0, 1, &samplerState);	// Pass in the entity’s material’s sampler state
}