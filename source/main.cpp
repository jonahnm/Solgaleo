#include "main.hpp"

#include <assert.h>

#include "imgui/imgui.cpp"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_sw.cpp"
#include "imgui/imgui_sw.hpp"
#include "imgui/imgui_tables.cpp"
#include "imgui/imgui_widgets.cpp"
#include "macros.h"
#include "skyline/logger/TcpLogger.hpp"
#include "skyline/utils/ipc.hpp"
#include "stuffineed.hpp"
#define PATCH(off, data, type)                                                                                   \
    {                                                                                                            \
        skyline::inlinehook::ControlledPages pages((void*)(skyline::utils::g_MainTextAddr + off), sizeof(type)); \
        pages.claim();                                                                                           \
        *(type*)pages.rw = (type)data;                                                                           \
        pages.unclaim();                                                                                         \
    }
#define PATCH_U32(off, data) PATCH(off, data, u32)
#define PATCH_INST(off, data) PATCH_U32(off, __builtin_bswap32(data))
// For handling exceptions
char ALIGNA(0x1000) exception_handler_stack[0x4000];
static __int64 lastInputs = 0x200;
nn::os::UserExceptionInfo exception_info;
sead::TextWriter* mTextWriter;
agl::DrawContext* mDrawContext;
bool showAprompt = true;
Lp::Sys::Ctrl* mController;
bool showrootmenu = false;
bool showdestructive = false;
bool oppositeink = false;
Game::PlayerMgr* playermgr;
Game::Player* player;
ImGuiIO io;
bool showfun = false;
float wideness = 1.0;
bool showwideness = false;
bool alreadychangedwidenesstothis = false;
bool godmode_ = false;
bool showmaster = false;
bool showteamselect = false;
int colorteam;
bool showcolors = false;
bool shouldmodifysetteamcolor = false;
bool rendermenu = true;
sead::Color4f* teamcolor;
using ismastertype = bool();
using setexternalscaletype = void(Game::Player*, float);
using getcontrolledperformertype = Game::Player*(Game::PlayerMgr*);
using textwritergraphicssetuptype = void(agl::DrawContext*);
using scenenametype = const char*();
using getctrltype = Lp::Sys::Ctrl*(int);
using textwriterprintftypeUTF = void(sead::TextWriter*, const char16_t*);
getctrltype* getctrl;
setexternalscaletype* setexternalscale;
getcontrolledperformertype* getcontrolledperformer;
textwritergraphicssetuptype* textwritergraphicssetup;
using textwriterprintftype = void(sead::TextWriter*, const char*);
textwriterprintftype* textwriterprintf;
scenenametype* getcurscenename;
ismastertype* ismaster;
void exception_handler(nn::os::UserExceptionInfo* info) {}

void stub() {}

