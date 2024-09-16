#include <windows.h>
#include <iostream>
#include <sstream>
#include <d3d9.h>
#include <d3dx9.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include "driver.h"
#include <Uxtheme.h>
#include <dwmapi.h>
#include "Classes.h"
#include <iomanip>
#include <mutex>
#include "blur.hpp"
#include "skStr.h"
#include "auth.hpp"
#include <filesystem>
#include <random>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

using namespace std;

float color_edit4[4] = { 1.00f, 1.00f, 1.00f, 1.000f };
float accent_color[4] = { Features::AccentColor.x, Features::AccentColor.y, Features::AccentColor.z, Features::AccentColor.w };

IDirect3DTexture9* scene = nullptr;
ImFont* ico = nullptr;
ImFont* ico_combo = nullptr;
ImFont* ico_button = nullptr;
ImFont* ico_grande = nullptr;
ImFont* segu = nullptr;
ImFont* default_segu = nullptr;
ImFont* bold_segu = nullptr;
IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };

HWND GameHwnd = NULL;
HWND MyHwnd = NULL;
MSG Message = { NULL };

struct GunSettings {
    float smoothing;
    int fovSize;
    int adsFovSize;
}; std::vector<GunSettings> gunSettings;

const char* targetOptions[] = { "Head", "Neck", "Chest"};
static int currentTarget = 0;
const char* snaplineOptions[] = {"None", "Bottom", "Top", "Center" };
static int currentSnapline = 0;
const char* boxOptions[] = {"None", "Full", "Cornered"};
static int currentBox = 0;
const char* gunOptions[] = { "Pistol", "Rifle", "Shotgun", "SMG", "Sniper", "Other" };
static int currentGun = 0;

uintptr_t procid = NULL;

int get_fps()
{
    using namespace chrono;
    static int count = 0;
    static auto last = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    static int fps = 0;

    count++;

    if (duration_cast<milliseconds>(now - last).count() > 1000) {
        fps = count;
        count = 0;
        last = now;
    }

    return fps;
}

static const char* keyNames[] = { "", "Left Mouse", "Right Mouse", "Cancel", "Middle Mouse", "Mouse 5", "Mouse 4", "", "Backspace", "Tab", "", "", "Clear", "Enter", "", "", "Shift", "Control", "Alt", "Pause", "Caps Lock", "", "", "", "", "", "", "Escape", "", "", "", "", "Space", "Page Up", "Page Down", "End", "Home", "Left Arrow", "Up Arrow", "Right Arrow", "Down Arrow", "Select", "Print", "Execute", "Print Screen", "Insert", "Delete", "Help", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "", "", "", "", "", "", "", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Left Win", "Right Win", "Apps", "", "Sleep", "Numpad 0", "Numpad 1", "Numpad 2", "Numpad 3", "Numpad 4", "Numpad 5", "Numpad 6", "Numpad 7", "Numpad 8", "Numpad 9", "Multiply", "Add", "Separator", "Subtract", "Decimal", "Divide", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24", "", "", "", "", "", "", "", "", "Num Lock", "Scroll Lock", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "", "", "", "", "", "Left Shift", "Right Shift", "Left Control", "Right Control", "Left Menu", "Right Menu", "Browser Back", "Browser Forward", "Browser Refresh", "Browser Stop", "Browser Search", "Browser Favorites", "Browser Home", "Volume Mute", "Volume Down", "Volume Up", "Next Track", "Previous Track", "Stop Media", "Play/Pause", "Launch Mail", "Launch Media Select", "Launch App1", "Launch App2", "", "", "", "", "", "", "", ";", "=", ",", "-", ".", "/", "`", "", "", "", "", "", "", "", "", "", "", "[", "\\", "]", "'", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "OEM Specific", "Attn", "CrSel", "ExSel", "Erase EOF", "Play", "Zoom", "PA1", "OEM Clear", "Scroll Wheel Up", "Scroll Wheel Down" };

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if (out_text)
        *out_text = items[idx];
    return true;
}

void HotkeyButton(int aimkey, void* changekey, int status)
{
    const char* preview_value = NULL;
    if (aimkey >= 0 && aimkey < IM_ARRAYSIZE(keyNames))
        Items_ArrayGetter(keyNames, aimkey, &preview_value);

    string aimkeys;
    if (preview_value == NULL)
        aimkeys = ("Select Key");
    else
        aimkeys = preview_value;

    if (status == 1)
    {
        aimkeys = ("Press Key");
    }
    if (ImGui::Button(aimkeys.c_str(), ImVec2(125, 25)))
    {
        if (status == 0)
        {
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey, nullptr, 0, nullptr);
        }
    }
}

static int keystatus = 0;

void ChangeKey(void* blank)
{
    keystatus = 1;
    while (true)
    {
        for (int i = 0; i < 0x87; i++)
        {
            if (GetKeyState(i) & 0x8000)
            {
                Features::rAimkey = i;
                keystatus = 0;
                return;
            }
        }
    }
}

static int keystatusTrig = 0;

void ChangeKeyTrig(void* blank)
{
    keystatusTrig = 1;
    while (true)
    {
        for (int i = 0; i < 0x87; i++)
        {
            if (GetKeyState(i) & 0x8000)
            {
                Features::rTrigkey = i;
                keystatusTrig = 0;
                return;
            }
        }
    }
}

HRESULT InitializeDirectXOverlay() {
    WNDCLASSEXA wcex = {
        sizeof(WNDCLASSEXA), 0, DefWindowProcA, 0, 0, nullptr,
        LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW),
        nullptr, nullptr, ("Medal"), LoadIcon(nullptr, IDI_APPLICATION)
    };

    RECT Rect;
    GetWindowRect(GetDesktopWindow(), &Rect);

    if (!RegisterClassExA(&wcex)) {
        return E_FAIL;
    }

    MyHwnd = CreateWindowExA(NULL, ("Medal"), ("MedalOverlay"), WS_POPUP, Rect.left, Rect.top, Rect.right, Rect.bottom, NULL, NULL, wcex.hInstance, NULL);
    if (!MyHwnd) {
        return E_FAIL;
    }

    SetWindowLong(MyHwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED); // topmost is not confirmed to flag but if you dont want to use topmost u can setwindowpos() to front in mainloop
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(MyHwnd, &margin);
    ShowWindow(MyHwnd, SW_SHOW);

    SetLayeredWindowAttributes(MyHwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    UpdateWindow(MyHwnd);

    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object))) {
        return E_FAIL;
    }

    ZeroMemory(&p_Params, sizeof(p_Params));
    p_Params.Windowed = TRUE;
    p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    p_Params.hDeviceWindow = MyHwnd;
    p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
    p_Params.BackBufferWidth = ScreenWidth;
    p_Params.BackBufferHeight = ScreenHeight;
    p_Params.EnableAutoDepthStencil = TRUE;
    p_Params.AutoDepthStencilFormat = D3DFMT_D16;
    p_Params.PresentationInterval = 0; // vsync

    if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, MyHwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device))) {
        p_Object->Release();
        return E_FAIL;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    if (!ImGui_ImplWin32_Init(MyHwnd) || !ImGui_ImplDX9_Init(p_Device)) {
        p_Object->Release();
        return E_FAIL;
    }

    ImGui::GetStyle();
    p_Object->Release();

    return S_OK;
}


void CleanupD3D() {
    if (p_Device != NULL) {
        p_Device->EndScene();
        p_Device->Release();
    }
    if (p_Object != NULL) {
        p_Object->Release();
    }
}

bool rMenu = true;
bool rColorMenu = false;
int tab = 0;

int tab_count = 0, tabs = 0;
bool active = false;
float size_child = 0;

BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
    char lpString[1024];
    sprintf_s(lpString, sizeof(lpString), TEXT("%d"), nInteger);
    return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
    char lpString[1024];
    sprintf_s(lpString, sizeof(lpString), TEXT("%f"), nInteger);
    return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
{
    char szData[32];

    GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

    return (float)atof(szData);
}

int MyGetPrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
{
    char szData[32];

    GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

    return (int)atof(szData);
}

