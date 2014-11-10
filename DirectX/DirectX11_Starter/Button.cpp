#include "Button.h"


Button::Button(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* pBatch, SpriteFont* pFont, wchar_t* pText)
: GameObject(mesh, mat, pos, new XMFLOAT3(0, 0, 0))
{
	batch = pBatch;
	font = pFont;
	text = pText;

	XMVECTOR xmSize = font->MeasureString(text);
	XMFLOAT2 size;
	XMStoreFloat2(&size, xmSize);
	textWidth = size.x;
	textHeight = size.y;
}

Button::~Button()
{
}

void Button::Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData) {
	GameObject::Draw(deviceContext, cBuffer, cBufferData);
	
	font->DrawString(batch, text, XMLoadFloat3(&position));
}
