#include "GameState_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include<cmath>
#define PI 3.14159265

GameState_Play::GameState_Play(GameEngine & game, const std::string & levelPath)
	: GameState(game)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}

void GameState_Play::init(const std::string & levelPath)
{
	loadLevel(levelPath);
}

void GameState_Play::loadLevel(const std::string & filename)
{
	m_entityManager = EntityManager();
	std::fstream ifs(filename);
	std::string key;
	while (ifs.good()) {
		ifs >> key;

		if (key == "Player")
		{
			ifs >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED;
		}
		if (key == "Tile")
		{
			std::string animation;
			Vec2 tilePos;
			int blocksMov;
			int blocksVision;
			ifs >> animation >> tilePos.x >> tilePos.y >> blocksMov >> blocksVision;
			auto tile = m_entityManager.addEntity("tile");
			tile->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
			Vec2 animationSize = tile->getComponent<CAnimation>()->animation.getSize();
			tile->addComponent<CBoundingBox>(1, blocksMov, blocksVision);
			tile->addComponent<CTransform>(tilePos);

		}
		if (key == "NPC")
		{

			std::string animation;
			std::string npcAi;
			Vec2 windowSize = Vec2(m_game.window().getSize().x, m_game.window().getSize().y);
			ifs >> animation >> npcAi;

			if (npcAi == "CSlowFollowAi")
			{
				float speed;
				float steeringPower;
				int spawnStage;
				int killPoints;
				ifs >> spawnStage >> killPoints >> speed >> steeringPower;
				auto npc = m_entityManager.addEntity("npc");
				npc->addComponent<CBoundingBox>(30, false, false);
				npc->addComponent<CSlowFollowAi>(speed, steeringPower);
				npc->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
				npc->addComponent<CSpawn>(spawnStage, killPoints);

			}
			if (npcAi == "CDropAi")
			{
				int numberOfEntites;
				int spawnStage;
				int killPoints;
				ifs >> spawnStage >> killPoints >> numberOfEntites;
				EntityVec entiteisToDrop;
				for (int i = 0; i < numberOfEntites; i++)
				{
					float speed;
					int chargeTimer;
					ifs >> speed >> chargeTimer;
					auto chargeNpc = m_entityManager.addEntity("npc");
					chargeNpc->addComponent<CBoundingBox>(30, false, false);
					chargeNpc->addComponent<CAnimation>(m_game.getAssets().getAnimation("Drone"),true);
					chargeNpc->addComponent<CChargeAi>(speed, chargeTimer);
					entiteisToDrop.push_back(chargeNpc);
				}
				auto npc = m_entityManager.addEntity("npc");
				npc->addComponent<CDropAi>(entiteisToDrop);
				npc->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
				npc->addComponent<CSpawn>(spawnStage, killPoints);

			}
			if (npcAi == "CChargeAi")
			{
				float speed;
				int chargeTimer;
				ifs >> speed >> chargeTimer;

				auto npc = m_entityManager.addEntity("npc");
				npc->addComponent<CBoundingBox>(30, false, false);
				npc->addComponent<CChargeAi>(speed, chargeTimer);
				npc->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
			}
			if (npcAi == "CCircleAi")
			{
				float speed;
				int spawnStage;
				int shootSpeed;
				int distanceToBorder;
				int killPoints;
				ifs >> spawnStage >> killPoints >> speed >> distanceToBorder >> shootSpeed;
				auto npc = m_entityManager.addEntity("npc");
				npc->addComponent<CBoundingBox>(30, false, false);
				npc->addComponent<CCircleAi>(speed, distanceToBorder, shootSpeed);
				npc->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
				npc->addComponent<CSpawn>(spawnStage, killPoints);

			}
			if (npcAi == "CTurretAi")
			{

				int spawnStage;
				int shootSpeed;
				int killPoints;
				Vec2 position;
				ifs >> spawnStage >> killPoints >> position.x >> position.y >> shootSpeed;
				auto npc = m_entityManager.addEntity("npc");
				npc->addComponent<CBoundingBox>(30, false, false);
				npc->addComponent<CTurretAi>( position, shootSpeed);
				npc->addComponent<CSpawn>(spawnStage, killPoints);
				npc->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);

			}
			if (npcAi == "CSelfDestoryAi")
			{
				int spawnStage;
				int killPoints;
				Vec2 position;
				ifs >> spawnStage >> killPoints >> position.x >> position.y;
				auto npc = m_entityManager.addEntity("npc");
				npc->addComponent<CBoundingBox>(30, false, false);
				npc->addComponent<CSelfDestroyAi>(position);
				npc->addComponent<CSpawn>(spawnStage, killPoints);
				npc->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);


			}
		}
	}

	// spawn a sample player and sword
	spawnPlayer();
}

