#include "../../../include/GameLogic/Zombies/zombie.h"
#include <random>
#include<cmath>

/* CONSTRUCTOR */

Zombie::Zombie(
    uint32_t zombie_id,
    double width,
    double height,
    double speed,
    double health) :
    zombie_id(zombie_id),
    speed(speed),
    health(health),
    width(width),
    height(height),
    position(0,0,width,height,0,0),
    att_vic(nullptr) {
}

/* COMANDOS */

void Zombie::move(
    uint8_t state,
    int8_t moveDirection) {

    switch(state) {
        case ON:
            moving = true;
            attacking = false;
            dir = moveDirection;
            dir_x = moveDirection;
            break;
        case OFF:
            moving = false;
            break;
    }
}

void Zombie::attack(uint8_t state, std::shared_ptr<Soldier> *victim) {
    switch(state) {
        case ON:
            attacking = true;
            att_vic = victim;
            moving = false;
            break;
        case OFF:
            attacking = false;
            att_vic = victim;
            break;
    }
}

void Zombie::idle(uint8_t state) {
    switch(state) {
        case ON:
            moving = attacking = false;
            break;
        case OFF:
            break;
    }
}

void Zombie::recvDamage(double damage) {
    if (damage < health) {
        health -= damage; 
        return;
    }
    alive = false;
}

/* SIMULADORES */

void Zombie::simulate(double time,
    std::map<uint32_t, std::shared_ptr<Soldier>>& soldiers,
    std::map<uint32_t, std::shared_ptr<Zombie>>& zombies, double dim_x, double dim_y) {
    if (!alive) return;
    simulateMove(time, soldiers, zombies, dim_x, dim_y);
    if (attacking) simulateAttack(time, soldiers, zombies, dim_x);
}

void Zombie::simulateMove(double time,
    std::map<uint32_t, std::shared_ptr<Soldier>>& soldiers,
    std::map<uint32_t, std::shared_ptr<Zombie>>& zombies, double dim_x, double dim_y) {

    // calculo la zona de visión del zombie.
    RadialHitbox sight_zone(position.getXPos(), position.getYPos(), sight);

    // verifico las colisiones.
    double distance = std::sqrt(std::pow(dim_x, 2) + std::pow(dim_y, 2)); // distancia maxima
    double new_distance;
    double m;
    double b;
    uint32_t victim_id;
    bool collision = false;
    for (auto i = soldiers.begin(); i != soldiers.end(); i++) {
        Position other_pos = i->second->getPosition();
        if (sight_zone.hits(other_pos)) {
            new_distance = std::sqrt(std::pow(std::abs(position.getXPos() - other_pos.getXPos()), 2) + std::pow(std::abs(position.getYPos() - other_pos.getYPos()), 2));
            if (distance > new_distance) {
                distance = new_distance;
                m = (position.getYPos() - other_pos.getYPos()) / (position.getXPos() - other_pos.getXPos());
                b = position.getYPos() - m * position.getXPos();
                // me tengo que quedar con el más cercano
                victim_id = i->first;
            }
            collision = true;
        }
    }
    if (collision) {
        std::shared_ptr<Soldier> &victim = soldiers.at(victim_id);
        double next_x;
        double next_y;
        int8_t direction;
        if (position.getXPos() > victim->getPosition().getXPos()) {
            // aca el desplazamiento en X en realidad debería depender del tiempo.
            next_x = position.getXPos() - ((position.getXPos() - victim->getPosition().getXPos()) * 0.006);
            direction = LEFT;
        } else {
            next_x = position.getXPos() + ((victim->getPosition().getXPos() - position.getXPos()) * 0.006);
            direction = RIGHT;
        }
        next_y = next_x * m + b;
        Position next_pos(next_x, next_y, getWidth(), getHeight(), dim_x, dim_y);
        if (next_pos.collides(victim->getPosition())) {
            move(OFF, getDir());
            attack(ON, &victim);
            return;
        } else {
            move(ON, direction);
            attack(OFF, nullptr);
            position = next_pos;
        }
    } else {
        move(OFF, getDir());
        attack(OFF, nullptr);
    }

}

void Zombie::simulateAttack(double time,
    std::map<uint32_t, std::shared_ptr<Soldier>>& soldiers,
    std::map<uint32_t, std::shared_ptr<Zombie>>& zombies,
    double dim_x) {
        std::shared_ptr<Soldier> victim = *att_vic;
        victim->recvDamage(40.0);
    }

/* GETTERS */

double Zombie::getWidth(void) {
    return width;
}
double Zombie::getHeight(void) {
    return height;
}

uint32_t Zombie::getId(void) {
    return zombie_id;
}

uint8_t Zombie::getAction(void) {
    if (!alive) return ZOMBIE_DEAD;
    if (moving) return ZOMBIE_RUN;
    if (attacking) return ZOMBIE_ATTACK_1;
    return ZOMBIE_IDLE;
}

int8_t Zombie::getDir(void) {
    return dir;
}

int8_t Zombie::getDirX(void) {
    return dir_x;
}

double Zombie::getHealth(void) {
    return health;
}

Position& Zombie::getPosition(void) {
    return std::ref(position);
}

const Position& Zombie::seePosition(void) const {
    return std::ref(position);
}

bool Distance_from_left_is_minor::operator()(std::shared_ptr<Zombie> below, std::shared_ptr<Zombie> above) {
        if (below->seePosition().getXPos() > above->seePosition().getXPos()) {
            return true;
        }
 
        return false;
}

bool Distance_from_right_is_minor::operator()(std::shared_ptr<Zombie> below, std::shared_ptr<Zombie> above) {
        if (below->seePosition().getXPos() < above->seePosition().getXPos()) {
            return true;
        }
 
        return false;
}

/* SETTERS */

void Zombie::setPosition(Position&& new_pos) {
    position = new_pos;
}

void Zombie::setRandomPosition(
        const std::map<uint32_t, std::shared_ptr<Soldier>> &soldiers,
        const std::map<uint32_t, std::shared_ptr<Zombie>> &zombies, double dim_x, double dim_y) {
    using std::random_device;
    using std::mt19937;
    using std::uniform_int_distribution;
    using std::uint32_t;

    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int32_t> distx(0, dim_x);
    uniform_int_distribution<int32_t> disty(0, dim_y);
    int32_t x_pos;
    int32_t y_pos;
    bool collides;
    do {
        collides = false;
        x_pos = distx(mt);
        y_pos = disty(mt);
        Position _position(x_pos, y_pos, getWidth(), getHeight(), dim_x, dim_y);
        for (auto i = soldiers.begin(); i != soldiers.end(); i++) {
            Position other_pos = i->second->getPosition();
            if (position.collides(other_pos)) {
                collides = true;
                break;
            }
        }
    } while (collides);
    Position _position(x_pos, y_pos, getWidth(), getHeight(), dim_x, dim_y);
    setPosition(std::move(_position));
}
