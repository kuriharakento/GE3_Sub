#pragma once
class MyGame
{
public:
	//コンストラクタ
	MyGame();
	//デストラクタ
	~MyGame();

	//初期化
	void Initialize();

	//終了
	void Finalize();

	//毎フレーム
	void Update();

	//描画
	void Draw();
};

