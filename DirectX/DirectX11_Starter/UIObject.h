#pragma once

#include "GameObject.h"
#include <SpriteFont.h>

class UIObject : public GameObject
{

public:
	UIObject(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* batch, SpriteFont* font, wchar_t* text);
	~UIObject();

	void Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData);
	void Update(int x, int y);
	void Move(float x, float y);
	bool IsOver(int x, int y);
	void SetText(wchar_t* text);

protected:
	SpriteBatch* batch;
	SpriteFont* font;
	XMFLOAT2 textPos;
	wchar_t* text;
	float textWidth;
	float textHeight;
	bool hovered = false;
};

