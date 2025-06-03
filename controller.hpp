#ifndef NES_CONTROLLER_HPP
#define NES_CONTROLLER_HPP

#include <iostream>
#include <mutex>
#include <atomic> // For std::atomic<bool>

///
/// Game controller
class Controller
{
 public:
    static const int AButton = 0;
    static const int BButton = 1;
    static const int SelectButton = 2;
    static const int StartButton = 3;
    static const int UpButton = 4;
    static const int DownButton = 5;
    static const int LeftButton = 6;
    static const int RightButton = 7;

    Controller() : current_is_a_(true), strobe_(false) {
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 8; ++j) {
                pressed_a_[i][j] = false;
                pressed_b_[i][j] = false;
            }
            idx_[i] = -1;
        }
    }
    ~Controller() {}

    void setState( int controller, int button, bool pressed )
    {
        std::lock_guard<std::mutex> lock(write_side_mutex_);

        bool is_currently_reading_a = current_is_a_.load(std::memory_order_relaxed);

        if (is_currently_reading_a) {
            // Readers are using 'a'. We prepare 'b' as the next buffer.
            // Copy current full state from 'a' to 'b'.
            for (int c = 0; c < 2; ++c) {
                for (int b = 0; b < 8; ++b) {
                    pressed_b_[c][b] = pressed_a_[c][b];
                }
            }
            // Apply the new change to 'b'.
            pressed_b_[controller][button] = pressed;
            current_is_a_.store(false, std::memory_order_release); // Flip: 'b' is now for reading.
        } else {
            // Readers are using 'b'. We prepare 'a' as the next buffer.
            // Copy current full state from 'b' to 'a'.
            for (int c = 0; c < 2; ++c) {
                for (int b = 0; b < 8; ++b) {
                    pressed_a_[c][b] = pressed_b_[c][b];
                }
            }
            // Apply the new change to 'a'.
            pressed_a_[controller][button] = pressed;
            current_is_a_.store(true, std::memory_order_release); // Flip: 'a' is now for reading.
        }
    }

    void setStrobe( bool state ) {
        strobe_ = state;
        if ( state ) {
            idx_[0] = 0;
            idx_[1] = 0;
        }
    }

    bool readPressed( int controller ) {
        // Atomically determine which buffer is for reading
        bool use_a = current_is_a_.load(std::memory_order_acquire);

        if (idx_[controller] == -1) {
            return true;
        }

        bool r;
        if (use_a) {
            r = pressed_a_[controller][idx_[controller]];
        } else {
            r = pressed_b_[controller][idx_[controller]];
        }

        if (!strobe_) {
            idx_[controller]++;
        }

        if (idx_[controller] > 7) {
            idx_[controller] = -1;
        }
        return r;
    }

    void print( std::ostream& ostr ) const
    {
        bool use_a = current_is_a_.load(std::memory_order_acquire);
        const bool (*active_pressed_buffer)[8]; // Pointer to an array of 8 booleans

        if (use_a) {
            active_pressed_buffer = pressed_a_;
        } else {
            active_pressed_buffer = pressed_b_;
        }

        ostr << "K1 A:" << active_pressed_buffer[0][AButton] << " B:" << active_pressed_buffer[0][BButton]
             << " Select:" << active_pressed_buffer[0][SelectButton] << " Start:" << active_pressed_buffer[0][StartButton]
             << " Left:" << active_pressed_buffer[0][LeftButton] << " Right:" << active_pressed_buffer[0][RightButton]
             << " Up:" << active_pressed_buffer[0][UpButton] << " Down:" << active_pressed_buffer[0][DownButton]
             << " idx: " << idx_[0] // idx_ read directly
             << std::endl;
        ostr << "K2 A:" << active_pressed_buffer[1][AButton] << " B:" << active_pressed_buffer[1][BButton]
             << " Select:" << active_pressed_buffer[1][SelectButton] << " Start:" << active_pressed_buffer[1][StartButton]
             << " Left:" << active_pressed_buffer[1][LeftButton] << " Right:" << active_pressed_buffer[1][RightButton]
             << " Up:" << active_pressed_buffer[1][UpButton] << " Down:" << active_pressed_buffer[1][DownButton]
             << " idx: " << idx_[1] // idx_ read directly
             << std::endl;
        ostr << "Strobe: " << (strobe_ ? "1" : "0") << std::endl; // strobe_ read directly
    }

 private:
    // Double buffer for button states
    bool pressed_a_[2][8];
    bool pressed_b_[2][8];

    // Atomic flag to indicate which buffer is currently active for reading
    // true: pressed_a_ is active for reading, pressed_b_ for writing
    // false: pressed_b_ is active for reading, pressed_a_ for writing
    std::atomic<bool> current_is_a_;

    // Mutex to protect write operations (setState and buffer swap)
    std::mutex write_side_mutex_;

    // strobe_ and idx_ remain as they are primarily reader-managed
    bool strobe_;
    int idx_[2];
};

#endif
