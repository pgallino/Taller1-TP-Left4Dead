// Copyright [2023] pgallino

#include <memory>
#include "../include/receiver.h"

#define SHUT_RDWR 2

Receiver::Receiver(Socket &&peer, GameManager& game_manager) :
    peer(std::move(peer)),
    protocol(peer),
    game_state_queue(5000),
    sender(peer, game_state_queue),
    game_manager(game_manager),
    is_running(true),
    keep_talking(true),
    joined(false),
    recv_action(nullptr) { }

void Receiver::run() { try {
    while (keep_talking && !joined) {
        // Excepcion si join falla (o nullptr). Excepcion: JoinFailed
        // Crear los comandos en una clase fuera del protocolo
        // Protocolo crea DTOs para evitar includes de todo el juego
        // Receiver puede pushear el command
        // Execute recibe siempre el Juego
        // Skippear lobby
        joined = true;
    }
    if (joined) {
        sender.start();
    }
    while (keep_talking) {
        recv_action = protocol.recvAction();
        if (recv_action == nullptr) {
            continue;
        }
        // Hacer el comando y pushearlo
        delete recv_action;
        recv_action = nullptr;
    }
    is_running = false;
    } catch (ClosedSocket& err) {
        // revisar catcheos
        is_running = false;
        keep_talking = false;
    }
}

void Receiver::stop() {
    keep_talking = false;
    peer.shutdown(SHUT_RDWR);
    peer.close();
    game_state_queue.close();
}

bool Receiver::isDead() const {
    return !is_running;
}

Receiver::~Receiver() {
    if (joined) {
        if (!sender.isDead()) {
            sender.stop();
        }
        sender.join();
    }
    // Just in case. Deleting nullptr has no effect.
    delete recv_action;

}
