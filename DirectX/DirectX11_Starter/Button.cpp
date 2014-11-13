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

	textPos = XMFLOAT2(pos->x + (mat->getTexWidth() - size.x) / 2, pos->y + (mat->getTexHeight() - size.y) / 2);
}

Button::~Button()
{
}

void Button::Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData) {
	//GameObject::Draw(deviceContext, cBuffer, cBufferData);
	batch->Draw(material->resourceView, XMFLOAT2(position.x, position.y));
	font->DrawString(batch, text, XMLoadFloat2(&textPos));
}
