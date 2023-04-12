#include "Monster.hpp"

#include "Engine.hpp"
#include "ResourceManager.hpp"
#include "Player.hpp"

#include <random>

using namespace Villain;

const float SCALE = 0.7f;
const float START = 0.0f;
const float SIZE_Y = SCALE * 2;
// Crazy constant to scale image and keep aspect ratio of monster sprite
const float SIZE_X = (float)((double)SIZE_Y / (1.9310344827586206896551724137931 * 2.0));

const float OFFSET_X = 0.05f;
const float OFFSET_Y = 0.01f;
const float TEX_MIN_X = -OFFSET_X;
const float TEX_MAX_X = 1 - OFFSET_X;
const float TEX_MIN_Y = -OFFSET_Y;
const float TEX_MAX_Y = 1 - OFFSET_Y;

const float MOVE_SPEED = 0.8f;
const float CHASE_STOP_DISTANCE = 2.0f;
const float MONSTER_WIDTH = 0.2;
const float MONSTER_LENGTH = 0.2;
const float SHOOT_DISTANCE = 100.0f;

Monster::Monster(Level* level) : MeshRenderer<VertexP1N1UV>(nullptr, Material()), currentLevel(level), deathTime(0.0f) {

    currentState = AIState::STATE_IDLE;
    material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("walk1")}, 8};
    std::vector<VertexP1N1UV> vertices;

    // NOTE: Now Mesh class only needs vertices and indices and it could contain methods to build common primitives like these Also need an easy way to generate UV coords in 3D, and calculate normals vertices.push_back({glm::vec3(Start, Start, Start), glm::vec3(0.0f), glm::vec2(0.5f, 0.f)}); vertices.push_back({glm::vec3(Start, Height, Start), glm::vec3(0.0f), glm::vec2(0.5f, 0.25f)});
    vertices.push_back({glm::vec3(-SIZE_X, START, START), glm::vec3(0.0f), glm::vec2(TEX_MAX_X, 1 - TEX_MAX_Y)});
    vertices.push_back({glm::vec3(-SIZE_X, SIZE_Y, START), glm::vec3(0.0f), glm::vec2(TEX_MAX_X, 1 - TEX_MIN_Y)});
    vertices.push_back({glm::vec3(SIZE_X, SIZE_Y, START), glm::vec3(0.0f), glm::vec2(TEX_MIN_X, 1 - TEX_MIN_Y)});
    vertices.push_back({glm::vec3(SIZE_X, START, START), glm::vec3(0.0f), glm::vec2(TEX_MIN_X, 1 - TEX_MAX_Y)});

    std::vector<unsigned int> indices{0, 1, 2, 0, 2, 3};

    mesh = new Mesh<VertexP1N1UV>(vertices, indices);
}

void Monster::update(float deltaTime) {
    // Keep at same y height
    glm::vec3 pos = GetTransform()->getPos();
    pos.y = 0.0f;
    GetTransform()->setPos(pos);


    glm::vec3 directionToCamera = GetTransform()->getPos()
        - parent->getEngine()->getRenderingEngine()->getMainCamera()->getPosition();
    distance = glm::length(directionToCamera);
    orientation = directionToCamera / distance;

    // Monster always faces camera
    float angleToCameraFace = glm::degrees(atanf(directionToCamera.z / directionToCamera.x));
    if (directionToCamera.x > 0.0f) {
        angleToCameraFace += 180.0f;
    }
    GetTransform()->setEulerRot(0.0f, angleToCameraFace + 90.0f, 0.0f );

    switch (currentState) {
        case AIState::STATE_IDLE: idleUpdate(deltaTime); break;
        case AIState::STATE_CHASE: chaseUpdate(deltaTime); break;
        case AIState::STATE_ATTACK: attackUpdate(deltaTime); break;
        case AIState::STATE_DYING: dyingUpdate(deltaTime); break;
        case AIState::STATE_DEAD: deadUpdate(deltaTime); break;
        default:
            std::cerr << "AI State not found!\n";
            break;
    }
}

void Monster::idleUpdate(float deltaTime) {
    float time = timer.readSeconds();
    float timeDecimals = time - (float)(int)(time);
    if (timeDecimals < 0.5f) {
        canLook = true;
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("walk1")}, 8};
    } else {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("walk2")}, 8};
        if (canLook) {
            // NOTE: almost exact copy of attack update, horrible practise and essentially a hack for now
            glm::vec2 lineStart(GetTransform()->getPos().x, GetTransform()->getPos().z);
            glm::vec2 castDirection(-orientation.x, -orientation.z);
            glm::vec2 lineEnd = lineStart + castDirection * SHOOT_DISTANCE;
            glm::vec3 playerPos3 = parent->getEngine()->getRenderingEngine()->getMainCamera()->getPosition();
            glm::vec2 playerPos(playerPos3.x, playerPos3.z);
            glm::vec2 playerSize(Player::PLAYER_SIZE);

            glm::vec2 collision = currentLevel->checkIntersections(lineStart, lineEnd, false);

            if (collision == glm::vec2(0.0f) || (glm::length(playerPos - lineStart) < glm::length(collision - lineStart))) {
                currentState = AIState::STATE_CHASE;
            }

            canLook = false;
        }
    }
}

