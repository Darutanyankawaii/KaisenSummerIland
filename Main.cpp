# include <Siv3D.hpp> // Siv3D v0.6.16
# include "Basic.hpp"
# include "Regist.hpp"
# include "Game.hpp"
# include "StageSelect.hpp"

# include "Title.hpp"
# include "GameOver.hpp"
# include "GameClear.hpp"
# include "How2Play.hpp"

void Main()
{
	//ウィンドウサイズの変更
	Window::Resize(SCENE_WIDTH, SCENE_HEIGHT);

	//シーンの追加方法
	//1,テンプレ文をもとにテンプレに追加したいシーンの名前を書きmanagerに加える
	//2,Basic.hにある「enum class SceneName」の中に1で作成したシーンの名前を書き込む
	//3,作りたいシーンの.hファイルと.cppファイルを作成する。
	//4,作りたいシーンの.hファイルを#includeする。

	App manager;
	manager
		.add<Title>(SceneName::Title)
		.add<StageSelect>(SceneName::StageSelect)
		.add<Game>(SceneName::Game)
		.add<GameOver>(SceneName::GameOver)
		.add<How2Play>(SceneName::How2Play)
		.add<GameClear>(SceneName::GameClear)
		;

	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });// 背景の色を設定 | Set background color

	RegisterAudioAsset();
	RegisterFontAsset();
	ResisterTextureAsset();

	const int FPS = 60;	// 1秒間に1画面を書き換える回数
	Stopwatch sw;
	sw.start();

	while (System::Update())
	{
		AudioAsset(U"bgm").setLoop(true);
		AudioAsset(U"bgm").play();

		if (!manager.update())
		{
			break;
		}

		while (sw.msF() < 1000.0 / FPS);    //1/60秒経過するまでループ
		sw.restart();
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
