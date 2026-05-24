#include "SoundSystem.hpp"

namespace Sound
{
	namespace
	{
		HashTable<SE, Audio> g_audios;

		constexpr uint32 kSampleRate = 44100;

		// 線形 decay エンベロープ (0..1 → 1..0)
		double linearDecay(size_t i, size_t total)
		{
			return 1.0 - static_cast<double>(i) / static_cast<double>(total);
		}

		// 二乗 decay (より急峻に減衰)
		double squaredDecay(size_t i, size_t total)
		{
			const double v = linearDecay(i, total);
			return v * v;
		}

		void writeStereo(Wave& wave, size_t i, double sample)
		{
			const float s = static_cast<float>(Clamp(sample, -1.0, 1.0));
			wave[i] = WaveSample{ s, s };
		}

		// ----- 波形生成プリミティブ -----

		// サイン波 + 二乗 decay
		Wave sineDecay(double freqHz, double durationSec, double volume = 0.3)
		{
			const size_t n = static_cast<size_t>(kSampleRate * durationSec);
			Wave wave(n, Arg::sampleRate = kSampleRate);
			for (size_t i = 0; i < n; ++i)
			{
				const double t = static_cast<double>(i) / kSampleRate;
				const double env = squaredDecay(i, n);
				writeStereo(wave, i, volume * env * std::sin(2.0 * Math::Pi * freqHz * t));
			}
			return wave;
		}

		// 周波数スイープ (freqStart → freqEnd) + 線形 decay
		Wave sineSweep(double freqStart, double freqEnd, double durationSec, double volume = 0.25)
		{
			const size_t n = static_cast<size_t>(kSampleRate * durationSec);
			Wave wave(n, Arg::sampleRate = kSampleRate);
			double phase = 0.0;
			for (size_t i = 0; i < n; ++i)
			{
				const double progress = static_cast<double>(i) / n;
				const double freq = freqStart + (freqEnd - freqStart) * progress;
				const double dt = 1.0 / kSampleRate;
				phase += 2.0 * Math::Pi * freq * dt;
				const double env = linearDecay(i, n);
				writeStereo(wave, i, volume * env * std::sin(phase));
			}
			return wave;
		}

		// ホワイトノイズ + 二乗 decay (打撃・爆発系)
		Wave noiseDecay(double durationSec, double volume = 0.25)
		{
			const size_t n = static_cast<size_t>(kSampleRate * durationSec);
			Wave wave(n, Arg::sampleRate = kSampleRate);
			for (size_t i = 0; i < n; ++i)
			{
				const double env = squaredDecay(i, n);
				writeStereo(wave, i, volume * env * Random(-1.0, 1.0));
			}
			return wave;
		}

		// 低域 sine + ノイズミックス (鈍い被弾音)
		Wave thudNoise(double freqHz, double durationSec, double volume = 0.35)
		{
			const size_t n = static_cast<size_t>(kSampleRate * durationSec);
			Wave wave(n, Arg::sampleRate = kSampleRate);
			for (size_t i = 0; i < n; ++i)
			{
				const double t = static_cast<double>(i) / kSampleRate;
				const double env = squaredDecay(i, n);
				const double tone = std::sin(2.0 * Math::Pi * freqHz * t);
				const double noise = Random(-0.5, 0.5);
				writeStereo(wave, i, volume * env * (tone * 0.6 + noise * 0.4));
			}
			return wave;
		}

		// 三連アルペジオ (取得音などキラッと感)
		Wave arpeggio(std::initializer_list<double> freqs, double totalSec, double volume = 0.25)
		{
			const size_t n = static_cast<size_t>(kSampleRate * totalSec);
			Wave wave(n, Arg::sampleRate = kSampleRate);
			const size_t segCount = freqs.size();
			const size_t segLen = n / Max<size_t>(segCount, 1);
			size_t idx = 0;
			for (double f : freqs)
			{
				for (size_t j = 0; j < segLen && (idx + j) < n; ++j)
				{
					const double t = static_cast<double>(j) / kSampleRate;
					const double env = squaredDecay(j, segLen);
					writeStereo(wave, idx + j, volume * env * std::sin(2.0 * Math::Pi * f * t));
				}
				idx += segLen;
			}
			return wave;
		}
	}

	void init()
	{
		// === ファイル読込 (既存アセット) ===
		g_audios[SE::BGM] = Audio(U"sound/bgm.mp3");
		g_audios[SE::Walk] = Audio(U"sound/walk.ogg");
		g_audios[SE::PlayerShot] = Audio(U"sound/shot.ogg");

		// === 波形生成 (素材がないものを補う) ===
		// 被弾: 低音 sine + ノイズで鈍い打撃感
		g_audios[SE::PlayerHit] = Audio(thudNoise(110.0, 0.25));

		// 敵弾発射: 短い低音 beep
		g_audios[SE::EnemyShot] = Audio(sineDecay(180.0, 0.08, 0.25));

		// 敵撃破: 下降スイープ
		g_audios[SE::EnemyDefeat] = Audio(sineSweep(700.0, 200.0, 0.25));

		// ボス撃破: 長めのスイープ + 強め
		g_audios[SE::BossDefeat] = Audio(sineSweep(900.0, 100.0, 0.6, 0.4));

		// アイテム取得: 上昇アルペジオ (C-E-G 風)
		g_audios[SE::ItemPickup] = Audio(arpeggio({ 523.25, 659.25, 783.99 }, 0.18));

		// ジャンプ: 短い上昇スイープ
		g_audios[SE::Jump] = Audio(sineSweep(350.0, 600.0, 0.1, 0.2));

		// BGM はデフォルトでループ
		setLoop(SE::BGM, true);
	}

	void play(SE se)
	{
		if (auto it = g_audios.find(se); it != g_audios.end())
		{
			it->second.playOneShot();
		}
	}

	void start(SE se)
	{
		if (auto it = g_audios.find(se); it != g_audios.end())
		{
			it->second.play();
		}
	}

	void stop(SE se)
	{
		if (auto it = g_audios.find(se); it != g_audios.end())
		{
			it->second.stop();
		}
	}

	void setLoop(SE se, bool loop)
	{
		if (auto it = g_audios.find(se); it != g_audios.end())
		{
			it->second.setLoop(loop);
		}
	}

	void stopAllShots(SE se)
	{
		if (auto it = g_audios.find(se); it != g_audios.end())
		{
			it->second.stopAllShots();
		}
	}
}