void savecfg(LPCSTR path)
{
    WritePrivateProfileInt(("a"), TEXT("Aimbot"), Features::rAimbot, path);
    WritePrivateProfileInt(("a"), TEXT("Gun Configs"), Features::rGunConfigs, path);
    WritePrivateProfileInt(("a"), TEXT("Triggerbot"), Features::rTriggerbot, path);
    WritePrivateProfileInt(("a"), TEXT("Prediction"), Features::rPrediction, path);
    WritePrivateProfileInt(("a"), TEXT("Visible Check"), Features::rVisibleCheck, path);
    WritePrivateProfileInt(("a"), TEXT("Lock Target"), Features::rLockTarget, path);
    WritePrivateProfileInt(("a"), TEXT("FOV Circle"), Features::rFovCircle, path);
    WritePrivateProfileInt(("a"), TEXT("Aimline"), Features::rAimline, path);
    WritePrivateProfileInt(("a"), TEXT("Ignore Downed"), Features::rDownedCheck, path);
    WritePrivateProfileInt(("a"), TEXT("Ignore Bots"), Features::rBotCheck, path);
    WritePrivateProfileInt(("a"), TEXT("Target"), currentTarget, path);
    WritePrivateProfileFloat(("a"), TEXT("FOV Color1"), Features::FovColor.x, path);
    WritePrivateProfileFloat(("a"), TEXT("FOV Color2"), Features::FovColor.y, path);
    WritePrivateProfileFloat(("a"), TEXT("FOV Color3"), Features::FovColor.z, path);
    WritePrivateProfileFloat(("a"), TEXT("FOV Color4"), Features::FovColor.w, path);
    WritePrivateProfileFloat(("a"), TEXT("Fov Fill Color1"), Features::FovFillColor.x, path);
    WritePrivateProfileFloat(("a"), TEXT("Fov Fill Color2"), Features::FovFillColor.y, path);
    WritePrivateProfileFloat(("a"), TEXT("Fov Fill Color3"), Features::FovFillColor.z, path);
    WritePrivateProfileFloat(("a"), TEXT("Fov Fill Color4"), Features::FovFillColor.w, path);
    WritePrivateProfileFloat(("a"), TEXT("Fill Color1"), Features::FillColorVis.x, path);
    WritePrivateProfileFloat(("a"), TEXT("Fill Color2"), Features::FillColorVis.y, path);
    WritePrivateProfileFloat(("a"), TEXT("Fill Color3"), Features::FillColorVis.z, path);
    WritePrivateProfileFloat(("a"), TEXT("Fill Color4"), Features::FillColorVis.w, path);
    WritePrivateProfileFloat(("a"), TEXT("Fill ColorHid1"), Features::FillColorHidden.x, path);
    WritePrivateProfileFloat(("a"), TEXT("Fill ColorHid2"), Features::FillColorHidden.y, path);
    WritePrivateProfileFloat(("a"), TEXT("Fill ColorHid3"), Features::FillColorHidden.z, path);
    WritePrivateProfileFloat(("a"), TEXT("Fill ColorHid4"), Features::FillColorHidden.w, path);
    WritePrivateProfileFloat(("a"), TEXT("Aimline Color1"), Features::AimlineColor.x, path);
    WritePrivateProfileFloat(("a"), TEXT("Aimline Color2"), Features::AimlineColor.y, path);
    WritePrivateProfileFloat(("a"), TEXT("Aimline Color3"), Features::AimlineColor.z, path);
    WritePrivateProfileFloat(("a"), TEXT("Aimline Color4"), Features::AimlineColor.w, path);
    WritePrivateProfileInt(("a"), TEXT("Aimkey"), Features::rAimkey, path);
    WritePrivateProfileInt(("a"), TEXT("Trigkey"), Features::rTrigkey, path);
    WritePrivateProfileFloat(("a"), TEXT("Triggerbot Delay"), Features::rTriggerbotDelay, path);
    WritePrivateProfileFloat(("a"), TEXT("Humanization"), Features::rHumanization, path);
    WritePrivateProfileFloat(("a"), TEXT("Smoothing0"), gunSettings[0].smoothing, path);
    WritePrivateProfileFloat(("a"), TEXT("Smoothing1"), gunSettings[1].smoothing, path);
    WritePrivateProfileFloat(("a"), TEXT("Smoothing2"), gunSettings[2].smoothing, path);
    WritePrivateProfileFloat(("a"), TEXT("Smoothing3"), gunSettings[3].smoothing, path);
    WritePrivateProfileFloat(("a"), TEXT("Smoothing4"), gunSettings[4].smoothing, path);
    WritePrivateProfileFloat(("a"), TEXT("Smoothing5"), gunSettings[5].smoothing, path);
    WritePrivateProfileFloat(("a"), TEXT("Fov0"), gunSettings[0].fovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("Fov1"), gunSettings[1].fovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("Fov2"), gunSettings[2].fovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("Fov3"), gunSettings[3].fovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("Fov4"), gunSettings[4].fovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("Fov5"), gunSettings[5].fovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("ADSFov0"), gunSettings[0].adsFovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("ADSFov1"), gunSettings[1].adsFovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("ADSFov2"), gunSettings[2].adsFovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("ADSFov3"), gunSettings[3].adsFovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("ADSFov4"), gunSettings[4].adsFovSize, path);
    WritePrivateProfileFloat(("a"), TEXT("ADSFov5"), gunSettings[5].adsFovSize, path);

    WritePrivateProfileInt(("b"), TEXT("Box Type"), currentBox, path);
    WritePrivateProfileInt(("b"), TEXT("Skeleton"), Features::rSkeleton, path);
    WritePrivateProfileInt(("b"), TEXT("Chams"), Features::rChams, path);
    WritePrivateProfileInt(("b"), TEXT("Wireframe"), Features::rWireframe, path);
    WritePrivateProfileInt(("b"), TEXT("Viewangle"), Features::rViewAngle, path);
    WritePrivateProfileInt(("b"), TEXT("Distance"), Features::rDistance, path);
    WritePrivateProfileInt(("b"), TEXT("FPS"), Features::rFPS, path);
    WritePrivateProfileFloat(("a"), TEXT("Accent Color1"), Features::AccentColor.x, path);
    WritePrivateProfileFloat(("a"), TEXT("Accent Color2"), Features::AccentColor.y, path);
    WritePrivateProfileFloat(("a"), TEXT("Accent Color3"), Features::AccentColor.z, path);
    WritePrivateProfileFloat(("a"), TEXT("Accent Color4"), Features::AccentColor.w, path);
    WritePrivateProfileFloat(("b"), TEXT("Box Color1"), Features::BoxColorVis.x, path);
    WritePrivateProfileFloat(("b"), TEXT("Box Color2"), Features::BoxColorVis.y, path);
    WritePrivateProfileFloat(("b"), TEXT("Box Color3"), Features::BoxColorVis.z, path);
    WritePrivateProfileFloat(("b"), TEXT("Box Color4"), Features::BoxColorVis.w, path);
    WritePrivateProfileFloat(("b"), TEXT("Skeleton Color1"), Features::SkeletonColorVis.x, path);
    WritePrivateProfileFloat(("b"), TEXT("Skeleton Color2"), Features::SkeletonColorVis.y, path);
    WritePrivateProfileFloat(("b"), TEXT("Skeleton Color3"), Features::SkeletonColorVis.z, path);
    WritePrivateProfileFloat(("b"), TEXT("Skeleton Color4"), Features::SkeletonColorVis.w, path);
    WritePrivateProfileFloat(("b"), TEXT("Distance Color1"), Features::DistanceColorVis.x, path);
    WritePrivateProfileFloat(("b"), TEXT("Distance Color2"), Features::DistanceColorVis.y, path);
    WritePrivateProfileFloat(("b"), TEXT("Distance Color3"), Features::DistanceColorVis.z, path);
    WritePrivateProfileFloat(("b"), TEXT("Distance Color4"), Features::DistanceColorVis.w, path);
    WritePrivateProfileFloat(("b"), TEXT("Username Color1"), Features::UsernameColorVis.x, path);
    WritePrivateProfileFloat(("b"), TEXT("Username Color2"), Features::UsernameColorVis.y, path);
    WritePrivateProfileFloat(("b"), TEXT("Username Color3"), Features::UsernameColorVis.z, path);
    WritePrivateProfileFloat(("b"), TEXT("Username Color4"), Features::UsernameColorVis.w, path);
    WritePrivateProfileFloat(("b"), TEXT("Kills Color1"), Features::KillsColorVis.x, path);
    WritePrivateProfileFloat(("b"), TEXT("Kills Color2"), Features::KillsColorVis.y, path);
    WritePrivateProfileFloat(("b"), TEXT("Kills Color3"), Features::KillsColorVis.z, path);
    WritePrivateProfileFloat(("b"), TEXT("Kills Color4"), Features::KillsColorVis.w, path);
    WritePrivateProfileFloat(("b"), TEXT("Weapon Color1"), Features::WeaponColorVis.x, path);
    WritePrivateProfileFloat(("b"), TEXT("Weapon Color2"), Features::WeaponColorVis.y, path);
    WritePrivateProfileFloat(("b"), TEXT("Weapon Color3"), Features::WeaponColorVis.z, path);
    WritePrivateProfileFloat(("b"), TEXT("Weapon Color4"), Features::WeaponColorVis.w, path);
    WritePrivateProfileFloat(("b"), TEXT("Snapline Color1"), Features::SnaplineColorVis.x, path);
    WritePrivateProfileFloat(("b"), TEXT("Snapline Color2"), Features::SnaplineColorVis.y, path);
    WritePrivateProfileFloat(("b"), TEXT("Snapline Color3"), Features::SnaplineColorVis.z, path);
    WritePrivateProfileFloat(("b"), TEXT("Snapline Color4"), Features::SnaplineColorVis.w, path);
    WritePrivateProfileFloat(("b"), TEXT("FOVArrow Color1"), Features::FovArrowsColorVis.x, path);
    WritePrivateProfileFloat(("b"), TEXT("FOVArrow Color2"), Features::FovArrowsColorVis.y, path);
    WritePrivateProfileFloat(("b"), TEXT("FOVArrow Color3"), Features::FovArrowsColorVis.z, path);
    WritePrivateProfileFloat(("b"), TEXT("FOVArrow Color4"), Features::FovArrowsColorVis.w, path);
    WritePrivateProfileFloat(("b"), TEXT("HidBox Color1"), Features::BoxColorHidden.x, path);
    WritePrivateProfileFloat(("b"), TEXT("HidBox Color2"), Features::BoxColorHidden.y, path);
    WritePrivateProfileFloat(("b"), TEXT("HidBox Color3"), Features::BoxColorHidden.z, path);
    WritePrivateProfileFloat(("b"), TEXT("HidBox Color4"), Features::BoxColorHidden.w, path);
    WritePrivateProfileFloat(("b"), TEXT("HidSkeleton Color1"), Features::SkeletonColorHidden.x, path);
    WritePrivateProfileFloat(("b"), TEXT("HidSkeleton Color2"), Features::SkeletonColorHidden.y, path);
    WritePrivateProfileFloat(("b"), TEXT("HidSkeleton Color3"), Features::SkeletonColorHidden.z, path);
    WritePrivateProfileFloat(("b"), TEXT("HidSkeleton Color4"), Features::SkeletonColorHidden.w, path);
    WritePrivateProfileFloat(("b"), TEXT("HidDistance Color1"), Features::DistanceColorHidden.x, path);
    WritePrivateProfileFloat(("b"), TEXT("HidDistance Color2"), Features::DistanceColorHidden.y, path);
    WritePrivateProfileFloat(("b"), TEXT("HidDistance Color3"), Features::DistanceColorHidden.z, path);
    WritePrivateProfileFloat(("b"), TEXT("HidDistance Color4"), Features::DistanceColorHidden.w, path);
    WritePrivateProfileFloat(("b"), TEXT("HidUsername Color1"), Features::UsernameColorHidden.x, path);
    WritePrivateProfileFloat(("b"), TEXT("HidUsername Color2"), Features::UsernameColorHidden.y, path);
    WritePrivateProfileFloat(("b"), TEXT("HidUsername Color3"), Features::UsernameColorHidden.z, path);
    WritePrivateProfileFloat(("b"), TEXT("HidUsername Color4"), Features::UsernameColorHidden.w, path);
    WritePrivateProfileFloat(("b"), TEXT("HidKills Color1"), Features::KillsColorHidden.x, path);
    WritePrivateProfileFloat(("b"), TEXT("HidKills Color2"), Features::KillsColorHidden.y, path);
    WritePrivateProfileFloat(("b"), TEXT("HidKills Color3"), Features::KillsColorHidden.z, path);
    WritePrivateProfileFloat(("b"), TEXT("HidKills Color4"), Features::KillsColorHidden.w, path);
    WritePrivateProfileFloat(("b"), TEXT("HidWeapon Color1"), Features::WeaponColorHidden.x, path);
    WritePrivateProfileFloat(("b"), TEXT("HidWeapon Color2"), Features::WeaponColorHidden.y, path);
    WritePrivateProfileFloat(("b"), TEXT("HidWeapon Color3"), Features::WeaponColorHidden.z, path);
    WritePrivateProfileFloat(("b"), TEXT("HidWeapon Color4"), Features::WeaponColorHidden.w, path);
    WritePrivateProfileFloat(("b"), TEXT("HidSnapline Color1"), Features::SnaplineColorHidden.x, path);
    WritePrivateProfileFloat(("b"), TEXT("HidSnapline Color2"), Features::SnaplineColorHidden.y, path);
    WritePrivateProfileFloat(("b"), TEXT("HidSnapline Color3"), Features::SnaplineColorHidden.z, path);
    WritePrivateProfileFloat(("b"), TEXT("HidSnapline Color4"), Features::SnaplineColorHidden.w, path);
    WritePrivateProfileFloat(("b"), TEXT("HidFOVArrow Color1"), Features::FovArrowsColorHidden.x, path);
    WritePrivateProfileFloat(("b"), TEXT("HidFOVArrow Color2"), Features::FovArrowsColorHidden.y, path);
    WritePrivateProfileFloat(("b"), TEXT("HidFOVArrow Color3"), Features::FovArrowsColorHidden.z, path);
    WritePrivateProfileFloat(("b"), TEXT("HidFOVArrow Color4"), Features::FovArrowsColorHidden.w, path);

    WritePrivateProfileInt(("b"), TEXT("Targeting Only"), Features::rTargetingOnly, path);
    WritePrivateProfileInt(("b"), TEXT("Weapon Name"), Features::rWeaponname, path);
    WritePrivateProfileInt(("b"), TEXT("Platform Name"), Features::rPlatformname, path);
    WritePrivateProfileInt(("b"), TEXT("Username"), Features::rUsername, path);
    WritePrivateProfileInt(("b"), TEXT("Rank"), Features::rRanked, path);
    WritePrivateProfileInt(("b"), TEXT("Kills"), Features::rKills, path);
    WritePrivateProfileInt(("b"), TEXT("Level"), Features::rLevel, path);
    WritePrivateProfileInt(("b"), TEXT("FOV Arrows"), Features::rFovRadar, path);
    WritePrivateProfileInt(("b"), TEXT("Snapline Position"), currentSnapline, path);
    WritePrivateProfileFloat(("b"), TEXT("ESP Distance"), Features::rMaxDistance, path);
}

