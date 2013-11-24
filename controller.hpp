#ifndef NES_CONTROLLER_HPP
#define NES_CONTROLLER_HPP

#include <iostream>

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
        for ( int i = 0; i < 2; i++ ) {
            for ( int j = 0; j < 8; j++ ) {
                pressed_[i][j] = false;
            }
        }
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
        return r;
    }

    void print( std::ostream& ostr ) const
    {
        ostr << "K1 A:" << pressed_[0][AButton] << " B:" << pressed_[0][BButton]
             << " Select:" << pressed_[0][SelectButton] << " Start:" << pressed_[0][StartButton]
             << " Left:" << pressed_[0][LeftButton] << " Right:" << pressed_[0][RightButton]
             << " Up:" << pressed_[0][UpButton] << " Down:" << pressed_[0][DownButton]
             << " idx: " << idx_[0]
             << std::endl;
        ostr << "K2 A:" << pressed_[1][AButton] << " B:" << pressed_[1][BButton]
             << " Select:" << pressed_[1][SelectButton] << " Start:" << pressed_[1][StartButton]
             << " Left:" << pressed_[1][LeftButton] << " Right:" << pressed_[1][RightButton]
             << " Up:" << pressed_[1][UpButton] << " Down:" << pressed_[1][DownButton]
             << " idx: " << idx_[1]
             << std::endl;
        ostr << "Strobe: " << (strobe_ ? "1" : "0") << std::endl;
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
