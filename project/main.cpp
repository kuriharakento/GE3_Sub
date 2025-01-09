#include <Windows.h>
#include <vector>

//ImGui
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

///////////////////////////////////////////////////////////////////////
///					>>>自作クラスのインクルード<<<						///
///////////////////////////////////////////////////////////////////////

#pragma region 自作クラスのインクルード
#include "input/Input.h"
#include "base/WinApp.h"
#include "base/DirectXCommon.h"
#include "base/D3DResourceLeakChecker.h"
#include "2d/SpriteCommon.h"
#include "2d/Sprite.h"
#include "manager/TextureManager.h"
#include "3d/Object3dCommon.h"
#include "3d/Object3d.h"
#include "3d/ModelManager.h"
#include "base/Logger.h"
#include "manager/CameraManager.h"
#include "manager/ImGuiManager.h"
#include "manager/SrvManager.h"
#include "effects/ParticleEmitter.h"
#include "framework/MyGame.h"
#include "math/VectorFunc.h"
#pragma endregion

//コードを整理するときに使う
/*--------------[  ]-----------------*/

///////////////////////////////////////////////////////////////////////
///							>>>構造体の宣言<<<						///
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
///						>>>関数の宣言<<<								///
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
///						>>>グローバル変数の宣言<<<						///
///////////////////////////////////////////////////////////////////////

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//フレームワーク
	Framework* game = new MyGame();

	//実行
	game->Run();

	//解放
	delete game;

	//終了
	return 0;
}