void GameState_Play::spawnPlayer()
{
	m_cursor = m_entityManager.addEntity("cursor");
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("Player"), true);
	m_cursor->addComponent<CAnimation>(m_game.getAssets().getAnimation("MouseCursor"), true);
	m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
	m_cursor->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
	m_cursor->getComponent<CTransform>()->scale = Vec2(0.15, 0.15);
	m_player->getComponent<CTransform>()->scale = Vec2(1.5, 1.5);

	Vec2 animationSize = m_player->getComponent<CAnimation>()->animation.getSize();
	m_player->addComponent<CBoundingBox>(30, true, true);
	m_player->addComponent<CState>("idle");
	m_player->addComponent<CInput>();

}

void GameState_Play::shoot(std::shared_ptr<Entity> entity)
{
	if(entity->hasComponent<CWeapon>())
	{
		auto entity = entity->getComponent<CWeapon>()->ammoclip;

		if(entity->curre > 0)
		{

		}
	}
	
}

//creates a weapon of a specific type and sets it position
void GameState_Play::createWeapon(Vec2 pos, std::string typeOfWeapon)
{
	auto entity = m_entityManager.addEntity("weapon");
	std::shared_ptr ammoEntity;  //needs to be changed
	entity->addComponent<CWeapon>(ammoEntity, typeOfWeapon);
	entity->addComponent<CAnimation>(m_game.getAssets().getAnimation("Weapon"), true); //needs to be changed
	//if the position is not null then set it to the position
	if(pos != Vec2(0,0))
	{
		entity->addComponent<CTransform>()->pos;
	}

	//return entity;
}

//creates an ammoclip and sets its position
void createAmmo(size_t clipCapacity, float delayTime, std::string typeOfWeapon, Vec2 pos)
{
	auto entity = m_entityManager.addEntity("ammoclip");
	entity->addComponent<CAmmoClip>(clipCapacity, clipCapacity, delayTime, typeOfWeapon);
	
	// maybe needs to be changed
	if(typeOfWeapon == "sniper")
	{
		entity->addComponent<CAnimation>(m_game.getAssets().getAnimation("Bullet1"), true);
	}
	else if((typeOfWeapon == "sniper"))
	{
		entity->addComponent<CAnimation>(m_game.getAssets().getAnimation("Bullet2"), true);
	}

	
	//if the position is not null then set it to the position
	if(pos != Vec2(0,0))
	{
		entity->addComponent<CTransform>(pos);
	}

	//return entity;
}

//reloads the current ammo clip with another new ammo clip
void reload(std::shared_ptr<Entity> entity)
{
}

//changes the weapon from one to another
void changeWeapon(std::shared_ptr<Entity> entity, std::string typeOfWeapon)
{
	if(entity->hasComponennt<CWeapon>())
	{
		entity->getComponent<CWeapon>()->weaponType = typeOfWeapon;
	}
}

void GameState_Play::update()
{
	m_entityManager.update();

	if (!m_paused)
	{
		sAI();
		sMovement();
		sLifespan();
		sCollision();
		sAnimation();
		sSpawn();
	}

	sUserInput();
	sRender();
}

