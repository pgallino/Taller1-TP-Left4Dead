#include <gtest/gtest.h>
#include "game_manager.h"
#include "Command/command_ingame_startshoot.h"

enum PlayerNum : std::uint8_t {
    PLAYER_1,
    PLAYER_2,
    PLAYER_3,
    PLAYER_4,
    PLAYER_5,
    PLAYER_6,
};

enum GameNum : std::uint8_t {
    GAME_1,
    GAME_2,
};

TEST(gamemanager_test, CreateTest00CreateGameShouldChangeGameQueuePointer) {
    Queue<InGameCommand*>* game_q = nullptr;
    Queue<GameState*> player_q(10);
    std::uint8_t player_id;
    GameManager manager = GameManager();

    manager.createGame(game_q, player_q, &player_id);
    ASSERT_NE(game_q, nullptr);
}

TEST(gamemanager_test,
     CreateTest01GameQueueReceivedByGameManagerShouldBeValid) {
    Queue<InGameCommand*>* game_q = nullptr;
    Queue<GameState*> player_q(10);
    std::uint8_t player_id;
    GameManager manager = GameManager();

    manager.createGame(game_q, player_q, &player_id);
    std::shared_ptr<InGameCommand> shoot_cmd(new StartShootCommand(player_id));
    ASSERT_NO_FATAL_FAILURE(game_q->push(shoot_cmd.get()));
}

TEST(gamemanager_test, CreateTest02CreateGameUpdatesPlayerID) {
    Queue<InGameCommand*>* game_q = nullptr;
    Queue<GameState*> player_q(10);
    std::uint8_t player_id = 0;
    GameManager manager = GameManager();

    manager.createGame(game_q, player_q, &player_id);
    ASSERT_NE(player_id, 0);
}

TEST(gamemanager_test,
     CreateTest03CreatingTwoGamesTheirGameCodesShouldBeDifferent) {
    GameManager manager = GameManager();
    Queue<InGameCommand*>* game_q1 = nullptr;
    Queue<GameState*> player_q1(10);
    std::uint8_t player_id1 = 0;

    std::uint32_t game_code1 = manager.createGame(game_q1, player_q1,
                                                  &player_id1);

    Queue<InGameCommand*>* game_q2 = nullptr;
    Queue<GameState*> player_q2(10);
    std::uint8_t player_id2 = 0;

    std::uint32_t game_code2 = manager.createGame(game_q2, player_q2,
                                                  &player_id2);

    ASSERT_NE(game_code1, game_code2);
}

TEST(gamemanager_test,
     CreateTest04CreatingTwoGamesTheQueuePointersShouldBeDifferent) {
    GameManager manager = GameManager();
    Queue<InGameCommand*>* game_q1 = nullptr;
    Queue<GameState*> player_q1(10);
    std::uint8_t player_id1 = 0;

    manager.createGame(game_q1, player_q1,
                       &player_id1);

    Queue<InGameCommand*>* game_q2 = nullptr;
    Queue<GameState*> player_q2(10);
    std::uint8_t player_id2 = 0;

    manager.createGame(game_q2, player_q2,
                       &player_id2);

    ASSERT_NE(game_q1, game_q2);
}
/*
 * A Test to verify that the player_q is successfully given to the Game is
 * needed when creating one. Game may push something into the queue.
 * If Game doesn't push anything (maybe it waits for specific player input
 * first) then mocking could be useful.
 */

TEST(gamemanager_test, JoinTest00JoiningValidGameShouldReturnTrue) {
    GameManager manager = GameManager();
    Queue<InGameCommand*>* game_q1 = nullptr;
    Queue<GameState*> player_q1(10);
    std::uint8_t player_id1 = 0;

    std::uint32_t game_code = manager.createGame(game_q1, player_q1,
                                                &player_id1);

    Queue<InGameCommand*>* game_q2 = nullptr;
    Queue<GameState*> player_q2(10);
    std::uint8_t player_id2 = 0;

    bool success = manager.joinGame(game_q2, player_q2, &player_id2, game_code);

    ASSERT_TRUE(success);
}

TEST(gamemanager_test, JoinTest01JoiningInvalidGameShouldReturnFalse) {
    GameManager manager = GameManager();
    Queue<InGameCommand*>* game_q1 = nullptr;
    Queue<GameState*> player_q1(10);
    std::uint8_t player_id1 = 0;

    std::uint32_t game_code = manager.createGame(game_q1, player_q1,
                                                 &player_id1);

    Queue<InGameCommand*>* game_q2 = nullptr;
    Queue<GameState*> player_q2(10);
    std::uint8_t player_id2 = 0;

    bool success = manager.joinGame(game_q2, player_q2, &player_id2,
                                    game_code + 1);

    ASSERT_FALSE(success);
}

