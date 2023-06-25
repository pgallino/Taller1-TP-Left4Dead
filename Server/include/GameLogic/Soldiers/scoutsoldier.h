#ifndef SCOUTSOLDIER_H_
#define SCOUTSOLDIER_H_

#include "soldier.h"

class ScoutSoldier : public Soldier {

public:
    explicit ScoutSoldier(
    uint32_t soldier_id,
    double width,
    double height,
    double speed,
    double health,
    std::unique_ptr<Weapon>&& weapon);

    virtual uint8_t getSoldierType(void) override;

    virtual uint8_t getAction(void) override;

    virtual ~ScoutSoldier() override = default;
};

#endif  // SCOUTSOLDIER_H_
