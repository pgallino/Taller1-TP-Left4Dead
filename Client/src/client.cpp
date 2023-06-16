#include <exception>
#include <SDL2pp/SDL2pp.hh>
#include "yaml-cpp/yaml.h"
#include "../include/client.h"
#include "../../Common/include/Information/Actions/shoot_start.h"
#include "../../Common/include/Information/Actions/game_join.h"
#include "../../Common/include/Information/Actions/game_create.h"
#include "../../Common/include/Information/information_code.h"
#include "../include/Animations/animation_manager.h"
#include "../include/Drawer/drawer_manager.h"
#include "../include/visual_game.h"
#include "../../Common/include/Information/Actions/moving_right_start.h"
#include "../../Common/include/Information/Actions/moving_left_start.h"
#include "../../Common/include/Information/Actions/moving_right_stop.h"
#include "../../Common/include/Information/Actions/moving_left_stop.h"


Client::Client(const char *hostname, const char *servname) :
    socket(hostname, servname) ,
    protocol(socket),
    actions_to_send(5000),
    feedback_received(10000),
    sender(actions_to_send, socket),
    receiver(feedback_received, socket) {
}

void Client::processEvent(std::uint32_t event_type, int key_code, bool *quit)
{
    using std::make_shared;
    using std::shared_ptr;

    if (event_type == SDL_QUIT) {
        *quit = true;
    }
    else if (event_type == SDL_KEYDOWN)
    {
        // process key down method
        switch (key_code)
        {
        case SDLK_ESCAPE:
            *quit = true;
            break;

        case SDLK_z:
            actions_to_send.push(
                    make_shared<StartShootAction>());
            break;

        case SDLK_RIGHT:
            actions_to_send.push(
                    make_shared<StartMovingRightAction>());
            break;

        case SDLK_LEFT:
            actions_to_send.push(
                    make_shared<StartMovingLeftAction>());
            break;

        default:
            break;
        }
    }
    else if (event_type == SDL_KEYUP) {
        //process key up method
        switch (key_code)
        {
        /*
        case SDLK_z:
            actions_to_send.push(
                    make_shared<StopShootAction>());
            break;
        */

        case SDLK_RIGHT:
            actions_to_send.push(
                    make_shared<StopMovingRightAction>());
            break;

        case SDLK_LEFT:
            actions_to_send.push(
                    make_shared<StopMovingLeftAction>());
            break;

        default:
            break;
        }
    }
}

void Client::lobbyProcess() {
    using std::make_shared;
    using std::shared_ptr;

    bool joined = false;
    std::cout << "Waiting input. Use 'create' or 'join <code>' to join a "
                 "game."
              << std::endl;
    while (!joined)
    {
        std::string action;
        std::cin >> action;
        if (action == "join")
        {
            std::uint32_t game_code;
            std::cin >> game_code;
            actions_to_send.push(
                    make_shared<JoinGameAction>(game_code));
            joined = true;
        }
        else if (action == "create")
        {
            actions_to_send.push(make_shared<CreateGameAction>());
            const std::shared_ptr<Information>& create_feed =
                    feedback_received.pop();

            if (create_feed == nullptr)
            {
                throw std::runtime_error("Client::lobbyProcess. "
                                         "CreateFeedback is null.\n");
            }
            std::cout <<
            dynamic_cast<CreateGameFeedback *>(create_feed.get())->game_code
                      << std::endl;
            joined = true;
        }
    }
}

void Client::gameProcess() {

    using YAML::LoadFile;
    using YAML::Node;

    using std::make_shared;
    using std::shared_ptr;

    // Init game with config
    Node window_config = LoadFile(CLIENT_CONFIG_PATH "/config.yaml")["window"];

    const auto window_width =
            window_config["width"].as<std::uint16_t>();
    const auto window_height =
            window_config["height"].as<std::uint16_t>();

    GameVisual game_visual(window_width, window_height);

    std::shared_ptr<Information> information_ptr = nullptr;

    // Game loop
    bool quit = false;
    while (!quit)
    {
        unsigned int frame_ticks = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            processEvent(event.type, event.key.keysym.sym, &quit);
        }

        game_visual.clear();

        feedback_received.try_pop(information_ptr);
        if (information_ptr != nullptr) {
            game_visual.updateInfo(dynamic_cast<GameStateFeedback&>
                                   (*information_ptr));
        }

        game_visual.draw(frame_ticks);

        game_visual.present();

        SDL_Delay(1);
    }
}

void Client::start() {

    sender.start();
    receiver.start();

    lobbyProcess();
    gameProcess();
}

Client::~Client() {

    if (!receiver.isDead()) {
        receiver.stop();
    }
    if(!sender.isDead()) {
        sender.stop();
    }
    receiver.join();
    sender.join();
}
