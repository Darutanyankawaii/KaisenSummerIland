#include "StageSelect.hpp"

//ステージIDから次のステージのデータを取得する
StageData StageSelect::getNextStageData(int stageID)
{
	LoadStageData();
	if (StageList.contains(stageID))
		if (StageList.contains(StageList[stageID].nextStageID))
			return StageList[StageList[stageID].nextStageID];
	return StageList[0];
}

//ステージのデータを取得する(ステージが登録されていない場合の対策付き)
StageData StageSelect::getStageData(int stageID)
{
	LoadStageData();
	if (StageList.contains(stageID))return StageList[stageID];
	return StageList[0];
}

//ステージデータ登録用の関数
void StageSelect::LoadStageData()
{
	//データ未登録の場合
	if (!StageList.contains(0))
	{
		StageList.emplace(0, StageData{});
		const CSV csv{ U"Maps/MapList.csv" };
		if (not csv)
		{
			throw Error{ U"Failed to load `Maps/MapList.csv`" };
		}
		for (size_t i = 0; i < csv.rows(); ++i)
		{
			if (csv[i].size() == 4)
			{
				StageList.emplace(
					Parse<int>(csv[i][0]),
					StageData{ U"Maps/" + csv[i][1],csv[i][2],Parse<int>(csv[i][3]) }
				);
			}
		}
	}
}

StageSelect::StageSelect(const InitData& init) : IScene{ init }
{
	selected_stage = 0;
	// 背景の色を設定 | Set background color
	Scene::SetBackground(ColorF{ 0.0, 1.0, 1.0 });
	//ステージデータのロード
	LoadStageData();
	//ステージの並びを指定
	for (auto [key, value] : StageList)
		if (key != 0)
			stageIDList.push_back(key);
	stageIDList.sort();
}

void StageSelect::update() {

	wheel_count += Mouse::Wheel() * 5;

	if (wheel_count > wheel_space) {
		wheel_count = 0;
		wheel_flag = -1;
	}
	else if (wheel_count < -wheel_space) {
		wheel_count = 0;
		wheel_flag = 1;
	}

	if ((KeyEnter | KeyZ | KeySpace | MouseL).down())
	{
		//ゲームシーンに遷移する
		getData().currentStageID = stageIDList[selected_stage];
		changeScene(SceneName::Game);
	}
	if ((KeyW | KeyUp).down() || wheel_flag == 1)
	{
		wheel_flag = 0;
		selected_stage = (selected_stage + stageIDList.size() - 1) % stageIDList.size();
	}
	if ((KeyS | KeyDown).down() || wheel_flag == -1)
	{
		wheel_flag = 0;
		selected_stage = (selected_stage + 1) % stageIDList.size();
	}

	time_anim += Scene::DeltaTime();
	if (time_anim > (time_span + time_space) * 2)time_anim -= (time_span + time_space) * 2;
}


void StageSelect::draw() const {
	// 半透明の円を描く | Draw a semi-transparent circle
#ifdef DEBUGGING
	Circle{ Cursor::Pos(), 40 }.draw(ColorF{ 1.0, 0.0, 0.0, 0.5 });
#endif

	Color color_1 = Color(255, 151, 75);
	Color color_2 = Color(0, 175, 110);

	const int margin = 50;
	Point drawable_size = Scene::Center();
	Point num = Point(drawable_size.x / margin + 1, drawable_size.y / margin + 1);
	if (time_anim < time_span)
	{
		if (time_anim < time_span - time_space)Scene::SetBackground(color_1);
		else Scene::SetBackground(color_2);
		for (int i = -num.x; i < num.x + 1; ++i)
		{
			for (int j = -num.y; j < num.y + 1; ++j)
			{
				Circle(drawable_size + Point(i, j) * margin, margin * time_anim / time_span).draw(color_2);
			}
		}
	}
	else if (time_anim < time_span + time_space)
	{
		Scene::SetBackground(color_2);
	}
	else if (time_anim < time_span * 2 + time_space)
	{
		if (time_anim < time_span * 2)Scene::SetBackground(color_2);
		else Scene::SetBackground(color_1);
		for (int i = -num.x; i < num.x; ++i)
		{
			for (int j = -num.y; j < num.y; ++j)
			{
				Circle(drawable_size + (Vec2(i, j) + Vec2(0.5, 0.5)) * margin, margin * (time_anim - time_span) / time_span).draw(color_1);
			}
		}
	}
	else
	{
		Scene::SetBackground(color_1);
	}

	for (size_t i = 0; i < stageIDList.size(); ++i)
	{
		FontAsset(U"StageTitle")(StageList[stageIDList[i]].name)
			.drawAt(Scene::Center() + Point(0, 50 * (i - selected_stage)),
				ColorF(1.0, 1.0, 1.0, (i == selected_stage) ? 1.0 : 0.5));
	}
}
