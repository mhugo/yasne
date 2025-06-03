#include "event_poller.hpp"
#include <iostream> // For potential debugging

EventPoller::EventPoller(Controller& controller)
    : controller_(controller), quit_flag_(false), running_flag_(false) {}

EventPoller::~EventPoller() {
    stop();
}

void EventPoller::start() {
    if (running_flag_.load()) return;

    running_flag_ = true;
    quit_flag_ = false;
    poller_thread_ = std::thread(&EventPoller::poll_loop, this);
}

void EventPoller::stop() {
    if (!running_flag_.load()) return;

    running_flag_ = false;
    if (poller_thread_.joinable()) {
        poller_thread_.join();
    }
}

bool EventPoller::should_quit() const {
    return quit_flag_.load();
}

void EventPoller::handle_key_event(const SDL_KeyboardEvent& key_event, bool is_pressed) {
    SDL_Keycode sym = key_event.keysym.sym;

    // Mapping for controller 0, based on main.cpp
    if (sym == SDLK_RETURN) {
        controller_.setState(0, Controller::StartButton, is_pressed);
    } else if (sym == SDLK_SPACE) {
        controller_.setState(0, Controller::SelectButton, is_pressed);
    } else if (sym == SDLK_RIGHT) {
        controller_.setState(0, Controller::RightButton, is_pressed);
    } else if (sym == SDLK_LEFT) {
        controller_.setState(0, Controller::LeftButton, is_pressed);
    } else if (sym == SDLK_UP) {
        controller_.setState(0, Controller::UpButton, is_pressed);
    } else if (sym == SDLK_DOWN) {
        controller_.setState(0, Controller::DownButton, is_pressed);
    } else if (sym == SDLK_a) {
        controller_.setState(0, Controller::AButton, is_pressed);
    } else if (sym == SDLK_b) {
        controller_.setState(0, Controller::BButton, is_pressed);
    }
    // Note: SDLK_ESCAPE for quitting is handled by SDL_QUIT event.
    // The 'd' key for debugger is not handled here as it's not direct controller input.
}

void EventPoller::poll_loop() {
    SDL_Event e;
    while (running_flag_.load()) {
        // SDL_PumpEvents(); // Not strictly necessary with SDL_WaitEventTimeout, but can be added if issues arise.
        if (SDL_WaitEventTimeout(&e, 100)) { // Wait up to 100ms for an event
            if (e.type == SDL_QUIT) {
                quit_flag_ = true;
                running_flag_ = false;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) { // Treat ESC keydown as quit
                quit_flag_ = true;
                running_flag_ = false;
            }
            else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                handle_key_event(e.key, e.type == SDL_KEYDOWN);
            }
        }
    }
}
