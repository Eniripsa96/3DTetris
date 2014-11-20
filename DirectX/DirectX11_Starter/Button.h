#pragma once

#include "UIObject.h"
#include <SpriteFont.h>

class Button : public UIObject
{

public:
	Button(Mesh* mesh, Material* mat, XMFLOAT3* pos, SpriteBatch* batch, SpriteFont* font, wchar_t* text);
	~Button();

	void Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData);
};

