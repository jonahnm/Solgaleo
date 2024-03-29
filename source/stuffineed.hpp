#pragma once
namespace sead {
    struct Color4f;
    class TextWriter {
        public:
        sead::Color4f *mColor;
    };
}
namespace agl {
    struct DrawContext;
}
namespace Lp
{
  namespace Sys {
      enum Key {
              KEY_A = 1 << 0,
              KEY_B = 1 << 1,
              KEY_ZL = 1 << 2,
              KEY_Y = 1 << 3,
              KEY_X = 1 << 4,
              KEY_ZR = 1 << 5,
              KEY_RSTICK = 1 << 6,
              KEY_LSTICK = 1 << 7,

              KEY_UNK1 = 1 << 8,
              KEY_MINUS = 1 << 9,
              KEY_PLUS = 1 << 10,
              KEY_PLUS_ALT = 1 << 11,
              KEY_MINUS_ALT = 1 << 12,
              KEY_L = 1 << 13,
              KEY_R = 1 << 14,
              KEY_UNK2 = 1 << 15,

              KEY_DUP = 1 << 16,
              KEY_DDOWN = 1 << 17,
              KEY_DLEFT = 1 << 18,
              KEY_DRIGHT = 1 << 19,
              KEY_LSTICK_UP = 1 << 20,
              KEY_LSTICK_DOWN = 1 << 21,
              KEY_LSTICK_LEFT = 1 << 22,
              KEY_LSTICK_RIGHT = 1 << 23,

              KEY_RSTICK_UP = 1 << 24,
              KEY_RSTICK_DOWN = 1 << 25,
              KEY_RSTICK_LEFT = 1 << 26,
              KEY_RSTICK_RIGHT = 1 << 27,
          };
      struct CtrlData
{
  Lp::Sys::Key mHoldButtons;
  uint mButtonTrigLengths[32];
  Lp::Sys::Key mNewButtonsPressed;
  Lp::Sys::Key mOldButtonsReleased;
  Lp::Sys::Key mRepeatButtons;
  //sead::Vector2 mLeftJoy;
  //sead::Vector2 mRightJoy;
};
      class Ctrl{
         public:
          __int32_t dword0;
          __int32_t dword4;
          __int64_t qword8;
          Lp::Sys::CtrlData data[2];
      };
  };
};
namespace Cmn {
    struct PlayerWeapon;
}
namespace Game {
    struct PlayerMgr;
    struct Player;
}