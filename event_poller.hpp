#ifndef EVENT_POLLER_HPP
#define EVENT_POLLER_HPP

#include <thread>
#include <mutex>
#include <atomic>
#include "SDL.h"
#include "controller.hpp"

class EventPoller {
public:
    EventPoller(Controller& controller);
    ~EventPoller();

    void start();
    void stop();
    bool should_quit() const;

private:
    void poll_loop();
    void handle_key_event(const SDL_KeyboardEvent& key_event, bool is_pressed);

    Controller& controller_;
    std::thread poller_thread_;
    std::atomic<bool> quit_flag_;
    std::atomic<bool> running_flag_;
};

#endif // EVENT_POLLER_HPP