void loadcfg(LPCSTR path)
{
    Features::rAimbot = MyGetPrivateProfileInt(("a"), TEXT("Aimbot"), Features::rAimbot, path);
    Features::rGunConfigs = MyGetPrivateProfileInt(("a"), TEXT("Gun Configs"), Features::rGunConfigs, path);
    Features::rTriggerbot = MyGetPrivateProfileInt(("a"), TEXT("Triggerbot"), Features::rTriggerbot, path);
    Features::rPrediction = MyGetPrivateProfileInt(("a"), TEXT("Prediction"), Features::rPrediction, path);
    Features::rVisibleCheck = MyGetPrivateProfileInt(("a"), TEXT("Visible Check"), Features::rVisibleCheck, path);
    Features::rLockTarget = MyGetPrivateProfileInt(("a"), TEXT("Lock Target"), Features::rLockTarget, path);
    Features::rFovCircle = MyGetPrivateProfileInt(("a"), TEXT("FOV Circle"), Features::rFovCircle, path);
    Features::rAimline = MyGetPrivateProfileInt(("a"), TEXT("Aimline"), Features::rAimline, path);
    Features::rDownedCheck = MyGetPrivateProfileInt(("a"), TEXT("Ignore Downed"), Features::rDownedCheck, path);
    Features::rBotCheck = MyGetPrivateProfileInt(("a"), TEXT("Ignore Bots"), Features::rBotCheck, path);
    currentTarget = MyGetPrivateProfileInt(("a"), TEXT("Target"), currentTarget, path);
    Features::FovColor.x = GetPrivateProfileFloat(("a"), TEXT("FOV Color1"), Features::FovColor.x, path);
    Features::FovColor.y = GetPrivateProfileFloat(("a"), TEXT("FOV Color2"), Features::FovColor.y, path);
    Features::FovColor.z = GetPrivateProfileFloat(("a"), TEXT("FOV Color3"), Features::FovColor.z, path);
    Features::FovColor.w = GetPrivateProfileFloat(("a"), TEXT("FOV Color4"), Features::FovColor.w, path);
    Features::FovFillColor.x = GetPrivateProfileFloat(("a"), TEXT("Fov Fill Color1"), Features::FovFillColor.x, path);
    Features::FovFillColor.y = GetPrivateProfileFloat(("a"), TEXT("Fov Fill Color2"), Features::FovFillColor.y, path);
    Features::FovFillColor.z = GetPrivateProfileFloat(("a"), TEXT("Fov Fill Color3"), Features::FovFillColor.z, path);
    Features::FovFillColor.w = GetPrivateProfileFloat(("a"), TEXT("Fov Fill Color4"), Features::FovFillColor.w, path);
    Features::FillColorVis.x = GetPrivateProfileFloat(("a"), TEXT("Fill Color1"), Features::FillColorVis.x, path);
    Features::FillColorVis.y = GetPrivateProfileFloat(("a"), TEXT("Fill Color2"), Features::FillColorVis.y, path);
    Features::FillColorVis.z = GetPrivateProfileFloat(("a"), TEXT("Fill Color3"), Features::FillColorVis.z, path);
    Features::FillColorVis.w = GetPrivateProfileFloat(("a"), TEXT("Fill Color4"), Features::FillColorVis.w, path);
    Features::FillColorHidden.x = GetPrivateProfileFloat(("a"), TEXT("Fill ColorHid1"), Features::FillColorHidden.x, path);
    Features::FillColorHidden.y = GetPrivateProfileFloat(("a"), TEXT("Fill ColorHid2"), Features::FillColorHidden.y, path);
    Features::FillColorHidden.z = GetPrivateProfileFloat(("a"), TEXT("Fill ColorHid3"), Features::FillColorHidden.z, path);
    Features::FillColorHidden.w = GetPrivateProfileFloat(("a"), TEXT("Fill ColorHid4"), Features::FillColorHidden.w, path);
    Features::AimlineColor.x = GetPrivateProfileFloat(("a"), TEXT("Aimline Color1"), Features::AimlineColor.x, path);
    Features::AimlineColor.y = GetPrivateProfileFloat(("a"), TEXT("Aimline Color2"), Features::AimlineColor.y, path);
    Features::AimlineColor.z = GetPrivateProfileFloat(("a"), TEXT("Aimline Color3"), Features::AimlineColor.z, path);
    Features::AimlineColor.w = GetPrivateProfileFloat(("a"), TEXT("Aimline Color4"), Features::AimlineColor.w, path);
    Features::rAimkey = MyGetPrivateProfileInt(("a"), TEXT("Aimkey"), Features::rAimkey, path);
    Features::rTrigkey = MyGetPrivateProfileInt(("a"), TEXT("Trigkey"), Features::rTrigkey, path);
    Features::rTriggerbotDelay = GetPrivateProfileFloat(("a"), TEXT("Triggerbot Delay"), Features::rTriggerbotDelay, path);
    Features::rHumanization = GetPrivateProfileFloat(("a"), TEXT("Humanization"), Features::rHumanization, path);
    gunSettings[0].smoothing = GetPrivateProfileFloat(("a"), TEXT("Smoothing0"), gunSettings[0].smoothing, path);
    gunSettings[1].smoothing = GetPrivateProfileFloat(("a"), TEXT("Smoothing1"), gunSettings[1].smoothing, path);
    gunSettings[2].smoothing = GetPrivateProfileFloat(("a"), TEXT("Smoothing2"), gunSettings[2].smoothing, path);
    gunSettings[3].smoothing = GetPrivateProfileFloat(("a"), TEXT("Smoothing3"), gunSettings[3].smoothing, path);
    gunSettings[4].smoothing = GetPrivateProfileFloat(("a"), TEXT("Smoothing4"), gunSettings[4].smoothing, path);
    gunSettings[5].smoothing = GetPrivateProfileFloat(("a"), TEXT("Smoothing5"), gunSettings[5].smoothing, path);
    gunSettings[0].fovSize = GetPrivateProfileFloat(("a"), TEXT("Fov0"), gunSettings[0].fovSize, path);
    gunSettings[1].fovSize = GetPrivateProfileFloat(("a"), TEXT("Fov1"), gunSettings[1].fovSize, path);
    gunSettings[2].fovSize = GetPrivateProfileFloat(("a"), TEXT("Fov2"), gunSettings[2].fovSize, path);
    gunSettings[3].fovSize = GetPrivateProfileFloat(("a"), TEXT("Fov3"), gunSettings[3].fovSize, path);
    gunSettings[4].fovSize = GetPrivateProfileFloat(("a"), TEXT("Fov4"), gunSettings[4].fovSize, path);
    gunSettings[5].fovSize = GetPrivateProfileFloat(("a"), TEXT("Fov5"), gunSettings[5].fovSize, path);
    gunSettings[0].adsFovSize = GetPrivateProfileFloat(("a"), TEXT("ADSFov0"), gunSettings[0].adsFovSize, path);
    gunSettings[1].adsFovSize = GetPrivateProfileFloat(("a"), TEXT("ADSFov1"), gunSettings[1].adsFovSize, path);
    gunSettings[2].adsFovSize = GetPrivateProfileFloat(("a"), TEXT("ADSFov2"), gunSettings[2].adsFovSize, path);
    gunSettings[3].adsFovSize = GetPrivateProfileFloat(("a"), TEXT("ADSFov3"), gunSettings[3].adsFovSize, path);
    gunSettings[4].adsFovSize = GetPrivateProfileFloat(("a"), TEXT("ADSFov4"), gunSettings[4].adsFovSize, path);
    gunSettings[5].adsFovSize = GetPrivateProfileFloat(("a"), TEXT("ADSFov5"), gunSettings[5].adsFovSize, path);

    currentBox = MyGetPrivateProfileInt(("b"), TEXT("Box Type"), currentBox, path);
    Features::rSkeleton = MyGetPrivateProfileInt(("b"), TEXT("Skeleton"), Features::rSkeleton, path);
    Features::rChams = MyGetPrivateProfileInt(("b"), TEXT("Chams"), Features::rChams, path);
    Features::rWireframe = MyGetPrivateProfileInt(("b"), TEXT("Wireframe"), Features::rWireframe, path);
    Features::rViewAngle = MyGetPrivateProfileInt(("b"), TEXT("Viewangle"), Features::rViewAngle, path);
    Features::rDistance = MyGetPrivateProfileInt(("b"), TEXT("Distance"), Features::rDistance, path);
    Features::rFPS = MyGetPrivateProfileInt(("b"), TEXT("FPS"), Features::rFPS, path);
    Features::AccentColor.x = GetPrivateProfileFloat(("a"), TEXT("Accent Color1"), Features::AccentColor.x, path);
    Features::AccentColor.y = GetPrivateProfileFloat(("a"), TEXT("Accent Color2"), Features::AccentColor.y, path);
    Features::AccentColor.z = GetPrivateProfileFloat(("a"), TEXT("Accent Color3"), Features::AccentColor.z, path);
    Features::AccentColor.w = GetPrivateProfileFloat(("a"), TEXT("Accent Color4"), Features::AccentColor.w, path);
    Features::BoxColorVis.x = GetPrivateProfileFloat(("b"), TEXT("Box Color1"), Features::BoxColorVis.x, path);
    Features::BoxColorVis.y = GetPrivateProfileFloat(("b"), TEXT("Box Color2"), Features::BoxColorVis.y, path);
    Features::BoxColorVis.z = GetPrivateProfileFloat(("b"), TEXT("Box Color3"), Features::BoxColorVis.z, path);
    Features::BoxColorVis.w = GetPrivateProfileFloat(("b"), TEXT("Box Color4"), Features::BoxColorVis.w, path);
    Features::SkeletonColorVis.x = GetPrivateProfileFloat(("b"), TEXT("Skeleton Color1"), Features::SkeletonColorVis.x, path);
    Features::SkeletonColorVis.y = GetPrivateProfileFloat(("b"), TEXT("Skeleton Color2"), Features::SkeletonColorVis.y, path);
    Features::SkeletonColorVis.z = GetPrivateProfileFloat(("b"), TEXT("Skeleton Color3"), Features::SkeletonColorVis.z, path);
    Features::SkeletonColorVis.w = GetPrivateProfileFloat(("b"), TEXT("Skeleton Color4"), Features::SkeletonColorVis.w, path);
    Features::DistanceColorVis.x = GetPrivateProfileFloat(("b"), TEXT("Distance Color1"), Features::DistanceColorVis.x, path);
    Features::DistanceColorVis.y = GetPrivateProfileFloat(("b"), TEXT("Distance Color2"), Features::DistanceColorVis.y, path);
    Features::DistanceColorVis.z = GetPrivateProfileFloat(("b"), TEXT("Distance Color3"), Features::DistanceColorVis.z, path);
    Features::DistanceColorVis.w = GetPrivateProfileFloat(("b"), TEXT("Distance Color4"), Features::DistanceColorVis.w, path);
    Features::UsernameColorVis.x = GetPrivateProfileFloat(("b"), TEXT("Username Color1"), Features::UsernameColorVis.x, path);
    Features::UsernameColorVis.y = GetPrivateProfileFloat(("b"), TEXT("Username Color2"), Features::UsernameColorVis.y, path);
    Features::UsernameColorVis.z = GetPrivateProfileFloat(("b"), TEXT("Username Color3"), Features::UsernameColorVis.z, path);
    Features::UsernameColorVis.w = GetPrivateProfileFloat(("b"), TEXT("Username Color4"), Features::UsernameColorVis.w, path);
    Features::KillsColorVis.x = GetPrivateProfileFloat(("b"), TEXT("Kills Color1"), Features::KillsColorVis.x, path);
    Features::KillsColorVis.y = GetPrivateProfileFloat(("b"), TEXT("Kills Color2"), Features::KillsColorVis.y, path);
    Features::KillsColorVis.z = GetPrivateProfileFloat(("b"), TEXT("Kills Color3"), Features::KillsColorVis.z, path);
    Features::KillsColorVis.w = GetPrivateProfileFloat(("b"), TEXT("Kills Color4"), Features::KillsColorVis.w, path);
    Features::WeaponColorVis.x = GetPrivateProfileFloat(("b"), TEXT("Weapon Color1"), Features::WeaponColorVis.x, path);
    Features::WeaponColorVis.y = GetPrivateProfileFloat(("b"), TEXT("Weapon Color2"), Features::WeaponColorVis.y, path);
    Features::WeaponColorVis.z = GetPrivateProfileFloat(("b"), TEXT("Weapon Color3"), Features::WeaponColorVis.z, path);
    Features::WeaponColorVis.w = GetPrivateProfileFloat(("b"), TEXT("Weapon Color4"), Features::WeaponColorVis.w, path);
    Features::SnaplineColorVis.x = GetPrivateProfileFloat(("b"), TEXT("Snapline Color1"), Features::SnaplineColorVis.x, path);
    Features::SnaplineColorVis.y = GetPrivateProfileFloat(("b"), TEXT("Snapline Color2"), Features::SnaplineColorVis.y, path);
    Features::SnaplineColorVis.z = GetPrivateProfileFloat(("b"), TEXT("Snapline Color3"), Features::SnaplineColorVis.z, path);
    Features::SnaplineColorVis.w = GetPrivateProfileFloat(("b"), TEXT("Snapline Color4"), Features::SnaplineColorVis.w, path);
    Features::FovArrowsColorVis.x = GetPrivateProfileFloat(("b"), TEXT("FOVArrow Color1"), Features::FovArrowsColorVis.x, path);
    Features::FovArrowsColorVis.y = GetPrivateProfileFloat(("b"), TEXT("FOVArrow Color2"), Features::FovArrowsColorVis.y, path);
    Features::FovArrowsColorVis.z = GetPrivateProfileFloat(("b"), TEXT("FOVArrow Color3"), Features::FovArrowsColorVis.z, path);
    Features::FovArrowsColorVis.w = GetPrivateProfileFloat(("b"), TEXT("FOVArrow Color4"), Features::FovArrowsColorVis.w, path);
    Features::DistanceColorHidden.x =  GetPrivateProfileFloat(("b"), TEXT("HidDistance Color1"), Features::DistanceColorHidden.x, path);
    Features::DistanceColorHidden.y = GetPrivateProfileFloat(("b"), TEXT("HidDistance Color2"), Features::DistanceColorHidden.y, path);
    Features::DistanceColorHidden.z = GetPrivateProfileFloat(("b"), TEXT("HidDistance Color3"), Features::DistanceColorHidden.z, path);
    Features::DistanceColorHidden.w = GetPrivateProfileFloat(("b"), TEXT("HidDistance Color4"), Features::DistanceColorHidden.w, path);
    Features::UsernameColorHidden.x = GetPrivateProfileFloat(("b"), TEXT("HidUsername Color1"), Features::UsernameColorHidden.x, path);
    Features::UsernameColorHidden.y = GetPrivateProfileFloat(("b"), TEXT("HidUsername Color2"), Features::UsernameColorHidden.y, path);
    Features::UsernameColorHidden.z = GetPrivateProfileFloat(("b"), TEXT("HidUsername Color3"), Features::UsernameColorHidden.z, path);
    Features::UsernameColorHidden.w = GetPrivateProfileFloat(("b"), TEXT("HidUsername Color4"), Features::UsernameColorHidden.w, path);
    Features::KillsColorHidden.x = GetPrivateProfileFloat(("b"), TEXT("HidKills Color1"), Features::KillsColorHidden.x, path);
    Features::KillsColorHidden.y = GetPrivateProfileFloat(("b"), TEXT("HidKills Color2"), Features::KillsColorHidden.y, path);
    Features::KillsColorHidden.z = GetPrivateProfileFloat(("b"), TEXT("HidKills Color3"), Features::KillsColorHidden.z, path);
    Features::KillsColorHidden.w = GetPrivateProfileFloat(("b"), TEXT("HidKills Color4"), Features::KillsColorHidden.w, path);
    Features::WeaponColorHidden.x = GetPrivateProfileFloat(("b"), TEXT("HidWeapon Color1"), Features::WeaponColorHidden.x, path);
    Features::WeaponColorHidden.y = GetPrivateProfileFloat(("b"), TEXT("HidWeapon Color2"), Features::WeaponColorHidden.y, path);
    Features::WeaponColorHidden.z = GetPrivateProfileFloat(("b"), TEXT("HidWeapon Color3"), Features::WeaponColorHidden.z, path);
    Features::WeaponColorHidden.w = GetPrivateProfileFloat(("b"), TEXT("HidWeapon Color4"), Features::WeaponColorHidden.w, path);
    Features::SnaplineColorHidden.x = GetPrivateProfileFloat(("b"), TEXT("HidSnapline Color1"), Features::SnaplineColorHidden.x, path);
    Features::SnaplineColorHidden.y = GetPrivateProfileFloat(("b"), TEXT("HidSnapline Color2"), Features::SnaplineColorHidden.y, path);
    Features::SnaplineColorHidden.z = GetPrivateProfileFloat(("b"), TEXT("HidSnapline Color3"), Features::SnaplineColorHidden.z, path);
    Features::SnaplineColorHidden.w = GetPrivateProfileFloat(("b"), TEXT("HidSnapline Color4"), Features::SnaplineColorHidden.w, path);
    Features::FovArrowsColorHidden.x = GetPrivateProfileFloat(("b"), TEXT("HidFOVArrow Color1"), Features::FovArrowsColorHidden.x, path);
    Features::FovArrowsColorHidden.y = GetPrivateProfileFloat(("b"), TEXT("HidFOVArrow Color2"), Features::FovArrowsColorHidden.y, path);
    Features::FovArrowsColorHidden.z = GetPrivateProfileFloat(("b"), TEXT("HidFOVArrow Color3"), Features::FovArrowsColorHidden.z, path);
    Features::FovArrowsColorHidden.w = GetPrivateProfileFloat(("b"), TEXT("HidFOVArrow Color4"), Features::FovArrowsColorHidden.w, path);
    Features::BoxColorHidden.x = GetPrivateProfileFloat(("b"), TEXT("HidBox Color1"), Features::BoxColorHidden.x, path);
    Features::BoxColorHidden.y = GetPrivateProfileFloat(("b"), TEXT("HidBox Color2"), Features::BoxColorHidden.y, path);
    Features::BoxColorHidden.z = GetPrivateProfileFloat(("b"), TEXT("HidBox Color3"), Features::BoxColorHidden.z, path);
    Features::BoxColorHidden.w = GetPrivateProfileFloat(("b"), TEXT("HidBox Color4"), Features::BoxColorHidden.w, path);
    Features::SkeletonColorHidden.x = GetPrivateProfileFloat(("b"), TEXT("HidSkeleton Color1"), Features::SkeletonColorHidden.x, path);
    Features::SkeletonColorHidden.y = GetPrivateProfileFloat(("b"), TEXT("HidSkeleton Color2"), Features::SkeletonColorHidden.y, path);
    Features::SkeletonColorHidden.z = GetPrivateProfileFloat(("b"), TEXT("HidSkeleton Color3"), Features::SkeletonColorHidden.z, path);
    Features::SkeletonColorHidden.w = GetPrivateProfileFloat(("b"), TEXT("HidSkeleton Color4"), Features::SkeletonColorHidden.w, path);

    Features::rTargetingOnly = MyGetPrivateProfileInt(("b"), TEXT("Targeting Only"), Features::rTargetingOnly, path);
    Features::rWeaponname = MyGetPrivateProfileInt(("b"), TEXT("Weapon Name"), Features::rWeaponname, path);
    Features::rPlatformname = MyGetPrivateProfileInt(("b"), TEXT("Platform Name"), Features::rPlatformname, path);
    Features::rUsername = MyGetPrivateProfileInt(("b"), TEXT("Username"), Features::rUsername, path);
    Features::rRanked = MyGetPrivateProfileInt(("b"), TEXT("Rank"), Features::rRanked, path);
    Features::rKills = MyGetPrivateProfileInt(("b"), TEXT("Kills"), Features::rKills, path);
    Features::rLevel = MyGetPrivateProfileInt(("b"), TEXT("Level"), Features::rLevel, path);
    Features::rFovRadar = MyGetPrivateProfileInt(("b"), TEXT("FOV Arrows"), Features::rFovRadar, path);
    currentSnapline = MyGetPrivateProfileInt(("b"), TEXT("Snapline Position"), currentSnapline, path);
    Features::rMaxDistance = GetPrivateProfileFloat(("b"), TEXT("ESP Distance"), Features::rMaxDistance, path);
}

