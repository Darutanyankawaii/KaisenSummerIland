#pragma once
#include "AssetRegistry.hpp"

// 旧 API 互換 wrapper (Main.cpp での古い呼び出しを保持するため)
inline void RegisterAudioAsset() {}
inline void RegisterFontAsset() {}
inline void RegisterTextureAsset() {}
inline void ResisterTextureAsset() {} // typo 互換