void GameState_Play::sMovement()
{
	auto pInput = m_player->getComponent<CInput>();
	auto pTransfrom = m_player->getComponent<CTransform>();
	auto pBounding = m_player->getComponent<CBoundingBox>();
	auto pState = m_player->getComponent<CState>();
	pTransfrom->speed = Vec2(0, 0);

	sf::Vector2i mousePos = sf::Mouse::getPosition(m_game.window());
	float angle = std::atan2((mousePos.y - pTransfrom->pos.y), (mousePos.x - pTransfrom->pos.x));
	pTransfrom->angle = ((180 / PI) * angle) + 90;
	angle = angle + PI / 2;
	float cosAngle = cos(angle);
	float sinAngle = sin(angle);
	if (pInput->right || pInput->left)
	{
		if (pInput->right && pInput->left)
		{
			return;
		}
		else if (pInput->right)
		{
			pTransfrom->speed.x =  m_playerConfig.SPEED *  cosAngle;
			pTransfrom->speed.y =  m_playerConfig.SPEED *  sinAngle;

		}
		else
		{
			pTransfrom->speed.x = -1 * m_playerConfig.SPEED * cosAngle;
			pTransfrom->speed.y = -1 * m_playerConfig.SPEED *  sinAngle;

		}


	}

	if (pInput->up || pInput->down)
	{
		if (pInput->up && pInput->down)
		{
			return;

		}
		else if (pInput->up)
		{
			if (pTransfrom->speed.y == 0)
			pTransfrom->speed.y =  -1 * m_playerConfig.SPEED * cosAngle;
			if (pTransfrom->speed.x == 0)
			pTransfrom->speed.x =  m_playerConfig.SPEED * sinAngle;


		}
		else
		{
			if (pTransfrom->speed.y == 0)
			pTransfrom->speed.y = m_playerConfig.SPEED  * cosAngle;
			if (pTransfrom->speed.x == 0)
			pTransfrom->speed.x = -1 * m_playerConfig.SPEED * sin(angle);

		}

	}



	EntityVec enteties = m_entityManager.getEntities();
	for (auto entity : enteties)
	{
		if (entity->hasComponent<CTransform>())
		{
			auto eTransfrom = entity->getComponent<CTransform>();
			eTransfrom->prevPos = eTransfrom->pos;
			eTransfrom->pos += eTransfrom->speed;
		}



	}

}
// I will worry about this part AI.
void GameState_Play::sAI()
{
	EntityVec npcs = m_entityManager.getEntities("npc");
	for (auto npc : npcs)
	{
		if (npc->hasComponent<CTransform>())
		{
			if (npc->hasComponent<CFollowPlayer>())
			{
				EntityVec enteties = m_entityManager.getEntities();
				for (auto entity : enteties)
				{
					if (entity->tag() != "player" && entity->hasComponent<CBoundingBox>() && entity->getComponent<CBoundingBox>()->blockVision)
					{
						if (Physics::EntityIntersect(m_player->getComponent<CTransform>()->pos, npc->getComponent<CTransform>()->pos, entity))
						{
							Vec2 target = npc->getComponent<CFollowPlayer>()->home;
							auto npcTransform = npc->getComponent<CTransform>();
							auto npcFollowSpeed = npc->getComponent<CFollowPlayer>()->speed;
							float angle = std::atan2((target.y - npcTransform->pos.y), (target.x - npcTransform->pos.x));
							npcTransform->speed = Vec2(npcFollowSpeed * cos(angle), npcFollowSpeed * sin(angle));
							break;
						}
						else
						{
							Vec2 target = m_player->getComponent<CTransform>()->pos;
							auto npcTransform = npc->getComponent<CTransform>();
							auto npcFollowSpeed = npc->getComponent<CFollowPlayer>()->speed;
							float angle = std::atan2((target.y - npcTransform->pos.y), (target.x - npcTransform->pos.x));
							npcTransform->speed = Vec2(npcFollowSpeed * cos(angle), npcFollowSpeed * sin(angle));
						}
					}
				}
			}
			if (npc->hasComponent<CPatrol>())
			{
				auto npcPatrol = npc->getComponent<CPatrol>();
				Vec2 target = npcPatrol->positions[npcPatrol->currentPosition];
				auto npcTransform = npc->getComponent<CTransform>();
				auto npcPatrolSpeed = npc->getComponent<CPatrol>()->speed;
				float angle = std::atan2((target.y - npcTransform->pos.y), (target.x - npcTransform->pos.x));
				npcTransform->speed = Vec2(npcPatrolSpeed * cos(angle), npcPatrolSpeed * sin(angle));
				Vec2 distanceToTarget = Vec2(abs(target.x - npcTransform->pos.x), abs(target.y - npcTransform->pos.y));

				if (distanceToTarget.x < 1 && distanceToTarget.y < 1)
				{
					npcPatrol->currentPosition = (npcPatrol->currentPosition + 1) % npcPatrol->positions.size();
				}

			}
			if (npc->hasComponent<CSlowFollowAi>())
			{
				auto npcFollow = npc->getComponent<CSlowFollowAi>();
				auto followSpeed = npcFollow->speed;
				auto npcTransform = npc->getComponent<CTransform>();
				auto target = m_player;
				Vec2 targetPos = target->getComponent<CTransform>()->pos;
				float angle = std::atan2((targetPos.y - npcTransform->pos.y),(targetPos.x - npcTransform->pos.x));
				Vec2 desired = Vec2(followSpeed * cos(angle), followSpeed * sin(angle));
				Vec2 steering = desired - npcTransform->speed;
				steering = steering * npcFollow->steeringPower;
				Vec2 actual = npcTransform->speed + steering;
				npcTransform->speed = actual;

			}
			if (npc->hasComponent<CChargeAi>())
			{

			}
			if (npc->hasComponent<CDropAi>())
			{
				auto npcDrop = npc->getComponent<CDropAi>();
				EntityVec entitiesToDrop = npcDrop->entitiesToDrop;
				EntityVec temp = entitiesToDrop;
				int numberOfEntities = temp.size();
				for (int i = 1; i <= numberOfEntities; i++)
				{

				}

			}

		}
		
	}
}

