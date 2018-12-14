#pragma once

#include <bitset>
#include <array>
#include "Animation.h"
#include "Assets.h"

class Component;
class Entity;
typedef std::vector<std::shared_ptr<Entity>> EntityVec;

const size_t MaxComponents = 32;

class Component
{
public:
    virtual ~Component() {}
};

class CTransform : public Component
{
public:
    Vec2 pos        = { 0.0, 0.0 };
    Vec2 prevPos    = { 0.0, 0.0 };
    Vec2 scale      = { 1.0, 1.0 };
    Vec2 speed      = { 0.0, 0.0 };
    Vec2 facing     = { 1.0, 0.0 };
    float angle = 0;

    CTransform(const Vec2 & p = { 0, 0 })
        : pos(p), angle(0) {}
    CTransform(const Vec2 & p, const Vec2 & sp, const Vec2 & sc, float a)
        : pos(p), prevPos(p), speed(sp), scale(sc), angle(a) {}

};
class CSpawn : public Component
{
public:
	int spawnStage;
	int levelKillScore;
	CSpawn(int spawn, int killScore) : spawnStage(spawn), levelKillScore(killScore) {}
};

class CChargeAi : public Component
{
public:
	float speed = 1;
	int chargeTimer = 0;
	sf::Clock clock;
	CChargeAi(float speed,int charge) : speed(speed),  chargeTimer(charge) {}
};

class CDropAi : public Component
{
public:
	EntityVec entitiesToDrop;
	CDropAi(EntityVec entities) : entitiesToDrop(entities) {}
};


class CCircleAi : public Component
{
public:
	float speed = 1;
	int distanceFromBorder;
	sf::Clock clock;
	int shootSpeed;
	CCircleAi(float speed,int distance, int shootSpeed) : speed(speed), distanceFromBorder(distance), shootSpeed(shootSpeed) {}
};

class CSlowFollowAi : public Component
{
public:
	float speed = 1;
	float steeringPower;
	CSlowFollowAi(float speed, float steer) : speed(speed),steeringPower(steer) {}
};

class CSelfDestroyAi : public Component
{
public:
	Vec2 positionToDie;
	CSelfDestroyAi(Vec2 pos) : positionToDie(pos) {}

};

class CTurretAi : public Component
{
public:
	Vec2 turretPos;
	int shootSpeed;
	sf::Clock clock;
	CTurretAi(Vec2 pos, int shootSpeed) : shootSpeed(shootSpeed), turretPos(pos) {}
};

class CBlackHole : public Component
{
public:
	float strength;
	CBlackHole(float strength) : strength(strength) {}
};



class CLifeSpan : public Component
{
public:
    sf::Clock clock;
    int lifespan = 0;
    
    CLifeSpan(int l) : lifespan(l) {}
};

class CInput : public Component
{
public:
    bool up         = false;
    bool down       = false;
    bool left       = false;
    bool right      = false;
    bool shoot      = false;
    bool canShoot   = true;

    CInput() {}
};

class CBoundingBox : public Component
{
public:
	float radious = 0;
    bool blockMove = false;
    bool blockVision = false;
    CBoundingBox(float r, bool m, bool v)
        : radious(r), blockMove(m), blockVision(v) {}
};

class CAnimation : public Component
{
public:
    Animation animation;
    bool repeat;

    CAnimation(const Animation & animation, bool r)
        : animation(animation), repeat(r) {}
};

class CGravity : public Component
{
public:
    float gravity;
    CGravity(float g) : gravity(g) {}
};

class CState : public Component
{
public:
    std::string state = "attack";
    size_t frames = 0;
    CState(const std::string & s) : state(s) {}
};

class CFollowPlayer : public Component
{
public:
    Vec2 home = { 0, 0 };
    float speed = 0;
    CFollowPlayer(Vec2 p, float s)
        : home(p), speed(s) {}
    
};

class CHealth : public Component
{
public:
	int health;
	bool isDead = false;
	CHealth(int health) : health(health) {}
};

class CPatrol : public Component
{
public:
    std::vector<Vec2> positions;
    size_t currentPosition = 0;
    float speed = 0;
    CPatrol(std::vector<Vec2> & pos, float s) : positions(pos), speed(s) {}
};

class CWeapon : public Component
{
public:
    std::shared_ptr<Vec2> ammoClips;
    std::string weaponType;
    CWeapon(std::shared_ptr<Vec2> & ammo_clip, std::string type) : ammoClips(ammo_clip), weaponType(type){}
};

class CAmmoClip : public Component
{
public:
    size_t currentAmmo;
    size_t clipCapacity;
    float reloadDelayTime = 0;
    float bulletSpeed = 0;
    std::string type;
    CAmmoClip(size_t cur_ammo, size_t clip_cap, float delay_time, float bullet_speed, std::string type) : currentAmmo(cur_ammo), clipCapacity(clip_cap), reloadDelayTime(delay_time), bulletSpeed(bullet_speed), type(type){}
};