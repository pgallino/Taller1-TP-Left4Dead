#ifndef ZOMBIE_H_
#define ZOMBIE_H_

#include "../position.h"
#include "../hitbox.h"
#include "../radialhitbox.h"
#include "../../../../Common/include/Information/information_code.h"
#include "../../../include/GameLogic/Soldiers/soldier.h"

#include <utility>
#include <cstdint>
#include <map>
#include <memory>

class Zombie {
public:
    uint32_t zombie_id;
    int8_t dir = RIGHT;
    uint8_t axis = 0;
    double speed;
    double health;
    double width;
    double height;
    double sight = 200.0;
    Position position;
    int8_t dir_x = RIGHT;
    bool moving = false;
    bool attacking = false;
    bool alive = true;

    virtual ~Zombie() {}

    explicit Zombie(
    uint32_t zombie_id,
    double width,
    double height,
    double speed,
    double health);

    /* COMANDOS */

    virtual void move(
    uint8_t state,
    uint8_t moveAxis,
    int8_t moveDirection,
    uint8_t moveForce);
    virtual void attack(uint8_t state);
    virtual void idle(uint8_t state);
    virtual void recvDamage(double damage);

    /* SIMULADORES */

    virtual void simulate(double time,
    std::map<uint32_t, std::shared_ptr<Soldier>>& soldiers,
    std::map<uint32_t, std::shared_ptr<Zombie>>& zombies, double dim_x, double dim_y);
    virtual void simulateMove(double time,
    std::map<uint32_t, std::shared_ptr<Soldier>>& soldiers,
    std::map<uint32_t, std::shared_ptr<Zombie>>& zombies, double dim_x, double dim_y);
    virtual void simulateAttack(double time,
    std::map<uint32_t, std::shared_ptr<Soldier>>& soldiers,
    std::map<uint32_t, std::shared_ptr<Zombie>>& zombies,
    double dim_x);

    /* GETTERS */

    int8_t getDir(void);
    int8_t getDirX(void);
    double getHealth(void);
    double getWidth(void);
    double getHeight(void);
    uint32_t getId(void);
    virtual uint8_t getZombieType(void) = 0;
    uint8_t getAction(void);
    Position& getPosition(void);
    const Position& seePosition(void) const;

    /* SETTERS */

    void setPosition(Position&& new_pos);
    void setRandomPosition(
            const std::map<uint32_t, std::shared_ptr<Soldier>> &soldiers,
            const std::map<uint32_t, std::shared_ptr<Zombie>> &zombies, double dim_x, double dim_y);

};

/* COMPARADORES PARA LA COLA DE PRIORIDAD DE SCOUT */

class Distance_from_left_is_minor {
public:
    bool operator()(std::shared_ptr<Zombie> below, std::shared_ptr<Zombie> above);
};

class Distance_from_right_is_minor {
public:
    bool operator()(std::shared_ptr<Zombie> below, std::shared_ptr<Zombie> above);
};

#endif  // ZOMBIE_H_