void testmenu() {
    static bool show_demo_window = true;
    static bool show_another_window = false;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 255.00f);

    if (GetAsyncKeyState(VK_INSERT) & 1) rMenu = !rMenu;

    if (!Features::rGunConfigs) {
        currentGun = 5;
        Features::rSmooth = gunSettings[5].smoothing;
        Features::rFovSize = gunSettings[5].fovSize;
        Features::rADSFovSize = gunSettings[5].adsFovSize;
    }

    if (rMenu) {
        DWORD window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoBackground;

        ImGui::Begin("Menu", &rMenu, window_flags);
        {
            const auto& p = ImGui::GetWindowPos();
            ImDrawList* pDrawList = ImGui::GetWindowDrawList();

            ImGui::GetBackgroundDrawList()->AddRectFilled(
                ImVec2(p.x, p.y),
                ImVec2(p.x + 475, p.y + 545),  
                ImColor(0.15f, 0.15f, 0.15f, 1.0f), 
                15.0f 
            );

            ImGui::GetBackgroundDrawList()->AddRect(
                ImVec2(p.x, p.y),
                ImVec2(p.x + 480, p.y + 550),
                ImColor(0.9f, 0.9f, 0.9f, 1.0f),  
                15.0f, 0, 2.0f  
            );

            ImGui::BeginChild("Tabs", ImVec2(140, 530), true);  
            {
                ImGui::SetCursorPosX(7);
                if (ImGui::Button("Aim", ImVec2(120, 40))) { tab_count = 0; active = true; }  
                ImGui::SetCursorPosX(7);
                if (ImGui::Button("ESP", ImVec2(120, 40))) { tab_count = 1; active = true; }
                ImGui::SetCursorPosX(7);
                if (ImGui::Button("Config", ImVec2(120, 40))) { tab_count = 2; active = true; }
            }
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("Main", ImVec2(320, 530), true, ImGuiWindowFlags_NoScrollbar);  
            {
                if (active) {
                    size_child = min(size_child + 1.0f / ImGui::GetIO().Framerate * 60.0f, 10.0f);
                    if (size_child == 10.0f) { active = false; tabs = tab_count; }
                }
                else {
                    size_child = max(size_child - 1.0f / ImGui::GetIO().Framerate * 60.0f, 0.0f);
                }

                switch (tabs) {
                case 0: 
                    ImGui::Checkbox("Enable Aimbot", &Features::rAimbot);
                    ImGui::Checkbox("Enable Triggerbot", &Features::rTriggerbot);
                    ImGui::Checkbox("Enable Prediction", &Features::rPrediction);
                    ImGui::Checkbox("Visible Check", &Features::rVisibleCheck);
                    ImGui::Checkbox("Lock Target", &Features::rLockTarget);
                    ImGui::Checkbox("Draw Fov Circle", &Features::rFovCircle);
                    ImGui::Checkbox("Draw Aimline", &Features::rAimline);
                    ImGui::Checkbox("Ignore Downed", &Features::rDownedCheck);
                    ImGui::Checkbox("Ignore Bots", &Features::rBotCheck);
                    ImGui::Combo("Target", &currentTarget, targetOptions, IM_ARRAYSIZE(targetOptions));
                    ImGui::Text("Aimbot Key:     "); ImGui::SameLine(); HotkeyButton(Features::rAimkey, ChangeKey, keystatus);
                    ImGui::Text("Triggerbot Key: "); ImGui::SameLine(); HotkeyButton(Features::rTrigkey, ChangeKeyTrig, keystatusTrig);
                    ImGui::Checkbox("Gun Configs", &Features::rGunConfigs);
                    if (Features::rGunConfigs) ImGui::Combo("Gun", &currentGun, gunOptions, IM_ARRAYSIZE(gunOptions));
                    ImGui::SliderFloat("Smoothing", &gunSettings[currentGun].smoothing, 1.f, 100.f, "%.3f");
                    ImGui::SliderInt("Fov Size", &gunSettings[currentGun].fovSize, 10, 600);
                    ImGui::SliderInt("ADS Fov Size", &gunSettings[currentGun].adsFovSize, 10, 600);
                    break;

                case 1: 
                    ImGui::Combo("Box Type", &currentBox, boxOptions, IM_ARRAYSIZE(boxOptions));
                    ImGui::Checkbox("Skeleton", &Features::rSkeleton);
                    ImGui::Checkbox("Chams", &Features::rChams);
                    ImGui::Checkbox("Wireframe", &Features::rWireframe);
                    ImGui::Checkbox("View Angle", &Features::rViewAngle);
                    ImGui::Checkbox("Distance", &Features::rDistance);
                    ImGui::Checkbox("FOV Arrows", &Features::rFovRadar);
                    ImGui::Checkbox("Targeting Only", &Features::rTargetingOnly);
                    ImGui::Checkbox("Weapon Name", &Features::rWeaponname);
                    ImGui::Checkbox("Username", &Features::rUsername);
                    ImGui::Checkbox("Rank", &Features::rRanked);
                    ImGui::Checkbox("Kills", &Features::rKills);
                    ImGui::Checkbox("Level", &Features::rLevel);
                    break;

                case 2: 
                    if (ImGui::Button("Save Config")) {
                        if (!std::filesystem::exists("C:\\red")) {
                            savecfg("C:\\red");
                        }
                        else {
                            int result = MessageBox(
                                nullptr,
                                "Overwrite Config?",
                                "",
                                MB_YESNO | MB_ICONQUESTION
                            );
                            if (result == IDYES) {
                                savecfg("C:\\red");
                            }
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Load Config")) {
                        loadcfg("C:\\red");
                    }
                    break;
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}

static float ProjectileSpeed;
static float ProjectileGravity;

Vector3 PredictPlayerPosition(Vector3 CurrentLocation, float Distance, Vector3 GravityScale, string weapon) {

    Vector3 CalculatedPosition = CurrentLocation;
    if (ProjectileSpeed <= 0.1 || ProjectileGravity <= 0.1) return CalculatedPosition;

    float TimeToTarget = Distance / fabsf(ProjectileSpeed);

    CalculatedPosition.x += (GravityScale.x * TimeToTarget * (60 * ProjectileGravity));
    CalculatedPosition.y += (GravityScale.y * TimeToTarget * (60 * ProjectileGravity));
    CalculatedPosition.z += (GravityScale.z * TimeToTarget);
    CalculatedPosition.z += fabsf(((-ProjectileSpeed / 75) * ProjectileGravity) / 2.0f * (TimeToTarget * 25));

    return CalculatedPosition;
}

namespace SonyDriverHelper {
    class api {
    public:
        static void Init();

        static void MouseMove(float x, float y);

        static bool GetKey(int id);
    };
}


bool IsTargetWithinFOV(const Vector2& target2D, float screenWidth, float screenHeight) {
    float distance = GetCrossDistance(target2D.x, target2D.y, screenWidth / 2, screenHeight / 2);
    return distance <= Features::rFovSize && target2D.x != 0 && target2D.y != 0;
}

ImVec2 CalculateAimOffset(const Vector2& target2D, const ImVec2& screenCenter, int aimSpeed, float screenWidth, float screenHeight) {
    float targetX = 0.0f;
    float targetY = 0.0f;

    if (target2D.x != 0) {
        targetX = (target2D.x > screenCenter.x) ? -(screenCenter.x - target2D.x) / aimSpeed : (target2D.x - screenCenter.x) / aimSpeed;
        if ((target2D.x > screenCenter.x && targetX + screenCenter.x > screenWidth) ||
            (target2D.x < screenCenter.x && targetX + screenCenter.x < 0)) {
            targetX = 0.0f;
        }
    }

    if (target2D.y != 0) {
        targetY = (target2D.y > screenCenter.y) ? -(screenCenter.y - target2D.y) / aimSpeed : (target2D.y - screenCenter.y) / aimSpeed;
        if ((target2D.y > screenCenter.y && targetY + screenCenter.y > screenHeight) ||
            (target2D.y < screenCenter.y && targetY + screenCenter.y < 0)) {
            targetY = 0.0f;
        }
    }

    return ImVec2(targetX, targetY);
}

void ResetTarget() {
    Cached::closestDistance = FLT_MAX;
    Cached::closestPawn = nullptr;
}

void aimbot(int bone, const std::string& weapon) {
    if (!Cached::closestPawn) return;

    auto mesh = Cached::closestPawn->Mesh();
    if (!mesh) {
        ResetTarget();
        return;
    }

    Vector3 LocalHead3D = Cached::LocalPawn->Mesh()->GetSocketLocation(bone);
    Vector3 Head3D = mesh->GetSocketLocation(bone);
    Vector2 Head2D = Cached::PlayerController->ProjectWorldLocationToScreen(Head3D);

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 screenCenter(io.DisplaySize.x / 2, io.DisplaySize.y / 2);

    if (Features::rAimline) {
        ImGui::GetForegroundDrawList()->AddLine(screenCenter, ImVec2(Head2D.x, Head2D.y), ImGui::ColorConvertFloat4ToU32(Features::AimlineColor), 1.0f);
    }

    auto root = Cached::closestPawn->RootComponent();
    Vector3 velocity = root->GetComponentVelocity();
    float distance = Head3D.Distance(camera.location) / 100.0f;
    Vector3 prediction = Head3D;

    if (!IsTargetWithinFOV(Head2D, io.DisplaySize.x, io.DisplaySize.y)) {
        ResetTarget();
        return;
    }

    float projectileSpeed = read<float>((FortPTR)Cached::CurrentWeapon + 0x1d08);
    float projectileGravity = read<float>((FortPTR)Cached::CurrentWeapon + 0x1d0c);

    if (Features::rPrediction && (projectileSpeed > 0.1f || projectileGravity > 0.1f)) {
        prediction = PredictPlayerPosition(Head3D, distance, velocity, weapon);
        Head2D = Cached::PlayerController->ProjectWorldLocationToScreen(prediction);
    }

    ImVec2 targetOffset = CalculateAimOffset(Head2D, screenCenter, Features::rSmooth, io.DisplaySize.x, io.DisplaySize.y);

    if (Features::rVisibleCheck && !mesh->WasRecentlyRendered(0.06f)) return;

    SonyDriverHelper::api::MouseMove(static_cast<int>(targetOffset.x), static_cast<int>(targetOffset.y));

    if (!Features::rLockTarget) {
        ResetTarget();
    }
}

void triggerbot(string weapon, uintptr_t playercontroller) {
    if (Features::rTriggerbot) {
        if (weapon.find("Shotgun") != string::npos) {
            if (read<uintptr_t>(playercontroller + Offsets::TargetedFortPawn)) {
                Sleep(Features::rTriggerbotDelay);
                mouse_event(MOUSEEVENTF_LEFTDOWN, DWORD(NULL), DWORD(NULL), DWORD(0x0002), ULONG_PTR(NULL));
                mouse_event(MOUSEEVENTF_LEFTUP, DWORD(NULL), DWORD(NULL), DWORD(0x0004), ULONG_PTR(NULL));
            }
        }
    }
}

void weaponconfig(string weapon) {
    if (weapon.find("Pistol") != string::npos || weapon.find("Hand Cannon") != string::npos) {
        Features::rSmooth = gunSettings[0].smoothing;
        Features::rFovSize = gunSettings[0].fovSize;
        Features::rADSFovSize = gunSettings[0].adsFovSize;
    }
    else if (weapon.find("Assault Rifle") != string::npos || weapon.find("AR") != string::npos) {
        Features::rSmooth = gunSettings[1].smoothing;
        Features::rFovSize = gunSettings[1].fovSize;
        Features::rADSFovSize = gunSettings[1].adsFovSize;
    }
     else if (weapon.find("Shotgun") != string::npos) {
        Features::rSmooth = gunSettings[2].smoothing;
        Features::rFovSize = gunSettings[2].fovSize;
        Features::rADSFovSize = gunSettings[2].adsFovSize;
    }
     else if (weapon.find("SMG") != string::npos || weapon.find("Submachine Gun") != string::npos) {
        Features::rSmooth = gunSettings[3].smoothing;
        Features::rFovSize = gunSettings[3].fovSize;
        Features::rADSFovSize = gunSettings[3].adsFovSize;
    }
     else if (weapon.find("Sniper") != string::npos || weapon.find("DMR") != string::npos) {
        Features::rSmooth = gunSettings[4].smoothing;
        Features::rFovSize = gunSettings[4].fovSize;
        Features::rADSFovSize = gunSettings[4].adsFovSize;
    }
   else {
        Features::rSmooth = gunSettings[5].smoothing;
        Features::rFovSize = gunSettings[5].fovSize;
        Features::rADSFovSize = gunSettings[5].adsFovSize;
    }
}

void ActorLoop() {
    Cached::World = read<UWorld*>(va_text + Offsets::UWorld);
    Uworld_Cam = (FortPTR)Cached::World;

    UGameInstance* GameInstance = Cached::World->OwningGameInstance();
    Cached::LocalPlayer = read<ULocalPlayer*>(GameInstance->LocalPlayers());
    Cached::PlayerController = Cached::LocalPlayer->PlayerController();
    Copy_PlayerController_Camera = (FortPTR)Cached::PlayerController;

    Cached::CameraManager = read<uintptr_t>((FortPTR)Cached::PlayerController + Offsets::CameraManager);
    Cached::LocalPawn = (AFortPlayerPawn*)Cached::PlayerController->AcknowledgedPawn();
    Cached::LocalPlayerState = Cached::LocalPawn->PlayerState();
    Cached::LocalRootComponent = Cached::LocalPawn->RootComponent();
    Cached::GameState = Cached::World->GameState();
    if (!Cached::GameState) return;

    Cached::CurrentWeapon = Cached::LocalPawn->CurrentWeapon();
    Cached::Mesh = Cached::LocalPawn->Mesh();

    auto PlayerArray = Cached::GameState->PlayerArray();
    int MyTeamId = Cached::LocalPlayerState->TeamIndex();

    ImVec2 ScreenCenter(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f);

    vector<FortPTR> addressesToRead;
    addressesToRead.reserve(PlayerArray.Count * 2);

    for (int i = 0; i < PlayerArray.Count; i++) {
        AFortPlayerState* PlayerState = reinterpret_cast<AFortPlayerState*>(PlayerArray.Get(i));
        if (!PlayerState) continue;

        AFortPlayerPawn* Player = reinterpret_cast<AFortPlayerPawn*>(PlayerState->PawnPrivate());
        if (!Player || Player == Cached::LocalPawn) continue;

        addressesToRead.push_back((FortPTR)Player + 0x6d0); // despawning flag
        addressesToRead.push_back((FortPTR)PlayerState + 0x29A); // bot check
    }

    vector<char> batchData = batch_read<char>(addressesToRead);

    size_t batchIndex = 0;

    for (int i = 0; i < PlayerArray.Count; i++) {
        AFortPlayerState* PlayerState = reinterpret_cast<AFortPlayerState*>(PlayerArray.Get(i));
        if (!PlayerState) continue;

        AFortPlayerPawn* Player = reinterpret_cast<AFortPlayerPawn*>(PlayerState->PawnPrivate());
        if (!Player || Player == Cached::LocalPawn) continue;

        USkeletalMeshComponent* Mesh = Player->Mesh();
        if (!Mesh) continue;

        int ActorTeamId = PlayerState->TeamIndex();
        if (MyTeamId && ActorTeamId == MyTeamId) continue;

        char despawning = batchData[batchIndex++];
        char botCheck = batchData[batchIndex++];

        if (despawning >> 4) continue;

        Vector3 LocalHead3D = Cached::Mesh->GetSocketLocation(110);
        Vector3 Head3D = Mesh->GetSocketLocation(110);
        Vector2 Head2D = Cached::PlayerController->ProjectWorldLocationToScreen(Head3D);
        Vector3 Neck3D = Mesh->GetSocketLocation(67);
        Vector2 Neck2D = Cached::PlayerController->ProjectWorldLocationToScreen(Neck3D);
        Vector3 Chest3D = Mesh->GetSocketLocation(2);
        Vector2 Chest2D = Cached::PlayerController->ProjectWorldLocationToScreen(Chest3D);
        Vector3 Pelvis3D = Mesh->GetSocketLocation(7);
        Vector2 Pelvis2D = Cached::PlayerController->ProjectWorldLocationToScreen(Pelvis3D);
        Vector3 Bottom3D = Mesh->GetSocketLocation(0);
        Vector2 Bottom2D = Cached::PlayerController->ProjectWorldLocationToScreen(Bottom3D);

        float BoxHeight = abs(Head2D.y - Bottom2D.y) * 1.2f;
        float CornerHeight = BoxHeight; 
        float CornerWidth = BoxHeight * 0.5f;

        float distance = Head3D.Distance(camera.location) / 100;

        if (distance > Features::rMaxDistance) continue;

        auto crossdist = GetCrossDistance(Head2D.x, Head2D.y, screenWidth / 2, screenHeight / 2);
        Vector2 Center = Head2D;
        Center.y = (Head2D.y + Bottom2D.y) / 2.0f;

        bool isRecentlyRendered = Mesh->WasRecentlyRendered(0.06f);
        auto visibilityColor = isRecentlyRendered
            ? make_tuple(Features::SnaplineColorVis, Features::BoxColorVis, Features::SkeletonColorVis, Features::FillColorVis, Features::DistanceColorVis, Features::UsernameColorVis, Features::PlatformColorVis, Features::KillsColorVis, Features::LevelsColorVis, Features::WeaponColorVis, Features::RadarColorVis, Features::FovArrowsColorVis)
            : make_tuple(Features::SnaplineColorHidden, Features::BoxColorHidden, Features::SkeletonColorHidden, Features::FillColorHidden, Features::DistanceColorHidden, Features::UsernameColorHidden, Features::PlatformColorHidden, Features::KillsColorHidden, Features::LevelsColorHidden, Features::WeaponColorHidden, Features::RadarColorHidden, Features::FovArrowsColorHidden);

        auto [SnaplinesColor, BoxColor, SkeletonColor, FillColor, DistanceColor, UsernameColor, PlatformColor, KillsColor, LevelsColor, WeaponColor, RadarColor, FovArrowColor] = visibilityColor;

        ImDrawList* draw_list = ImGui::GetForegroundDrawList();
        ImFont* font = ImGui::GetIO().Fonts->Fonts[0];
        float textSpacing = 12.0f;
        float textYPos = Bottom2D.y + 2;
        float multiplier = max(1.0f, 1.0f + distance * 0.025f);
        float topTextYPos = Center.y - (CornerHeight / 2) * multiplier;

        auto draw_centered_text = [&](const std::string& text, ImColor color, float yPos) { draw_list->AddText(font, 12.f, ImVec2(Center.x - (font->CalcTextSizeA(12.f, FLT_MAX, 0.0f, text.c_str()).x / 2), yPos), color, text.c_str()); };

        if (Features::rGunConfigs && Cached::CurrentWeapon) {
            weaponconfig(lazy::GetWeaponName((FortPTR)Cached::LocalPawn));
        }

        if (Features::rFovCircle) {
            bool isTargeting = read<bool>((FortPTR)Cached::LocalPawn + 0x1e34);
            Features::rFovDrawSize = isTargeting ? Features::rADSFovSize : Features::rFovSize;

            ImVec2 screenCenter(screenWidth / 2, screenHeight / 2);
            draw_list->AddCircle(screenCenter, Features::rFovDrawSize, ImGui::ColorConvertFloat4ToU32(Features::FovColor), 64);
            draw_list->AddCircleFilled(screenCenter, Features::rFovDrawSize, ImGui::ColorConvertFloat4ToU32(Features::FovFillColor), 64);
        }

        if (currentBox == 1) {
            draw::FullBox(Center.x - (CornerWidth / 2), Center.y - (CornerHeight / 2), CornerWidth, CornerHeight, BoxColor, 1.6f);
            draw::DrawFill(Center.x - (CornerWidth / 2), Center.y - (CornerHeight / 2), CornerWidth, CornerHeight, FillColor, 1.6f);
        }
        else if (currentBox == 2) {
            draw::DrawCornerBox(Center.x - (CornerWidth / 2), Center.y - (CornerHeight / 2), CornerWidth, CornerHeight, BoxColor, 1.6f);
            draw::DrawFill(Center.x - (CornerWidth / 2), Center.y - (CornerHeight / 2), CornerWidth, CornerHeight, FillColor, 1.6f);
        }

        if (Features::rSkeleton) {
            draw::DrawSkeleton(Mesh, distance, SkeletonColor);
        }

        if (Features::rViewAngle) {
            float playerAngle = camera.rotation.y * (PI / 180.0f);
            float fovAngle = camera.fov * (PI / 180.0f);

            ImVec2 direction(cos(playerAngle - fovAngle), sin(playerAngle - fovAngle));
            direction = ImVec2(direction.x / sqrt(direction.x * direction.x + direction.y * direction.y),
                direction.y / sqrt(direction.x * direction.x + direction.y * direction.y));

            float lineLength = 175.0f / distance;
            ImVec2 lineEnd(Head2D.x + lineLength * direction.x, Head2D.y + lineLength * direction.y);

            draw_list->AddLine(ImVec2(Head2D.x, Head2D.y), lineEnd, ImGui::ColorConvertFloat4ToU32(SkeletonColor), 1.5f);
        }

        if (read<BYTE>((FortPTR)Player + 0x982) >> 4 & 1) {
            continue;
        }

        bool bot = botCheck >> 3 & 1;

        if (Features::rFovRadar && !bot) {
            draw::addtofovradar(Head3D, 187, isRecentlyRendered, ScreenCenter, FovArrowColor);
        }

        ImVec2 center(screenWidth / 2, screenHeight / 2);
        if (!bot) {
            if (currentSnapline == 1) draw_list->AddLine(ImVec2(screenWidth / 2, screenHeight - 5), ImVec2(Bottom2D.x, Bottom2D.y), ImColor(SnaplinesColor), 1); // bottom
            else if (currentSnapline == 2) draw_list->AddLine(ImVec2(screenWidth / 2, 0), ImVec2(Head2D.x, Head2D.y), ImColor(SnaplinesColor), 1); // top
            else if (currentSnapline == 3) draw_list->AddLine(ImVec2(center.x, center.y), ImVec2(Head2D.x, Head2D.y), ImColor(SnaplinesColor), 1); // center
        }

        if (Features::rDistance) {
            draw_centered_text("[" + to_string((int)distance) + " m]", DistanceColor, textYPos);
            textYPos += textSpacing;
        }

        bool knocked = read<BYTE>((FortPTR)Player + 0x982) >> 4 & 1;
        if ((!Features::rDownedCheck || !knocked) && crossdist < Features::rFovSize && crossdist < Cached::closestDistance) {
            if (!Features::rBotCheck || !bot) {
                Cached::closestDistance = crossdist;
                Cached::closestPawn = Player;
            }
        }

        if (Features::rTargetingOnly && Player != Cached::closestPawn) continue;

        if (Features::rRanked) {
            int32_t RankProgress = read<int32_t>(read<uintptr_t>((FortPTR)PlayerState + 0x9a8) + 0xb8 + 0x10);
            draw_centered_text(lazy::GetRank(RankProgress), lazy::GetRankColor(RankProgress), topTextYPos);
            topTextYPos += textSpacing;
        }

        if (Features::rUsername || Features::rPlatformname) {
            string nameText;
            if (Features::rUsername) nameText = lazy::GetPlayerName((FortPTR)PlayerState);
            if (Features::rPlatformname) {
                if (!nameText.empty()) nameText += " (";
                nameText += lazy::GetPlatformName((FortPTR)PlayerState);
                if (!nameText.empty()) nameText += ")";
            }
            draw_centered_text(nameText, UsernameColor, topTextYPos);
            topTextYPos += textSpacing;
        }

        if (Features::rWeaponname) {
            auto CurrentWeapon = read<uint64_t>((FortPTR)Player + Offsets::CurrentWeapon);
            string weaponName = lazy::GetWeaponName((FortPTR)Player) + " [" + to_string(read<uint64_t>(CurrentWeapon + Offsets::AmmoCount)) + "]";
            draw_centered_text(weaponName, WeaponColor, textYPos);
            textYPos += textSpacing;
        }

        if (Features::rKills || Features::rLevel) {
            string combinedInfo;
            if (Features::rKills) combinedInfo = "Kills: " + to_string(read<int>((FortPTR)PlayerState + Offsets::KillScore));
            if (Features::rLevel) {
                if (!combinedInfo.empty()) combinedInfo += " | ";
                combinedInfo += "Level: " + to_string(read<int>((FortPTR)PlayerState + Offsets::SeasonLevelUIDisplay));
            }
            draw_centered_text(combinedInfo, KillsColor, textYPos);
            textYPos += textSpacing;
        }

        if (Features::rChams) {
            int DepthStencil = 11;

            auto SkeletalMeshes = read<TArray>((FortPTR)Player + 0x5c90);
            auto AllMaterials = read<TArray>((FortPTR)Player + 0x5ca0);

            if (SkeletalMeshes.isValid()) {
                int skeletalMeshSize = SkeletalMeshes.size();
                for (int i = 0; i < skeletalMeshSize; i++) {
                    auto skeletal_component = SkeletalMeshes[i];
                    if (skeletal_component != 0 && is_valid(skeletal_component)) {
                        uint8_t render_custom_depth = read<uint8_t>((FortPTR)skeletal_component + 0x251);
                        if (render_custom_depth) {
                            render_custom_depth |= (1 << 5);
                            if (DepthStencil != 3)
                                write<uint8_t>((FortPTR)skeletal_component + 0x251, render_custom_depth);
                        }

                        uint8_t replication_flag = read<uint8_t>((FortPTR)skeletal_component + 0x88);
                        if (replication_flag) {
                            replication_flag |= (1 << 4);
                            write<uint8_t>((FortPTR)skeletal_component + 0x88, replication_flag);
                        }
                        write<int32_t>((FortPTR)skeletal_component + 0x278, DepthStencil);
                    }
                }
            }

            if (AllMaterials.isValid()) {
                int allMaterialsSize = AllMaterials.size();
                for (int i = 0; i < allMaterialsSize; i++) {
                    auto material_instance = AllMaterials[i];
                    if (material_instance != 0 && is_valid(material_instance)) {
                        uintptr_t Parent = material_instance;

                        for (int j = 0; j < 3; ++j) {
                            if (!Parent || !is_valid(Parent)) {
                                break;
                            }
                            Parent = read<uintptr_t>(Parent + 0x100);
                        }

                        if (Parent && is_valid(Parent)) {
                            auto disable_depth_test_ptr = (Parent + 0x1a0);
                            if (disable_depth_test_ptr) {
                                uint8_t disable_depth_test = read<uint8_t>(disable_depth_test_ptr);
                                if (disable_depth_test) {
                                    disable_depth_test |= (1 << 0);
                                    write<uint8_t>(disable_depth_test_ptr, disable_depth_test);
                                }
                            }
                        }
                        if (Features::rWireframe) {
                            auto wireframe_ptr = (FortPTR)(Parent + 0x1b0);
                            if (is_valid(wireframe_ptr)) {
                                uint8_t bwireframe = read<uint8_t>(wireframe_ptr);
                                if (bwireframe) {
                                    bwireframe |= (1 << 6);
                                    write<uint8_t>(wireframe_ptr, bwireframe);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Render() {
    if (GetAsyncKeyState(Features::rAimkey)) {
        int boneID = 0;

        switch (currentTarget) {
        case 0: boneID = 110; break; // Head
        case 1: boneID = 67;  break; // Neck
        case 2: boneID = 37;  break; // Chest
        default: break;
        }

        if (boneID != 0) {
            aimbot(boneID, lazy::GetWeaponName((FortPTR)(Cached::LocalPawn)));
        }
    }

    if (GetAsyncKeyState(Features::rTrigkey) && Features::rTriggerbot) {
        triggerbot(lazy::GetWeaponName((FortPTR)(Cached::LocalPawn)), (FortPTR)(Cached::PlayerController));
    }

    ActorLoop();
}

WPARAM MainLoop() {
    static RECT old_rc;
    ZeroMemory(&Message, sizeof(MSG));

    while (Message.message != WM_QUIT) {
        if (PeekMessage(&Message, MyHwnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        HWND hwnd_active = GetForegroundWindow();
        SetWindowPos(hwnd_active, MyHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        if (hwnd_active == GameHwnd) {
            HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
            SetWindowPos(MyHwnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        RECT rc;
        POINT xy;
        ZeroMemory(&rc, sizeof(RECT));
        ZeroMemory(&xy, sizeof(POINT));
        GetClientRect(GameHwnd, &rc);
        ClientToScreen(GameHwnd, &xy);
        rc.left = xy.x;
        rc.top = xy.y;

        ImGuiIO& io = ImGui::GetIO();
        io.ImeWindowHandle = GameHwnd;
        io.DeltaTime = 1.0f / 60.0f;

        POINT p;
        GetCursorPos(&p);
        io.MousePos.x = p.x - xy.x;
        io.MousePos.y = p.y - xy.y;

        if (GetAsyncKeyState(0x1)) {
            io.MouseDown[0] = true;
            io.MouseClicked[0] = true;
            io.MouseClickedPos[0].x = io.MousePos.x;
            io.MouseClickedPos[0].x = io.MousePos.y;
        }
        else {
            io.MouseDown[0] = false;
        }

        if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom) {
            old_rc = rc;

            ScreenWidth = rc.right;
            ScreenHeight = rc.bottom;

            p_Params.BackBufferWidth = ScreenWidth;
            p_Params.BackBufferHeight = ScreenHeight;
            SetWindowPos(MyHwnd, (HWND)0, xy.x, xy.y, ScreenWidth, ScreenHeight, SWP_NOREDRAW);
            p_Device->Reset(&p_Params);
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Render(); 
        testmenu();

        ImGui::EndFrame();

        p_Device->SetRenderState(D3DRS_ZENABLE, false);
        p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
        p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
        p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
        if (p_Device->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            p_Device->EndScene();
        }

        HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

        if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            p_Device->Reset(&p_Params);
            ImGui_ImplDX9_CreateDeviceObjects();
        }
    }
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupD3D();
    DestroyWindow(MyHwnd);

    return Message.wParam;
}

int main() {
    if (!handler::find_driver()) {
        MessageBox(NULL, "Error loading driver!", "", NULL);
        exit(0);
    }

    SonyDriverHelper::api::Init();

    HWND hwnd = NULL;
    while (!hwnd) {
        system("cls");
        cout << "Waiting for game..." << endl;
        hwnd = FindWindowA(NULL, "Fortnite  ");
        if (hwnd) break;
        Sleep(3000);
    }
    system("cls");

    handler::process_id = handler::find_process("FortniteClient-Win64-Shipping.exe"); // FortniteClient-Win64-Shipping

    auto virtualaddy = handler::find_image();
    auto cr3 = handler::fetch_cr3();
    for (auto i = 0; i < 25; i++) {
        if (read<__int32>(virtualaddy + (i * 0x1000) + 0x250) == 0x6F41C00) {
            va_text = virtualaddy + ((i + 1) * 0x1000);
        }
    }

    std::cout << "Base Address -> " << virtualaddy << std::endl;
    std::cout << "CR3 -> " << cr3 << std::endl;
    std::cout << "VAText -> " << va_text << std::endl;

    gunSettings.resize(sizeof(gunOptions), {5.0f, 100, 100});
    InitializeDirectXOverlay();
    MainLoop();
}