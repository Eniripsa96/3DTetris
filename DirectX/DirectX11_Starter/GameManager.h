#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <DirectXMath.h>
#include "DirectXGame.h"
#include "GameObject.h"
#include "Button.h"
#include "Camera.h"
#include "BlockManager.h"
#include <vector>
#include "ObjLoader.h"
#include <SpriteFont.h>
#include <SpriteBatch.h>

// Include run-time memory checking in debug builds
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// For DirectX Math
using namespace DirectX;
using namespace std;

enum GAME_STATE
{
	MENU,
	GAME,
	DEBUG
};

struct BLEND_DESC : public D3D11_BLEND_DESC {};

// Demo class which extends the base DirectXGame class
class GameManager : public DirectXGame
{
public:
	GameManager(HINSTANCE hInstance);
	~GameManager();

	// Overrides for base level methods
	bool Init();
	void LoadShadersAndInputLayout();
	void BuildBlockTypes();
	void LoadMeshesAndMaterials();
	void OnResize();
	void UpdateScene(float dt);
	void CheckKeyBoard(float dt);
	void DrawScene();	// Deprecated

	// For handing mouse input
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);

	// Shaders
	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* uiPixelShader;
	ID3D11VertexShader* uiVertexShader;

	// A few more odds and ends we'll need
	ID3D11InputLayout* inputLayout;

	// Blend state
	ID3D11BlendState* blendState;

	// Constant buffer info
	ID3D11Buffer* vsConstantBuffer;
	VertexShaderConstantBufferLayout dataToSendToVSConstantBuffer;

	GAME_STATE gameState;

private:
	std::vector<GameObject*> gameObjects;
	std::vector<UIObject*> gameUIObjects;
	std::vector<UIObject*> menuObjects;
	BlockManager* blockManager;

	SpriteBatch* spriteBatch;
	SpriteFont* spriteFont24;
	SpriteFont* spriteFont32;
	SpriteFont* spriteFont72;

	Mesh* triangleMesh;
	Mesh* quadMesh;
	Mesh* cubeMesh;
	Mesh* jBlockMesh;
	Mesh* lBlockMesh;
	Mesh* leftBlockMesh;
	Mesh* longBlockMesh;
	Mesh* rightBlockMesh;
	Mesh* squareBlockMesh;
	Mesh* stairsBlockMesh;
	Mesh* frameMesh;

	Material* shapeMaterial;
	Material* buttonMaterial;
	Material* titleMaterial;
	Material* labelMaterial;
	Material* jBlockMaterial;
	Material* lBlockMaterial;
	Material* leftBlockMaterial;
	Material* longBlockMaterial;
	Material* rightBlockMaterial;
	Material* squareBlockMaterial;
	Material* stairsBlockMaterial;
	Material* frameMaterial;

	Block* blocks;
	vector<GameObject*> cubes;
	bool canRotate = false;

	Button* playButton;
	Button* quitButton;
	UIObject* scoreLabel;

	Camera* camera;
	XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	const float CAMERA_MOVE_FACTOR = 10.0f;
	const float CAMERA_TURN_FACTOR = 1.0f;
};

#endif