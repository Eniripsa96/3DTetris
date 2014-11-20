#include "UIObject.h"


UIObject::UIObject(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* pBatch, SpriteFont* pFont, wchar_t* pText)
: GameObject(mesh, mat, pos, new XMFLOAT3(0, 0, 0))
{
	batch = pBatch;
	font = pFont;
	SetText(pText);
}

UIObject::~UIObject()
{
}

void UIObject::Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData) {
	batch->Draw(material->resourceView, XMFLOAT2(position.x, position.y));
	font->DrawString(batch, text, XMLoadFloat2(&textPos));
}

void UIObject::Move(float x, float y) {
	position.x += x;
	position.y += y;

	textPos.x += x;
	textPos.y += y;
}

void UIObject::Update(int x, int y) {
	hovered = IsOver(x, y);
}

bool UIObject::IsOver(int x, int y) {
	return x >= position.x && x <= position.x + material->getTexWidth() && y >= position.y && y <= position.y + material->getTexHeight();
}

void UIObject::SetText(const wchar_t* pText) {
	text = pText;

	XMVECTOR xmSize = font->MeasureString(text);
	XMFLOAT2 size;
	XMStoreFloat2(&size, xmSize);
	textWidth = size.x;
	textHeight = size.y;

	textPos = XMFLOAT2(position.x + (material->getTexWidth() - size.x) / 2, position.y + (material->getTexHeight() - size.y) / 2);
}
