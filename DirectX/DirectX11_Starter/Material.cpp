#include "Material.h"

Material::Material(ID3D11Device* device, ID3D11DeviceContext* context)
{
	this->deviceContext = context;

	device->CreateShaderResourceView(NULL , NULL, &resourceView);
	D3D11_SAMPLER_DESC* samplerDesc = new D3D11_SAMPLER_DESC();
	samplerDesc->Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc->AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc->AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc->AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	/*D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;*/

	HRESULT result1 = device->CreateSamplerState(samplerDesc, &samplerState);

	HRESULT result2 = CreateWICTextureFromFile(device, context, L"image.png", 0, &resourceView);

	delete samplerDesc;
}

Material::~Material()
{
}

void Material::Draw()
{
	deviceContext->PSSetShaderResources(0, 1, &resourceView); // Pass in the entity’s material’s shader resource view (the texture)
	
	deviceContext->PSSetSamplers(0, 1, &samplerState);	// Pass in the entity’s material’s sampler state
}