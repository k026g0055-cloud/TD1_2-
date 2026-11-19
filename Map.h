#pragma once

#include "GameConfig.h"

class Camera;

class Map
{
public:
    void Initialize();

    void Draw(const Camera& camera) const;
    void DrawBoss(const Camera& camera) const;
    void Debug(const Camera& camera)const;
    void DebugBoss(const Camera& camera)const;

    // 指定したマスが壁かどうか
    bool IsWall(int x, int y) const;

    // ワールド座標が壁の中かどうか
    bool IsWallAt(float worldX, float worldY) const;

    // マップのサイズを取得
    static int GetWidth();
    static int GetHeight();

	static int GetBossWidth();
    static int GetBossHeight();

private:
    enum TileType
    {
        TILE_FLOOR,
        TILE_WALL
    };

    int tiles_[kMapHeight][kMapWidth] = {};

    int floorTexture_ = -1;
    int wallTexture_ = -1;
};