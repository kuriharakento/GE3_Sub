#include "SceneFactory.h"

#include "application/scene/play/GamePlayScene.h"
#include "application/scene/play/TitleScene.h"
#include "base/Logger.h"

IScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	//次のシーンを生成
	IScene* newScene = nullptr;

	if(sceneName == "TITLE")
	{
		newScene = new TitleScene();
	} else if (sceneName == "GAMEPLAY")
	{
		newScene = new GamePlayScene();
	}else
	{
		Logger::Log("Can't Create Scene\n");
	}

	return newScene;
}