void GameState_Play::sLifespan()
{
	EntityVec entities = m_entityManager.getEntities();
	for (auto entity : entities)
	{
		if (entity->hasComponent<CLifeSpan>())
		{
			auto entityCLifeSpan = entity->getComponent<CLifeSpan>();
			if (entityCLifeSpan->clock.getElapsedTime().asMilliseconds() > entityCLifeSpan->lifespan) {
				entity->destroy();
			}
		}
	}
}

void GameState_Play::sCollision()
{
	EntityVec npcs = m_entityManager.getEntities("npc");
	EntityVec enteties = m_entityManager.getEntities();
	auto pCTransform = m_player->getComponent<CTransform>();
	auto pBoudningBox = m_player->getComponent<CBoundingBox>();

	// npc collision.
	for (auto npc : npcs) {
		if (npc->hasComponent<CBoundingBox>() && npc->hasComponent<CTransform>()) {
			for (auto entity : enteties)
			{
				if (entity->tag() == "player" || entity->tag() == "bullet")
				{
					auto npcTransform = npc->getComponent<CTransform>();
					auto eTransform = entity->getComponent<CTransform>();
					auto npcBooundingBox = npc->getComponent<CBoundingBox>();
					auto eBooundingBox = entity->getComponent<CBoundingBox>();

					float distance = eTransform->pos.dist(npcTransform->pos);
					bool collide = distance <= eBooundingBox->radious + npcBooundingBox->radious;

					if (collide)
					{
						if (entity->tag() == "player")
						{
							//npc hit the player should lose health
						}
						else {
							// npc hit a bullet
						}
					}
				}

			}

		}

	}
	// player bounding by walls.

	if (pCTransform->pos.y + pBoudningBox->radious > m_game.window().getSize().y)
	{
		pCTransform->pos = Vec2(pCTransform->pos.x, m_game.window().getSize().y - pBoudningBox->radious);
	}
	if (pCTransform->pos.y - pBoudningBox->radious < 0)
	{
		pCTransform->pos = Vec2(pCTransform->pos.x, 0 + pBoudningBox->radious);
	}
	if (pCTransform->pos.x + pBoudningBox->radious > m_game.window().getSize().x)
	{
		pCTransform->pos = Vec2(m_game.window().getSize().x - pBoudningBox->radious, pCTransform->pos.y);
	}
	if (pCTransform->pos.x - pBoudningBox->radious < 0)
	{
		pCTransform->pos = Vec2(0 + pBoudningBox->radious, pCTransform->pos.y);
	}

}

void GameState_Play::sSpawn()
{
	sf::Vector2u windowSize = m_game.window().getSize();
	EntityVec entities = m_entityManager.getEntities();
	for (auto entity : entities)
	{
		if (entity->hasComponent<CSpawn>() && !entity->hasComponent<CTransform>())
		{
			auto cSpawn = entity->getComponent<CSpawn>();
			if (m_progress >= cSpawn->spawnStage)
			{
				if (entity->hasComponent<CDropAi>())
				{
					int splitX = (rand() % (6));
					int splitY = (rand() % (6));
					int spawnXFactor = 1 + (rand() % (splitX - 1));
					int spawnYFactor = 1 + (rand() % (splitY - 1));
					Vec2 spawnPos = Vec2((windowSize.x / splitX) *  spawnXFactor, (windowSize.y / splitY)  * spawnYFactor);
					entity->addComponent<CTransform>(spawnPos);
					entity->addComponent<CLifeSpan>(10000);
				}
				else if (entity->hasComponent<CTurretAi>())
				{
					entity->addComponent<CTransform>(entity->getComponent<CTurretAi>()->turretPos);
				}
				else
				{
					int leftOrRight = -1 + ((rand() % 2) * 2);
					int upOrDown = -1 + ((rand() % 2) * 2);
					int randomYPosition = (rand() % (1 + windowSize.y)) * upOrDown;
					int randomXPosition = (rand() % (1 + windowSize.x)) * leftOrRight;
					entity->addComponent<CTransform>(Vec2(randomXPosition, randomYPosition));
				}
			}
		}
	}
}

void GameState_Play::sAnimation()
{
	auto pState = m_player->getComponent<CState>();
	auto pAnimation = m_player->getComponent<CAnimation>();
	auto pTranfrom = m_player->getComponent<CTransform>();
	EntityVec entites = m_entityManager.getEntities();
	for (auto entity : entites)
	{

		if (entity->hasComponent<CAnimation>())
		{
			auto entityAnimation = entity->getComponent<CAnimation>();

			entityAnimation->animation.update();
			if (!entityAnimation->repeat && entityAnimation->animation.hasEnded())
			{
				entity->destroy();
			}
		}
	}
}

