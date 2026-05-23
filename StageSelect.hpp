#pragma once
# include "Basic.hpp"

class StageSelect :public App::Scene {
private:
	//ステージIDのリスト。並びを指定するのに扱う
	Array<int> stageIDList;

	//UI用の変数
	size_t selected_stage;

	//アニメーション用の変数
	const double time_span = 3.0;
	const double time_space = 0.1;
	double time_anim = 0;

	//ホイール用の変数
	const double wheel_space = 10;
	double wheel_count = 0.0;
	int wheel_flag = 0;

public:
	//外部からもアクセス可能にする
	static inline HashTable<int, StageData> StageList;
	//ステージIDから次のステージのデータを取得する
	static StageData getNextStageData(int);
	//ステージのデータを取得する(ステージが登録されていない場合の対策付き)
	static StageData getStageData(int);
	//ステージデータ登録用の関数
	static void LoadStageData();

	StageSelect(const InitData& init);
	void update() override;
	void draw() const override;
};