bool isTriggered(Lp::Sys::Ctrl* controller, unsigned long const& id) {
    bool buttonHeld = controller->data[0].mNewButtonsPressed & id;
    return buttonHeld & !(controller->data[0].mNewButtonsPressed & lastInputs & id);
}
/*void mastershow() {
    if(ismaster()) {
        textwriterprintf(mTextWriter,"Press B to open the Master Menu.\n");
    }
}*/
/*
void menu() {
  if (showrootmenu) {
    textwriterprintf(mTextWriter, "Press X to open Destructive Stuff.\n");
    textwriterprintf(mTextWriter, "Press Y to open Fun Stuff.\n");
    textwriterprintf(mTextWriter,
                     "Press R to open the Master Menu. THIS ONLY WORKS WHEN "
                     "YOU ARE THE MASTER OF THE MATCH.\n");
    textwriterprintf(mTextWriter, "Press L to close the Menu.\n");
    if (isTriggered(mController, Lp::Sys::Key::KEY_X)) {
      showrootmenu = false;
      showdestructive = true;
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_L)) {
      showrootmenu = false;
      showAprompt = true;
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_Y)) {
      showrootmenu = false;
      showfun = true;
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_R)) {
      showrootmenu = false;
      showmaster = true;
    }
  } else if (showdestructive) {
    textwriterprintf(mTextWriter, "Press D-Pad Down to start Water.\n");
    textwriterprintf(mTextWriter, "Press L to go back to root menu.\n");
    if (isTriggered(mController, Lp::Sys::Key::KEY_DDOWN)) {
      showdestructive = false;
      oppositeink = true;
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_L)) {
      showdestructive = false;
      showrootmenu = true;
    }
  } else if (oppositeink) {
    textwriterprintf(mTextWriter, "Water is enabled. Press L to stop.\n");
    if (isTriggered(mController, Lp::Sys::Key::KEY_L)) {
      showdestructive = true;
      oppositeink = false;
    }
  } else if (showfun) {
    textwriterprintf(mTextWriter, "Press D-Pad Down to become WIDE\n");
    textwriterprintf(mTextWriter, "Press L to go back to the root menu.\n");
    if (isTriggered(mController, Lp::Sys::Key::KEY_L)) {
      showfun = false;
      showrootmenu = true;
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_DDOWN)) {
      showfun = false;
      showwideness = true;
    }
  } else if (showwideness) {
    textwriterprintf(mTextWriter,
                     "Press D-Pad Up to Increase in wideness by .1\n");
    textwriterprintf(mTextWriter,
                     "Press D-Pad Down to Decrease wideness by .1\n");
    textwriterprintf(mTextWriter,
                     "Press L to go back and reset all changes.\n");
    if (isTriggered(mController, Lp::Sys::Key::KEY_DUP)) {
      wideness = wideness + 0.1;
      alreadychangedwidenesstothis = false;
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_DDOWN)) {
      wideness = wideness - 0.1;
      alreadychangedwidenesstothis = false;
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_L)) {
      wideness = 1;
      showwideness = false;
      showfun = true;
      alreadychangedwidenesstothis = false;
    }
    if (player != NULL && !alreadychangedwidenesstothis) {
      setexternalscale(player, wideness);
      alreadychangedwidenesstothis = true;
    }
  } else if (showmaster) {
    textwriterprintf(mTextWriter,
                     "You are currently master of this lobby. You have alot of "
                     "power in this state. Please do NOT abuse it.\n");
    textwriterprintf(mTextWriter, "Press D-Pad Down to select team colors.\n");
    textwriterprintf(mTextWriter, "Press L to go back.\n");
    if (isTriggered(mController, Lp::Sys::Key::KEY_L)) {
      showmaster = false;
      showrootmenu = true;
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_DDOWN)) {
      showmaster = false;
      showteamselect = true;
    }
  } else if (showteamselect) {
    textwriterprintf(mTextWriter, "Press A To Choose Team Alpha's Colors.\n");
    textwriterprintf(mTextWriter, "Press B to Select Team Bravo's Color.\n");
    textwriterprintf(mTextWriter, "Press L to go back.");
    if (isTriggered(mController, Lp::Sys::Key::KEY_A)) {
      showteamselect = false;
      colorteam = 1;
      showcolors = true;
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_B)) {
      showteamselect = false;
      colorteam = 2;
      showcolors = true;
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_L)) {
      showteamselect = false;
      colorteam = (int)nullptr;
    }
  } else if (showcolors) {
    textwriterprintf(mTextWriter, "Press B for Black.\n");
    textwriterprintf(mTextWriter, "Press L to go back.\n");
    if (isTriggered(mController, Lp::Sys::Key::KEY_B)) {
      shouldmodifysetteamcolor = true;
      teamcolor =
          *(sead::Color4f **)(skyline::utils::g_MainTextAddr + 0x2514C6C);
    } else if (isTriggered(mController, Lp::Sys::Key::KEY_L)) {
      showcolors = false;
      showteamselect = true;
    }
  }
}
void do_render_stuff(InlineCtx *registers) {
  playermgr = *(Game::PlayerMgr **)(skyline::utils::g_MainTextAddr +
                                    0x2E41B20); // grab playermgr
  if (playermgr != NULL) {
    player =
        getcontrolledperformer(playermgr); // grab current player instance using
                                           // recently grabbed playermgr
  }
  mTextWriter = (sead::TextWriter *)registers->registers[0]
                    .x; // grab TextWriter from register X0
  mDrawContext = (agl::DrawContext *)registers->registers[25]
                     .x; // grab DrawContext from register X25
  mController =
      getctrl(0); // grab the player's controller for input functionality.
  textwritergraphicssetup(mDrawContext);
  sead::Color4f *color =
      *(sead::Color4f **)(skyline::utils::g_MainTextAddr + 0x2514C9C);
  if (color != NULL) {
    mTextWriter->mColor = color;
  }
  if (getcurscenename() != NULL && strncmp(getcurscenename(), "Boot", 4) == 0) {
    textwriterprintf(mTextWriter,
                     "Credits to the skyline dev team for skyline.\n");
    textwriterprintf(
        mTextWriter,
        "Solgaleo Initialized. Thank you for checking this out.\n");
  } else if (showAprompt) {
    textwriterprintf(mTextWriter, "Press L to open the menu.\n");
    if (isTriggered(mController, Lp::Sys::Key::KEY_L)) {
      showAprompt = false;
      showrootmenu = true;
    }
  } else {
    menu();
  }
}
*/
/*
void (*rendertramp)(uintptr_t a1);
void render(uintptr_t a1) {
  rendertramp(a1);
  int width_pixels = 500;
    int height_pixels = 500;
    imgui_sw::SwOptions sw_options;
    std::vector<uint32_t> pixel_buffer(width_pixels * height_pixels, 0);
        uintptr_t* checknull = *(uintptr_t**)(skyline::utils::g_MainTextAddr + 0x2e96cb0);
        memset(io.NavInputs, 0, sizeof(io.NavInputs));
        if (checknull != NULL) {
            mController = getctrl(0);
#define MAP_BUTTON(NAV, BUTTON)                                         \
    {                                                                   \
        if (isTriggered(mController, BUTTON)) io.NavInputs[NAV] = 1.0f; \
    }
            MAP_BUTTON(ImGuiNavInput_Activate, Lp::Sys::Key::KEY_A);
            MAP_BUTTON(ImGuiNavInput_Cancel, Lp::Sys::Key::KEY_B);
#undef MAP_BUTTON
            io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
            io.DeltaTime = 1.0f / 60.0f;
            ImGui::NewFrame();
            if (ImGui::Begin("Solgaleo")) {
                // all imgui rendering here
                ImGui::Text("Credits To the Skyline Dev team for this.");
            } else {
                ImGui::ShowDemoWindow(NULL);
            }
            ImGui::End();
            // actually render lol
            ImGui::Render();
            imgui_sw::paint_imgui(pixel_buffer.data(), width_pixels, height_pixels, sw_options);
        }
}
*/
void (*erasethingtramp)(uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6,
                        uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11, uintptr_t a12,
                        uintptr_t a13, uintptr_t a14, uintptr_t a15, uintptr_t a16);
