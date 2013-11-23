#ifndef NES_CONTROLLER_HPP
#define NES_CONTROLLER_HPP

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

    Controller()
    {
        idx_[0] = -1;
        idx_[1] = -1;
    }
    ~Controller() {}

    void setState( int controller, int button, bool pressed )
    {
        pressed_[ controller ][ button ] = pressed;
    }

    void setStrobe( bool state ) {
        strobe_ = state;
        if ( state ) {
            idx_[0] = 0;
            idx_[1] = 0;
        }
    }

    bool readPressed( int controller ) {
        if ( idx_[controller] == -1 ) {
            return true; // 1
        }
        bool r = pressed_[controller][idx_[controller]];
        if ( ! strobe_ ) {
            idx_[controller]++;
        }
        if ( idx_[controller] > 7 ) {
            idx_[controller] = -1;
        }
    }

 private:
    // state of controllers
    bool pressed_[2][8];

    // strobe state (0x4016)
    // *while* high (true), the state of the first button is reloaded
    // when low, the *re*loading stops. A sequence of true then false
    // allows to read all button states
    bool strobe_;

    // which button is being read (for each controller)
    // -1 if none
    int idx_[2];
};

#endif