void GameState_Play::sUserInput()
{
	auto pInput = m_player->getComponent<CInput>();

	sf::Event event;
	while (m_game.window().pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_game.quit();
		}
		// this event is triggered when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape: { m_game.popState(); break; }
			case sf::Keyboard::W: { pInput->up = true; break; }
			case sf::Keyboard::A: { pInput->left = true; break; }
			case sf::Keyboard::S: { pInput->down = true; break; }
			case sf::Keyboard::D: { pInput->right = true; break; }
			case sf::Keyboard::Z: { init(m_levelPath); break; }
			case sf::Keyboard::R: { m_drawTextures = !m_drawTextures; break; }
			case sf::Keyboard::F: { m_drawCollision = !m_drawCollision; break; }
			case sf::Keyboard::Y: { m_follow = !m_follow; break; }
			case sf::Keyboard::P: { setPaused(!m_paused); break; }
			case sf::Keyboard::Space: { shoot(m_player); break; }
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W: { pInput->up = false; break; }
			case sf::Keyboard::A: { pInput->left = false; break; }
			case sf::Keyboard::S: { pInput->down = false; break; }
			case sf::Keyboard::D: { pInput->right = false; break; }
			}
		}
	}
}

void GameState_Play::sRender()
{

	sf::Sprite background;
	background.setTexture(m_game.getAssets().getTexture("TexBackGround"));
	background.setPosition(0, 0);
	m_game.window().draw(background);
	
	// draw all Entity textures / animations
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities())
		{

			if (e->hasComponent<CAnimation>() && e->hasComponent<CTransform>())
			{
				auto transform = e->getComponent<CTransform>();
				auto animation = e->getComponent<CAnimation>()->animation;
				animation.getSprite().setRotation(transform->angle);
				animation.getSprite().setPosition(transform->pos.x, transform->pos.y);
				animation.getSprite().setScale(transform->scale.x, transform->scale.y);
				m_game.window().draw(animation.getSprite());
			}
		}
	}

	// draw all Entity collision bounding boxes with a rectangleshape
	if (m_drawCollision)
	{
		sf::CircleShape dot(4);
		dot.setFillColor(sf::Color::Black);
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto box = e->getComponent<CBoundingBox>();
				auto transform = e->getComponent<CTransform>();
				sf::CircleShape circle;
				circle.setRadius(box->radious);
				circle.setOrigin(0 + box->radious, 0 + box->radious);
				circle.setPosition(transform->pos.x, transform->pos.y);
				circle.setFillColor(sf::Color(0, 0, 0, 0));

				if (box->blockMove && box->blockVision) { circle.setOutlineColor(sf::Color::White); }
				if (box->blockMove && !box->blockVision) { circle.setOutlineColor(sf::Color::White); }
				if (!box->blockMove && box->blockVision) { circle.setOutlineColor(sf::Color::White); }
				if (!box->blockMove && !box->blockVision) { circle.setOutlineColor(sf::Color::White); }
				circle.setOutlineThickness(1);
				m_game.window().draw(circle);
			}

			if (e->hasComponent<CPatrol>())
			{
				auto & patrol = e->getComponent<CPatrol>()->positions;
				for (size_t p = 0; p < patrol.size(); p++)
				{
					dot.setPosition(patrol[p].x, patrol[p].y);
					m_game.window().draw(dot);
				}
			}

			if (e->hasComponent<CFollowPlayer>())
			{
				sf::VertexArray lines(sf::LinesStrip, 2);
				lines[0].position.x = e->getComponent<CTransform>()->pos.x;
				lines[0].position.y = e->getComponent<CTransform>()->pos.y;
				lines[0].color = sf::Color::Black;
				lines[1].position.x = m_player->getComponent<CTransform>()->pos.x;
				lines[1].position.y = m_player->getComponent<CTransform>()->pos.y;
				lines[1].color = sf::Color::Black;
				m_game.window().draw(lines);
				dot.setPosition(e->getComponent<CFollowPlayer>()->home.x, e->getComponent<CFollowPlayer>()->home.y);
				m_game.window().draw(dot);
			}
		}
	}
	m_game.window().setMouseCursorVisible(false);
	sf::Vector2i mousePos = sf::Mouse::getPosition(m_game.window());
	m_cursor->getComponent<CTransform>()->pos = Vec2(mousePos.x, mousePos.y);
	m_game.window().display();
}
