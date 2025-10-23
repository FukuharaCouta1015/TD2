#pragma once
#include <cstdint>
#include "kamataEngine.h"
#include <vector>

enum class MapChipType {
	kBlank,
	kBlock,
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data_;
};

class MapChipField {
public:

	//範囲矩形
	struct Rect {
		float left;
		float right; 
		float bottom ;
		float top ;
	};

	//ブロックの範囲取得関数
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);


	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	static inline const uint32_t kNumBlocksHorizontal = 12;
	static inline const uint32_t kNumBlocksVertical = 21;

	uint32_t GetNumBlockHorizontal()const { return kNumBlocksHorizontal; }
	uint32_t GetNumBlockVertical() const { return kNumBlocksVertical; }

	void ResetMapChipData();

	void LodeMapChipCsv(const std::string& filePaht);
	
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	MapChipData mapChipData_;

	// インデックスセット
	struct IndexSet{
		uint32_t xIndex;
		uint32_t yIndex;
	};

	IndexSet GetMapChipIndexByPosition(const KamataEngine::Vector3& position); 

private:

};