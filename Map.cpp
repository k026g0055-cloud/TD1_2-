#include "Map.h"
#include "Camera.h"
#include <Novice.h>

void Map::Initialize()
{
	/*floorTexture_ = Novice::LoadTexture("./Resources/Images/floor.png");
	wallTexture_ = Novice::LoadTexture("./Resources/Images/wall.png");*/

	for (int y = 0; y < kMapHeight; y++)
	{
		for (int x = 0; x < kMapWidth; x++)
		{
			if (x == 0 || x == kMapWidth - 1 ||
				y == 0 || y == kMapHeight - 1)
			{
				tiles_[y][x] = TILE_WALL;
			}
			else
			{
				tiles_[y][x] = TILE_FLOOR;
			}
		}
	}
}

void Map::Draw(const Camera& camera) const
{
	for (int y = 0; y < kMapHeight; y++)
	{
		for (int x = 0; x < kMapWidth; x++)
		{
			Vector2 worldPos = {
				static_cast<float>(x * kTileSize),
				static_cast<float>(y * kTileSize)
			};

			Vector2 screenPos =
				camera.WorldToScreen(worldPos);

			// 画面外のマップチップは描画しない
			if (screenPos.x < -kTileSize ||
				screenPos.x >= 1280 ||
				screenPos.y < -kTileSize ||
				screenPos.y >= 720)
			{
				continue;
			}

			int texture = floorTexture_;

			if (tiles_[y][x] == TILE_WALL)
			{
				texture = wallTexture_;
			}

			Novice::DrawSprite(
				static_cast<int>(screenPos.x),
				static_cast<int>(screenPos.y),
				texture,
				1.0f,
				1.0f,
				0.0f,
				WHITE
			);
		}
	}
}
void Map::DrawBoss(const Camera& camera) const
{
	for (int y = 0; y < kBossMapHeight; y++)
	{
		for (int x = 0; x < kBossMapWidth; x++)
		{
			Vector2 worldPos = {
				static_cast<float>(x * kTileSize),
				static_cast<float>(y * kTileSize)
			};

			Vector2 screenPos =
				camera.WorldToScreen(worldPos);

			// 画面外のマップチップは描画しない
			if (screenPos.x < -kTileSize ||
				screenPos.x >= 1280 ||
				screenPos.y < -kTileSize ||
				screenPos.y >= 720)
			{
				continue;
			}

			int texture = floorTexture_;

			if (tiles_[y][x] == TILE_WALL)
			{
				texture = wallTexture_;
			}

			Novice::DrawSprite(
				static_cast<int>(screenPos.x),
				static_cast<int>(screenPos.y),
				texture,
				1.0f,
				1.0f,
				0.0f,
				WHITE
			);
		}
	}
}

bool Map::IsWall(int x, int y) const
{
	// マップ外は壁として扱う
	if (x < 0 || x >= kMapWidth ||
		y < 0 || y >= kMapHeight)
	{
		return true;
	}

	return tiles_[y][x] == TILE_WALL;
}

bool Map::IsWallAt(float worldX, float worldY) const
{
	// マップ外は壁として扱う
	if (worldX < 0.0f || worldY < 0.0f)
	{
		return true;
	}

	int tileX = static_cast<int>(worldX) / kTileSize;
	int tileY = static_cast<int>(worldY) / kTileSize;

	return IsWall(tileX, tileY);
}

int Map::GetWidth()
{
	return kMapWidth * kTileSize;
}

int Map::GetHeight()
{
	return kMapHeight * kTileSize;
}

int Map::GetBossWidth()
{
	return kBossMapWidth * kTileSize;
}

int Map::GetBossHeight()
{
	return kBossMapHeight * kTileSize;
}

void Map::Debug(const Camera& camera) const
{
	for (int y = 0; y < kMapHeight; y++)
	{
		for (int x = 0; x < kMapWidth; x++)
		{
			Vector2 worldPos = {
				static_cast<float>(x * kTileSize),
				static_cast<float>(y * kTileSize)
			};

			Vector2 screenPos = camera.WorldToScreen(worldPos);

			// 画面外は描画しない
			if (screenPos.x < -kTileSize ||
				screenPos.x >= 1280 ||
				screenPos.y < -kTileSize ||
				screenPos.y >= 720)
			{
				continue;
			}

			// 交互に色を変える
			unsigned int color;

			if ((x + y) % 2 == 0)
			{
				color = 0x555555FF;
			}
			else
			{
				color = 0x888888FF;
			}

			Novice::DrawBox(
				static_cast<int>(screenPos.x),
				static_cast<int>(screenPos.y),
				kTileSize,
				kTileSize,
				0.0f,
				color,
				kFillModeSolid
			);
		}
	}
}
void Map::DebugBoss(const Camera& camera) const
{
	for (int y = 0; y < kBossMapHeight; y++)
	{
		for (int x = 0; x < kBossMapWidth; x++)
		{
			Vector2 worldPos = {
				static_cast<float>(x * kTileSize),
				static_cast<float>(y * kTileSize)
			};

			Vector2 screenPos = camera.WorldToScreen(worldPos);

			// 画面外は描画しない
			if (screenPos.x < -kTileSize ||
				screenPos.x >= 1280 ||
				screenPos.y < -kTileSize ||
				screenPos.y >= 720)
			{
				continue;
			}

			// 交互に色を変える
			unsigned int color;

			if ((x + y) % 2 == 0)
			{
				color = 0x555555FF;
			}
			else
			{
				color = 0x888888FF;
			}

			Novice::DrawBox(
				static_cast<int>(screenPos.x),
				static_cast<int>(screenPos.y),
				kTileSize,
				kTileSize,
				0.0f,
				color,
				kFillModeSolid
			);
		}
	}
}