void erasething(uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7,
                uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11, uintptr_t a12, uintptr_t a13, uintptr_t a14,
                uintptr_t a15, uintptr_t a16) {
    if (oppositeink) {
        erasethingtramp(3, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16);
    } else {
        erasethingtramp(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16);
    }
}
void (*ACBYPASSTRAMP)(uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6);
void ACBYPASS(uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6) {
    if (a3 == skyline::utils::g_MainTextAddr) {
        auto b = (skyline::utils::g_MainTextAddr + 0x2EBE000);
        ACBYPASSTRAMP(a1, a2, b, a4, a5, a6);
    } else {
        if (a3 == skyline::utils::g_MainTextAddr + 0x02415A00) {
            auto c = ((skyline::utils::g_MainTextAddr + 0x2EBE000) + 0x02415A00);
            ACBYPASSTRAMP(a1, a2, c, a4, a5, a6);
        } else {
            ACBYPASSTRAMP(a1, a2, a3, a4, a5, a6);
        }
    }
}
bool (*godmodetramp)(Game::Player*);
bool godmode(Game::Player* a1) {
    if (godmode_) { return true; }
    else {
        godmodetramp(a1);
    }
}
void (*setteamcolortramp)(uintptr_t a1, uintptr_t a2, sead::Color4f* a3);
void setteamcolor(uintptr_t a1, uintptr_t a2, sead::Color4f* a3) {
    if (shouldmodifysetteamcolor) {
        setteamcolortramp(a1, colorteam, teamcolor);
    } else {
        setteamcolortramp(a1, a2, a3);
    }
}
void skyline_main() {
    ismaster = (ismastertype*)(skyline::utils::g_MainTextAddr + 0x75ffe4);
    setexternalscale = (setexternalscaletype*)(skyline::utils::g_MainTextAddr + 0xff1b10);
    getcontrolledperformer = (getcontrolledperformertype*)(skyline::utils::g_MainTextAddr + 0x10E6D2C);
    textwriterprintf = (textwriterprintftype*)(skyline::utils::g_MainTextAddr + 0x174A19C);
    getcurscenename = (scenenametype*)(skyline::utils::g_MainTextAddr + 0x19F4100);
    getctrl = (getctrltype*)(skyline::utils::g_MainTextAddr + 0x19EC678);
    textwritergraphicssetup = (textwritergraphicssetuptype*)(skyline::utils::g_MainTextAddr + 0x174A0EC);
    // populate our own process handle
    Handle h;
    skyline::utils::Ipc::getOwnProcessHandle(&h);
    envSetOwnProcessHandle(h);
    // setup Dear ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    imgui_sw::bind_imgui_painting();
    ImGui::StyleColorsDark();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    // init hooking setup
    A64HookInit();

    // override exception handler to dump info
    nn::os::SetUserExceptionHandler(exception_handler, exception_handler_stack, sizeof(exception_handler_stack),
                                    &exception_info);

    // manually init nn::ro ourselves, then stub it so the game doesn't try again
    nn::ro::Initialize();
    A64HookFunction(reinterpret_cast<void*>(nn::ro::Initialize), reinterpret_cast<void*>(stub), NULL);
    // inline hook to render stuff.
    uintptr_t renderstuff = 0x185121C + skyline::utils::g_MainTextAddr;
    // PATCH_INST(0x185121C + 0x284, 0x1F2003D5);
    // PATCH_INST(0x185121C + 0x394, 0xB0000014);
    A64HookFunction((void*)(skyline::utils::g_MainTextAddr + 0x37f198), (void*)setteamcolor,
                    (void**)&setteamcolortramp);
    A64HookFunction((void*)(skyline::utils::g_MainTextAddr + 0x1010b58), (void*)godmode, (void**)&godmodetramp);
    A64HookFunction((void*)(skyline::utils::g_MainTextAddr + 0x6e480), (void*)ACBYPASS, (void**)&ACBYPASSTRAMP);
    A64HookFunction(reinterpret_cast<void*>(skyline::utils::g_MainTextAddr + 0xfc1bac),
                    reinterpret_cast<void*>(erasething), (void**)&erasethingtramp);
    // A64HookFunction((void*)renderstuff,(void*)render,(void**)&rendertramp);
    skyline::utils::SafeTaskQueue* taskQueue = new skyline::utils::SafeTaskQueue(100);
    taskQueue->startThread(20, 3, 0x4000);
    // TODO: experiment more with NVN
    /*nvnInit(NULL);

    NVNdeviceBuilder deviceBuilder;
    nvnDeviceBuilderSetDefaults(&deviceBuilder);
    nvnDeviceBuilderSetFlags(&deviceBuilder, 0);

    NVNdevice device;
    nvnDeviceInitialize(&device, &deviceBuilder);

    nvnInit(&device); // re-init with our newly acquired device
    */
}

extern "C" void skyline_init() {
    skyline::utils::init();
    virtmemSetup();  // needed for libnx JIT

    skyline_main();
}
