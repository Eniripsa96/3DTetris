#include "Material.h"

Material::Material(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, const wchar_t* texPath)
{
	deviceContext = context;
	vertexShader = vShader;
	pixelShader = pShader;

	device->CreateShaderResourceView(NULL , NULL, &resourceView);
	D3D11_SAMPLER_DESC* samplerDesc = new D3D11_SAMPLER_DESC();
	samplerDesc->Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc->AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc->AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc->AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	HRESULT result1 = device->CreateSamplerState(samplerDesc, &samplerState);

	HRESULT result2 = CreateWICTextureFromFile(device, context, texPath, 0, &resourceView);

	delete samplerDesc;
}

Material::~Material()
{
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
}

void Material::Draw()
{
	deviceContext->PSSetShaderResources(0, 1, &resourceView); // Pass in the entity’s material’s shader resource view (the texture)
	
	deviceContext->PSSetSamplers(0, 1, &samplerState);	// Pass in the entity’s material’s sampler state
}