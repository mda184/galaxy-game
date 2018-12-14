#pragma once

#include "Common.h"
#include "GameState.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

struct PlayerConfig 
{ 
    float X, Y, CX, CY, SPEED;
};

class GameState_Play : public GameState
{

protected:

    EntityManager           m_entityManager;
    std::shared_ptr<Entity> m_player;
	std::shared_ptr<Entity> m_cursor;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
	int						m_progress = 0;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_follow = false;
    
    void init(const std::string & levelPath);

    void loadLevel(const std::string & filename);

    void update();
    void spawnPlayer();
    void shoot(std::shared_ptr<Entity> entity);
    void createWeapon(Vec2 pos, std::string typeOfWeapon);
    void createAmmo(size_t clipCapacity, float delayTime, std::string typeOfWeapon, Vec2 pos);
    void reload(std::shared_ptr<Entity> entity);
    void changeWeapon(std::shared_ptr<Entity> entity, std::string typeOfWeapon);
	
    void sMovement();
    void sAI();
    void sLifespan();
    void sUserInput();
    void sAnimation();
	void sSpawn();
    void sCollision();
    void sRender();

public:

    GameState_Play(GameEngine & game, const std::string & levelPath);

};