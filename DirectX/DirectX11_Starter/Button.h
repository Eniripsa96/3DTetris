#pragma once

#include "GameObject.h"
#include <SpriteFont.h>

class Button : public GameObject
{

public:
	Button(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* batch, SpriteFont* font, wchar_t* text);
	~Button();

	void Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData);
	void Move(float x, float y);
	bool IsOver(float x, float y);

private:
	SpriteBatch* batch;
	SpriteFont* font;
	XMFLOAT2 textPos;
	wchar_t* text;
	float textWidth;
	float textHeight;
};

