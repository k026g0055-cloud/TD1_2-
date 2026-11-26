#pragma once

#include "Vector2.h"
#include <algorithm>

class collision
{
public:
	//軸に平行な四角と楕円が重なっているか調べる
	//マグマの当たり判定
	static bool IntersectsBoxEllipse(
		Vector2 boxCenter, Vector2 boxHalfSize,
		Vector2 ellipseCenter, Vector2 ellipseRadius) {
		if (ellipseRadius.x <= 0.0f || ellipseRadius.y <= 0.0f) {
			return false;
		}

		//四角の中で、楕円の中心に最も近い点を求める
		const float closetX = std::clamp(
			ellipseCenter.x,
			boxCenter.x - boxHalfSize.x,
			boxCenter.x + boxHalfSize.x
		);

		const float closetY = std::clamp(
			ellipseCenter.y,
			boxCenter.y - boxHalfSize.y,
			boxCenter.y + boxHalfSize.y
		);

		//その点が楕円の内側か、境界上にあれば当たり
		const float dx = (closetX - ellipseCenter.x) / ellipseRadius.x;
		const float dy = (closetY - ellipseCenter.y) / ellipseRadius.y;
		return dx * dx + dy * dy <= 1.0f;
	}

	//火炎放射当たり判定
	static bool IntersectsCircleCapsule(
		Vector2 circleCenter, float circleRadius,
		Vector2 segmentStart, Vector2 segmentEnd, float segmentRadius) {
		const float vx = segmentEnd.x - segmentStart.x;
		const float vy = segmentEnd.y - segmentStart.y;
		const float lengthSquared = vx * vx + vy * vy;

		if (lengthSquared <= 0.001f) {
			return false;
		}
		const float px = circleCenter.x - segmentStart.x;
		const float py = circleCenter.y - segmentStart.y;
		const float t = std::clamp(
			(px * vx + py * vy) / lengthSquared, 0.0f, 1.0f
		);

		const float dx = px - vx * t;
		const float dy = py - vy * t;
		const float hitRadius = circleRadius + segmentRadius;

		return dx * dx + dy * dy <= hitRadius * hitRadius;
	}
};

