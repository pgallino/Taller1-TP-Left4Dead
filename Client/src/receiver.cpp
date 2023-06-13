//
// Created by luan on 12/06/23.
//
#include "../include/receiver.h"

Receiver::Receiver(Queue<std::shared_ptr<Information>> &feedback_received,
                   GameSocket &socket) :
                   feedback_received(feedback_received),
                   protocol(socket),
                   is_running(true),
                   keep_receiving(true) {
}

void Receiver::run() {
    using std::cerr;
    using std::endl;

    try {
    while (keep_receiving) {
        feedback_received.push(std::move(protocol.recvFeedback()));
    }
    } catch (const ClosedSocket& err) {
        cerr << "In Receiver thread: " << err.what() << endl;
        is_running = false;
        keep_receiving = false;
    } catch (const std::exception& e) {
        cerr << "An exception was caught in Receiver thread: "
             << e.what() << endl;
    } catch (...) {
        cerr << "An unknown exception was caught in Receiver thread." << endl;
    }
}

void Receiver::stop() {
    keep_receiving = false;
    feedback_received.close();
}

bool Receiver::isDead() const {
    return !is_running;
}