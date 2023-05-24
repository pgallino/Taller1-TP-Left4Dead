#ifndef ACTIONDTO_H_
#define ACTIONDTO_H_

#include <cstdint>
#include <vector>

class Action {

public:
    explicit Action();
    [[nodiscard]] virtual std::vector<int8_t> serialize() const
    = 0;

    Action(Action&&) = default;
    Action& operator=(Action&&) = default;

    virtual ~Action() = default;
};

class StartShootAction : public Action {

public:
    explicit StartShootAction();
    [[nodiscard]] virtual std::vector<int8_t> serialize() const override;

    ~StartShootAction() = default;
};

#endif  // ACTIONDTO_H_
