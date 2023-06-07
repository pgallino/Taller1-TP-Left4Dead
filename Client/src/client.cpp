#include <exception>
#include <SDL2pp/SDL2pp.hh>
#include "yaml-cpp/yaml.h"
#include "../include/client.h"
#include "../../Common/include/Information/action_startshoot.h"
#include "../../Common/include/Information/action_joingame.h"
#include "../../Common/include/Information/action_creategame.h"
#include "../../Common/include/Information/state_dto_element.h"
#include "../../Common/include/Information/information_code.h"
#include "../../Common/include/Information/feedback_server_creategame.h"
#include "../include/Animations/animation_manager.h"
#include "../include/Drawer/drawer_actor.h"

Client::Client(const char *hostname, const char *servname) :
    socket(hostname, servname) ,
    protocol(socket) {
}

void Client::processEvent(std::uint32_t event_type, int key_code, bool *quit) {
    if (event_type == SDL_KEYDOWN) {
        switch (key_code) {
            case SDLK_ESCAPE:
                *quit = true;
                break;

            case SDLK_z:
                protocol.sendAction(StartShootAction());
                break;

            default:
                break;
        }
    }
}

// Crear clases: texture manager q tenga dicc mapa etc para mapear. No crear
// la textura en cada game loop.
// jugadores, animaciones, mapa
// Es responsabilidad del cliente transformar lo q reciba a pixeles. No va a
// recibir pixeles directamente
// Poder cambiar parametros in game para probar
void Client::start() {
    using SDL2pp::SDL;
    using SDL2pp::Window;
    using SDL2pp::Renderer;
    using SDL2pp::Texture;
    using SDL2pp::Rect;
    using SDL2pp::NullOpt;

    using YAML::Node;
    using YAML::LoadFile;

    bool joined = false;
    std::cout << "Waiting input. Use 'create' or 'join <code>' to join a "
                 "game." << std::endl;
    while (!joined) {
        std::string action;
        std::cin >> action;
        if (action == "join") {
            std::uint32_t game_code;
            std::cin >> game_code;
            protocol.sendAction(JoinGameAction(game_code));
            joined = true;
        } else if (action == "create") {
            protocol.sendAction(CreateGameAction());
            std::unique_ptr<Information> create_feed(
                    protocol.recvFeedback());
            if (create_feed == nullptr) {
                throw std::runtime_error("Client::start. CreateFeedback is "
                                         "null.\n");
            }
            std::cout << dynamic_cast<CreateGameFeedback*>(create_feed.get())
            ->game_code << std::endl;
            joined = true;
        }
    }

    SDL sdl(SDL_INIT_VIDEO);

    Node window_config = LoadFile(CLIENT_CONFIG_PATH "/config.yaml")["window"];

    const auto window_width =
            window_config["width"].as<std::uint16_t>();
    const auto window_height =
            window_config["height"].as<std::uint16_t>();

    Window window("Game", SDL_WINDOWPOS_UNDEFINED,
                  SDL_WINDOWPOS_UNDEFINED, window_width,
                  window_height,SDL_WINDOW_RESIZABLE);

    Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

    AnimationManager animation_manager(renderer);

    ActorDrawer player(animation_manager);

    // Big part of this logic has to be done by the Server
    double position = 0.0;
    bool is_running_right = false;
    bool is_running_left = false;
    bool last_input_right = false;
    bool last_input_left = false;
    unsigned int prev_ticks = SDL_GetTicks();
    std::uint8_t direction = DrawDirection::DRAW_RIGHT;

    bool quit = false;
    while(!quit) {
        unsigned int frame_ticks = SDL_GetTicks();
        unsigned int frame_delta = frame_ticks - prev_ticks;
        prev_ticks = frame_ticks;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            processEvent(event.type, event.key.keysym.sym, &quit);
            if (event.type == SDL_QUIT) {
                quit = true;

            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_RIGHT:
                        is_running_right = true;
                        last_input_right = true;
                        last_input_left = false;
                        break;

                    case SDLK_LEFT:
                        is_running_left = true;
                        last_input_right = false;
                        last_input_left = true;
                        break;
                }
                // Parseamos y definimos valores
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_RIGHT:
                        is_running_right = false;
                        break;

                    case SDLK_LEFT:
                        is_running_left = false;
                        break;
                }
            }
        }
        if (is_running_right && is_running_left) {
            if (last_input_right) {
                direction = DrawDirection::DRAW_RIGHT;
                position += frame_delta * 0.2;
            } else if (last_input_left) {
                direction = DrawDirection::DRAW_LEFT;
                position -= frame_delta * 0.2;
            }
        } else if (is_running_right) {
            direction = DrawDirection::DRAW_RIGHT;
            position += frame_delta * 0.2;
        } else if (is_running_left) {
            direction = DrawDirection::DRAW_LEFT;
            position -= frame_delta * 0.2;
        }

        int src_width = 128;
        int src_height = 128;

        if (position > renderer.GetOutputWidth())
            position = -src_width;

        if (position < -src_width)
            position = renderer.GetOutputWidth();

        int vcenter = renderer.GetOutputHeight() / 2;

        renderer.Clear();

        uint8_t type = ElementType::SOLDIER_1;
        uint8_t action = SoldierOneActionID::SOLDIER_1_IDLE;
        if (is_running_right || is_running_left) {
            action = SoldierOneActionID::SOLDIER_1_RUN;
        }
        ElementStateDTO player_state = {type, action,
                                        direction,
                                        static_cast<int>(position),
                                        vcenter - src_height};
        player.updateInfo(player_state);
        player.draw(frame_ticks);
        
        renderer.Present();

        SDL_Delay(1);
    }
}