void Monster::chaseUpdate(float deltaTime) {
    float time = timer.readSeconds();
    float timeDecimals = time - (float)(int)(time);

    // HACKY Animations
    if (timeDecimals < 0.25f) {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("walk1")}, 8};
    } else if (timeDecimals < 0.5f){
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("walk2")}, 8};
    } else if (timeDecimals < 0.75f){
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("walk3")}, 8};
    } else {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("walk4")}, 8};
    }

    // TODO: add some randomisation here to start attack state
    if (distance > CHASE_STOP_DISTANCE) {
        float moveAmt = -MOVE_SPEED * deltaTime;
        glm::vec3 oldPos = GetTransform()->getPos();
        glm::vec3 newPos = GetTransform()->getPos() + (orientation * moveAmt);

        glm::vec3 collision = currentLevel->checkCollisions(oldPos, newPos, MONSTER_WIDTH, MONSTER_LENGTH);
        glm::vec3 movement;
        movement.x = orientation.x * collision.x;
        movement.y = orientation.y * collision.y;
        movement.z = orientation.z * collision.z;

        if (movement.length() > 0)
            GetTransform()->translatePosition(movement * moveAmt);

        if (glm::length(movement - orientation) != 0) {
            currentLevel->openDoors(GetTransform()->getPos());
        }
    } else {
        currentState = AIState::STATE_ATTACK;
    }
}

void Monster::attackUpdate(float deltaTime) {
    // TODO: Rotate cast direction by random float
    //std::mt19937 rndEngine((unsigned int)time(nullptr));
    //std::uniform_real_distribution<float> rand(-0.5f, 0.5f);

    float time = timer.readSeconds();
    float timeDecimals = time - (float)(int)(time);
    if (timeDecimals < 0.25f) {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("fire1")}, 8};
    } else if (timeDecimals < 0.5f) {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("fire2")}, 8};
    } else if (timeDecimals < 0.75f) {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("fire3")}, 8};
        if (canAttack) {
            glm::vec2 lineStart(GetTransform()->getPos().x, GetTransform()->getPos().z);
            glm::vec2 castDirection(-orientation.x, -orientation.z);
            glm::vec2 lineEnd = lineStart + castDirection * SHOOT_DISTANCE;
            glm::vec3 playerPos3 = parent->getEngine()->getRenderingEngine()->getMainCamera()->getPosition();
            glm::vec2 playerPos(playerPos3.x, playerPos3.z);
            glm::vec2 playerSize(Player::PLAYER_SIZE);

            glm::vec2 collision = currentLevel->checkIntersections(lineStart, lineEnd, false);

            glm::vec2 playerIntersect = currentLevel->lineIntersectRect(lineStart, lineEnd, playerPos, playerSize);
            if (playerIntersect != glm::vec2(0.0f) && (collision == glm::vec2(0.0f)
                || (glm::length(playerIntersect - lineStart) < glm::length(collision - lineStart)))) {
                currentLevel->damagePlayer(getDamage());
                std::cout << "Player shot!\n";
            }

            currentState = AIState::STATE_CHASE;
            canAttack = false;
        }
    } else {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("fire2")}, 8};
        canAttack = true;
    }
}

void Monster::dyingUpdate(float deltaTime) {
    float time = timer.readSeconds();
    float timeDecimals = time - (float)(int)(time);

    if (deathTime == 0.0f) {
        deathTime = time;
    }

    // keyframes
    const float time1 = 0.1f;
    const float time2 = 0.3f;
    const float time3 = 0.45f;
    const float time4 = 0.6f;

    if (time < deathTime + time1) {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("die1")}, 8};
    } else if (time < deathTime + time2) {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("die2")}, 8};
    } else if (time < deathTime + time3) {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("die3")}, 8};
    } else if (time < deathTime + time4) {
        material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("die4")}, 8};
    } else {
        currentState = AIState::STATE_DEAD;
    }

}

void Monster::deadUpdate(float deltaTime) {
    material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("death")}, 8};
    GetTransform()->getPos().y = 0.01f; // prevent z-fighting
    GetTransform()->setEulerRot(90.0f, 0.0f, 0.0f);
}

void Monster::damage(int amount) {
    if (currentState == AIState::STATE_IDLE) {
        currentState = AIState::STATE_CHASE;
    }
    material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->getTexture("pain")}, 8};
    health -= amount;

    if (health <= 0) {
        currentState = AIState::STATE_DYING;
    }
}

glm::vec2 Monster::getSize() {
    return glm::vec2(MONSTER_WIDTH, MONSTER_LENGTH);
}
