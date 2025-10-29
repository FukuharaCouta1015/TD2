#include "MapChipField.h"
#include "kamataEngine.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
    {"2", MapChipType::kMino },
};

// ブロックの範囲取得関数
MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {

	KamataEngine::Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;

	return rect;
}

void MapChipField::ResetMapChipData() {

	mapChipData_.data_.clear();
	mapChipData_.data_.resize(kNumBlocksVertical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data_) {
		mapChipDataLine.resize(kNumBlocksHorizontal);
	}
}

void MapChipField::LodeMapChipCsv(const std::string& filePaht) {
	// マップチップのデータリセット
	ResetMapChipData();

	// ファイルを開く
	std::ifstream file;
	file.open(filePaht);
	// assert(file.is_open());
	//  マップチップcsv
	std::stringstream mapChipCsv;
	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// csvからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlocksVertical; ++i) {
		std::string line;
		std::getline(mapChipCsv, line);
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlocksHorizontal; ++j) {
			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data_[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlocksHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlocksVertical - 1 < yIndex) {
		return MapChipType::kBlank;
	}
	return mapChipData_.data_[yIndex][xIndex];
}

KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return KamataEngine::Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlocksVertical - 1 - yIndex), 0);
}

void MapChipField::ClearFullines() { 
	printf("ClearFullines: --- Before --- frame: (unknown frame)\n");
	for (int i = kNumBlocksVertical - 1; i >= 0; --i) { // 上から下に表示
		printf("Row %2d: ", i);
		for (uint32_t j = 0; j < kNumBlocksHorizontal; ++j) {
			printf("%d ", (int)mapChipData_.data_[i][j]);
		}
		printf("\n");
	}
	printf("----------------------------\n");

    uint32_t y = 0;
    while (y < kNumBlocksVertical - 1) { // y=20 の行は判定しない
        bool isLineFull = true;
        for (uint32_t x = 1; x < kNumBlocksHorizontal - 1; ++x) { // 左右の壁は除く
            if (mapChipData_.data_[y][x] == MapChipType::kBlank) {
                isLineFull = false;
                break;
            }
        }

        if (isLineFull) {
            printf("ClearFullines: Line FULL detected at y=%u\n", y);

            // ★変更箇所★: 揃った行 (y) から最上行までを一つずつ下にシフトする
            // moveY は消去された行 (y) から最上行の一つ手前 (kNumBlocksVertical-1) まで
            // 下から上にコピーするループに変更
            for (uint32_t moveY = y; moveY > 0; --moveY) { 
                for (uint32_t x = 1; x < kNumBlocksHorizontal - 1; ++x) {
                    // moveY 行に moveY-1 行の内容をコピー
                    // kBlock は動かないので、moveY 行が kBlock でない場合のみコピー
                    if (mapChipData_.data_[moveY][x] != MapChipType::kBlock) {
                        if (mapChipData_.data_[moveY-1][x] != MapChipType::kBlock) {
                            mapChipData_.data_[moveY][x] = mapChipData_.data_[moveY-1][x];
                        } else {
                            mapChipData_.data_[moveY][x] = MapChipType::kBlank; // 上の行が壁なら空白にする
                        }
                    }
                }
            }
            
            // 最上行 (y=0) は必ず空白にする (壁は除く)
            for (uint32_t x = 1; x < kNumBlocksHorizontal - 1; ++x) {
                if (mapChipData_.data_[0][x] != MapChipType::kBlock) {
                    mapChipData_.data_[0][x] = MapChipType::kBlank;
                }
            }

            // 行が揃った場合は、y の値を変更せず、同じ行をもう一度チェックする
            // (上の行が落ちてきて、再びこの行が揃う可能性があるため)
            // printf("ClearFullines: Re-checking current y=%u after clearing.\n", y);

        } else {
            // 行が揃っていない場合は y をインクリメントして次の行へ
            ++y;
        }
    }

	printf("ClearFullines: --- After --- frame: (unknown frame)\n");
	for (int i = kNumBlocksVertical - 1; i >= 0; --i) { // 上から下に表示
		printf("Row %2d: ", i);
		for (uint32_t j = 0; j < kNumBlocksHorizontal; ++j) {
			printf("%d ", (int)mapChipData_.data_[i][j]);
		}
		printf("\n");
	}
	printf("---------------------------\n");
}

MapChipField::IndexSet MapChipField::GetMapChipIndexByPosition(const KamataEngine::Vector3& position) {

	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.yIndex = kNumBlocksVertical - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2) / kBlockHeight);

	return indexSet;
}