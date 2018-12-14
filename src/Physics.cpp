#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	/*
	Vec2 aPos = a->getComponent<CTransform>()->pos;
	Vec2 bPos = b->getComponent<CTransform>()->pos;

	Vec2 aHalfSize = a->getComponent<CBoundingBox>()->halfSize;
	Vec2 bHalfSize = b->getComponent<CBoundingBox>()->halfSize;

	Vec2 delta = Vec2(abs(aPos.x - bPos.x), abs(aPos.y - bPos.y));

	float xOverlap = aHalfSize.x + bHalfSize.x - delta.x;
	float yOverlap = aHalfSize.y + bHalfSize.y - delta.y;
	return Vec2(xOverlap, yOverlap);
	*/
	return Vec2(0, 0);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	/*
	Vec2 aPos = a->getComponent<CTransform>()->prevPos;
	Vec2 bPos = b->getComponent<CTransform>()->pos;

	Vec2 aHalfSize = a->getComponent<CBoundingBox>()->halfSize;
	Vec2 bHalfSize = b->getComponent<CBoundingBox>()->halfSize;

	Vec2 delta = Vec2(abs(aPos.x - bPos.x), abs(aPos.y - bPos.y));

	float xOverlap = aHalfSize.x + bHalfSize.x - delta.x;
	float yOverlap = aHalfSize.y + bHalfSize.y - delta.y;
	return Vec2(xOverlap, yOverlap);
	*/
	return Vec2(0, 0);

}

Intersect Physics::LineIntersect(const Vec2 & a, const Vec2 & b, const Vec2 & c, const Vec2 & d)
{
	/*
	Vec2 r = b - a;
	Vec2 s = d - c;
	float rxs = (r.x * s.y) - (r.y * s.x);
	Vec2 cma = c - a;
	float t = ((cma.x * s.y) - (cma.y * s.x)) / rxs;
	float u = ((cma.x * r.y) - (cma.y * r.x)) / rxs;
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
	{
		return { true , Vec2(a.x + t * r.x,a.y + t * r.y) };
	}

	return { false, Vec2(0,0) };
	*/
	return { false,Vec2(0, 0) };
}

bool Physics::EntityIntersect(const Vec2 & a, const Vec2 & b, std::shared_ptr<Entity> e)
{
	/*
	std::vector<Vec2> corners;
	auto eBoundingBox = e->getComponent<CBoundingBox>();
	Vec2 ePos = e->getComponent<CTransform>()->pos;
	corners.push_back(Vec2(ePos.x - eBoundingBox->halfSize.x, ePos.y - eBoundingBox->halfSize.y));
	corners.push_back(Vec2(ePos.x - eBoundingBox->halfSize.x, ePos.y + eBoundingBox->halfSize.y));
	corners.push_back(Vec2(ePos.x + eBoundingBox->halfSize.x, ePos.y + eBoundingBox->halfSize.y));
	corners.push_back(Vec2(ePos.x + eBoundingBox->halfSize.x, ePos.y - eBoundingBox->halfSize.y));
	for (int i = 0; i < 4; i++)
	{
		Intersect intersect = LineIntersect(a, b, corners[i], corners[(i + 1) % 4]);
		if (intersect.result)
			return true;

	}
	return false;
	*/
	return false;
}