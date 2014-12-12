#include "Material.h"

Material::Material(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11SamplerState* pSampler, const wchar_t* texPath, ID3D11GeometryShader* gShader )
{
	// Set dx11 variables
	deviceContext = context;
	vertexShader = vShader;
	pixelShader = pShader;
	samplerState = pSampler;

	geometryShader = gShader;

	// Create SRV
	//HRESULT result1 = device->CreateShaderResourceView(NULL , NULL, &resourceView);	// Error from having a NULL resource is intentional

	// Create texture
	HRESULT result2 = CreateWICTextureFromFile(device, context, texPath, 0, &resourceView);

	// Get the dimensions of the texture
	ID3D11Texture2D *tex = 0;
	ID3D11Resource *res;
	resourceView->GetResource(&res);
	res->QueryInterface<ID3D11Texture2D>(&tex);
	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);
	texWidth = desc.Width;
	texHeight = desc.Height;
	ReleaseMacro(res);
	ReleaseMacro(tex);
}

Material::~Material()
{
	ReleaseMacro(samplerState);
	//ReleaseMacro(resourceView);	// This causes error
	resourceView->Release();
}

void Material::Draw()
{
	// Set the current vertex and pixel shaders and geometry shader (if it exists)
	//deviceContext->VSSetShader(vertexShader, NULL, 0);

	deviceContext->GSSetShader(geometryShader, NULL, 0);

	//deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetShaderResources(0, 1, &resourceView); // Pass in the entity’s material’s shader resource view (the texture)
	
	deviceContext->PSSetSamplers(0, 1, &samplerState);	// Pass in the entity’s material’s sampler state
}

void Material::SetShaders()
{
	deviceContext->VSSetShader(vertexShader, NULL, 0);

	//deviceContext->GSSetShader(geometryShader, NULL, 0);

	deviceContext->PSSetShader(pixelShader, NULL, 0);
}

UINT Material::getTexWidth() {
	return texWidth;
}
UINT Material::getTexHeight() {
	return texHeight;
}