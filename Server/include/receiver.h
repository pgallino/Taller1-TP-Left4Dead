// Copyright [2023] pgallino

#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <atomic>
#include <vector>
#include "../../libs/thread.h"
#include "../../libs/queue.h"
#include "protocol.h"
#include "../../Common/include/Socket/socket_game.h"
#include "sender.h"
#include "game_manager.h"

class Receiver: public Thread {
private:
    GameSocket peer;
    Protocol protocol;
    // Queue<int>& commands_queue;
    Queue<GameState*> send_state_queue;
    Queue<Command*>* game_queue;
    Sender sender;
    GameManager& game_manager;
    std::atomic<bool> is_running;
    std::atomic<bool> keep_talking;
    std::atomic<bool> joined;

    std::uint8_t player_id;  // id that changes when joining a game.

    Action* recv_action;

protected:
    virtual void run() override;

public:
    explicit Receiver(GameSocket&& peer, GameManager& game_manager);

    [[nodiscard]] bool isDead() const;

    void stop();

    Receiver(const Receiver&) = delete;
    Receiver& operator=(const Receiver&) = delete;

    Receiver(Receiver&&) = delete;
    Receiver& operator=(Receiver&&) = delete;

    virtual ~Receiver() override;
};

#endif  // RECEIVER_H_
