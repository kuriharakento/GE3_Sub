#include "SceneFactory.h"

#include "application/scene/play/GameClearScene.h"
#include "application/scene/play/GameOverScene.h"
#include "application/scene/play/GameScene.h"
#include "application/scene/play/TitleScene.h"
#include "base/Logger.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	//次のシーンを生成
	BaseScene* newScene = nullptr;

	if(sceneName == "TITLE")
	{
		newScene = new TitleScene();
	} else if (sceneName == "GAME")
	{
		newScene = new GameScene();
	}else if (sceneName == "CLEAR")
	{
		newScene = new GameClearScene();
	} else if (sceneName == "GAMEOVER")
	{
		newScene = new  GameOverScene();
	}
	else
	{
		//名前のシーンがない場合
		Logger::Log("Can't Create Scene\n");
	}

	return newScene;
}
