//
// Created by luan on 04/06/23.
//

#ifndef TP_FEEDBACK_SERVER_CREATEGAME_H
#define TP_FEEDBACK_SERVER_CREATEGAME_H

#include "../Information/information.h"

class CreateGameFeedback : public Information {

public:
    std::uint32_t game_code;

    explicit CreateGameFeedback(std::uint32_t game_code);

    [[nodiscard]] std::vector<std::int8_t> serialize() const override;

    CreateGameFeedback(const CreateGameFeedback&) = delete;
    CreateGameFeedback& operator=(const CreateGameFeedback&) = delete;

    ~CreateGameFeedback() override = default;
};


#endif //TP_FEEDBACK_SERVER_CREATEGAME_H
