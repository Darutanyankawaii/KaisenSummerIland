#pragma once
#include "Basic.hpp"

// ゲームで使う全 SE と BGM を一元管理する名前空間。
// 追加方法:
//   1. SE enum に新項目を追加
//   2. SoundSystem.cpp の init() でファイル読込か波形生成を登録
//   3. 呼び出し側で Sound::play(SE::XXX) する
namespace Sound
{
	enum class SE
	{
		// ----- BGM / ループ系 -----
		BGM,           // タイトル/ゲーム共通の BGM
		Walk,          // プレイヤーの足音 (ループ)

		// ----- ワンショット系 -----
		PlayerShot,    // プレイヤーの銃発射
		PlayerHit,     // プレイヤー被弾
		EnemyShot,     // 敵の弾発射
		EnemyDefeat,   // 敵撃破
		BossDefeat,    // ボス撃破 (特別)
		ItemPickup,    // 武器アイテム取得
		Jump,          // ジャンプ
		Land,          // 着地
		Empty,         // 空打ち (クールタイム中のクリック)
		HpRecover,     // HP 回復 (かき氷)
		BossSpawn,     // ボス出現

		// ----- シーン遷移・ゲームフロー -----
		StageStart,    // ステージ開始
		StageClear,    // ステージクリア (ゴール到達)
		GameOverJingle,// ゲームオーバー演出

		// ----- メニュー操作 -----
		MenuSelect,    // 選択カーソル移動
		MenuConfirm,   // 決定
		MenuCancel,    // 戻る / キャンセル
	};

	// 起動時に 1 回だけ呼ぶ。ファイル読み込みと波形生成を全て行う。
	void init();

	// ワンショット再生 (同時発音可)
	void play(SE se);

	// ループ系を再生 (BGM や足音)
	void start(SE se);

	// 停止 (BGM/足音用)
	void stop(SE se);

	// ループ設定 (主に BGM 用に init 後に呼ぶ)
	void setLoop(SE se, bool loop);

	// 音量設定 (0.0 ~ 1.0)。playOneShot で再生中の音には反映されないので、
	// BGM や常時音にのみ有効。
	void setVolume(SE se, double volume);

	// playOneShot で再生中のインスタンスを全停止 (シーン切替時に有用)
	void stopAllShots(SE se);
}