TEST(gamemanager_test, JoinTest02JoiningValidGameShouldUpdateGameQueuePointer) {
    GameManager manager = GameManager();
    Queue<InGameCommand*>* game_q1 = nullptr;
    Queue<GameState*> player_q1(10);
    std::uint8_t player_id1 = 0;

    std::uint32_t game_code = manager.createGame(game_q1, player_q1,
                                                 &player_id1);

    Queue<InGameCommand*>* game_q2 = nullptr;
    Queue<GameState*> player_q2(10);
    std::uint8_t player_id2 = 0;

    manager.joinGame(game_q2, player_q2, &player_id2,
                                    game_code);

    ASSERT_NE(game_q2, nullptr);
}

TEST(gamemanager_test,
     JoinTest03JoiningInvalidGameShouldNotUpdateGameQueuePointer) {
    GameManager manager = GameManager();
    Queue<InGameCommand*>* game_q1 = nullptr;
    Queue<GameState*> player_q1(10);
    std::uint8_t player_id1 = 0;

    std::uint32_t game_code = manager.createGame(game_q1, player_q1,
                                                 &player_id1);

    Queue<InGameCommand*>* game_q2 = nullptr;
    Queue<GameState*> player_q2(10);
    std::uint8_t player_id2 = 0;

    manager.joinGame(game_q2, player_q2, &player_id2,
                     game_code + 1);

    ASSERT_EQ(game_q2, nullptr);
}

TEST(gamemanager_test, JoinTest04JoiningValidGameShouldUpdatePlayer2ID) {
    GameManager manager = GameManager();
    Queue<InGameCommand*>* game_q1 = nullptr;
    Queue<GameState*> player_q1(10);
    std::uint8_t player_id1 = 0;

    std::uint32_t game_code = manager.createGame(game_q1, player_q1,
                                                 &player_id1);

    Queue<InGameCommand*>* game_q2 = nullptr;
    Queue<GameState*> player_q2(10);
    std::uint8_t player_id2 = 0;

    manager.joinGame(game_q2, player_q2, &player_id2,
                     game_code);

    ASSERT_NE(player_id2, 0);
}

TEST(gamemanager_test,
     JoinTest05AfterJoiningTheSameGameTheGamePointerOfPlayer1ShouldEqualThePointerOfPlayer2) {
    GameManager manager = GameManager();
    Queue<InGameCommand*>* game_q1 = nullptr;
    Queue<GameState*> player_q1(10);
    std::uint8_t player_id1 = 0;

    std::uint32_t game_code = manager.createGame(game_q1, player_q1,
                                                 &player_id1);

    Queue<InGameCommand*>* game_q2 = nullptr;
    Queue<GameState*> player_q2(10);
    std::uint8_t player_id2 = 0;

    manager.joinGame(game_q2, player_q2, &player_id2,
                     game_code);

    ASSERT_EQ(game_q2, game_q1);
}

TEST(gamemanager_test,
     JoinTest06AfterJoiningTheSameGameTheIDFromPlayer1ShouldNotEqualTheIDFromPlayer2) {
    GameManager manager = GameManager();
    Queue<InGameCommand*>* game_q1 = nullptr;
    Queue<GameState*> player_q1(10);
    std::uint8_t player_id1 = 0;

    std::uint32_t game_code = manager.createGame(game_q1, player_q1,
                                                 &player_id1);

    Queue<InGameCommand*>* game_q2 = nullptr;
    Queue<GameState*> player_q2(10);
    std::uint8_t player_id2 = 0;

    manager.joinGame(game_q2, player_q2, &player_id2,
                     game_code);

    ASSERT_NE(player_id1, player_id2);
}

TEST(gamemanager_test,
     JoinTest08TwoPlayersCreateGamesThenOtherTwoJoinsEachGameShouldHaveDifferentGameQueues) {
    using std::array;
    using std::vector;
    using std::uint8_t;
    using std::uint32_t;

    GameManager manager = GameManager();

    // Initialize four players.
    array<Queue<InGameCommand*>*, 4> cmd_queues{nullptr};
    array<Queue<GameState*>*, 4> player_queues{new Queue<GameState*>(10)};
    array<uint8_t, 4> player_ids{0};
    array<uint32_t, 2> game_codes{0};

    // Player 1 creates first game
    game_codes[GAME_1] = manager.createGame(cmd_queues[PLAYER_1],
                                       *player_queues[PLAYER_1],
                                       &player_ids[PLAYER_1]);

    // Player 3 creates second game
    game_codes[GAME_2] = manager.createGame(cmd_queues[PLAYER_3],
                                            *player_queues[PLAYER_3],
                                            &player_ids[PLAYER_3]);

    // Player 2 joins first game
    manager.joinGame(cmd_queues[PLAYER_2], *player_queues[PLAYER_2],
                              &player_ids[PLAYER_2], game_codes[GAME_1]);

    // Player 4 joins second game
    manager.joinGame(cmd_queues[PLAYER_4], *player_queues[PLAYER_4],
                     &player_ids[PLAYER_4], game_codes[GAME_2]);

    ASSERT_NE(cmd_queues[PLAYER_2], cmd_queues[PLAYER_4]);

    for (int i = 0; i < 4; i++) {
        delete player_queues[i];
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
