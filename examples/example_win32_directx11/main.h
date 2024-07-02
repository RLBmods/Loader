#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include "Fonts.h"
#include "images.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui_settings.h"

#include <d3d11.h>
#include <string>
#include "../Mega loader/examples/Include/D3DX11.h"
#pragma comment (lib, "d3dx11.lib")

static std::string msg_text;

ID3D11ShaderResourceView* menu_bg;
ID3D11ShaderResourceView* avatar;
ID3D11ShaderResourceView* logo;
ID3D11ShaderResourceView* image[6];

static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

float calc_distance(ImVec2 p1, ImVec2 p2)
{
    const float dx = p2.x - p1.x;
    const float dy = p2.y - p1.y;

    return sqrt((dx * dx) + (dy * dy));
}

static bool image_loaded = false;

void LoadImages()
{
    if (image_loaded)
        return;

    D3DX11_IMAGE_LOAD_INFO iInfo;
    ID3DX11ThreadPump* threadPump{ nullptr };

    if (menu_bg == nullptr)D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"E:\\Images\\anime.jpg", &iInfo, threadPump, &menu_bg, 0);
    if (avatar == nullptr)D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"E:\\LoaderImage\\avatar.jpg", &iInfo, threadPump, &avatar, 0);

    if (image[0] == nullptr)D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"E:\\Images\\image1.jpg", &iInfo, threadPump, &image[0], 0);
    if (image[1] == nullptr)D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"E:\\Images\\image2.jpg", &iInfo, threadPump, &image[1], 0);
    if (image[2] == nullptr)D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"E:\\Images\\image3.jpg", &iInfo, threadPump, &image[2], 0);
    if (image[3] == nullptr)D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"E:\\Images\\image4.jpg", &iInfo, threadPump, &image[3], 0);
    if (image[4] == nullptr)D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"E:\\Images\\image5.jpg", &iInfo, threadPump, &image[4], 0);
    if (image[5] == nullptr)D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"E:\\Images\\image6.jpg", &iInfo, threadPump, &image[5], 0);

    D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, logotype, sizeof(logotype), &iInfo, threadPump, &logo, 0);
    image_loaded = true;
}


float rand__float(float min, float max)
{
    return min + float(rand() / float(RAND_MAX)) * (max - min);
}


void LoadingBackground(ImVec2 p)
{
    if (loading_radius > 3.f) {
        for (int a = 0; a < 55; a++) {
            for (int i = 0; i < 85; i++) {
                if (calc_distance(p + ImVec2(i * 10, a * 10), p + ImVec2(800, 0)) < loading_radius + 50.f && calc_distance(p + ImVec2(i * 10, a * 10), p + ImVec2(800, 0)) > loading_radius) {
                    ImGui::GetWindowDrawList()->AddCircleFilled(p + ImVec2(i * 10, a * 10), 3.f, main_color - (ImU32)ImColor(0.f, 0.f, 0.f, (calc_distance(p + ImVec2(i * 10, a * 10), p + ImVec2(800, 0)) + 25 - loading_radius) / 90.f), 60.f);
                }
            }
        }
    }
}

static ImVec2 rain_pos[100];
static float rain_speed[100];

int ImDialogBoxBase(ImVec2 ps)
{
    if (!msg_text[0])
        return 0;

    ImGui::SetNextWindowPos(ps + ImVec2{ 730 * 0.5f, 500 * 0.5f }, ImGuiCond_Once, { 0.5f, 0.5f });
    ImGui::SetNextWindowSize({ 360.f , 144.f });

    ImGui::Begin("##unload_window", nullptr, ImGuiWindowFlags_NoDecoration);


        ImVec2 p = ImGui::GetWindowPos();
    auto draw_list = ImGui::GetWindowDrawList();

    draw_list->AddImage(menu_bg, p, p + ImVec2(730, 500), ImVec2(0,0), ImVec2(1,1), main_color);
    draw_list->AddRectFilled(p, p + ImVec2(800, 500), ImColor(0.05f, 0.05f, 0.05f, 0.9f));

    draw_list->AddRectFilled({ p.x, p.y + 74.f }, { p.x + 360.f, p.y + 144.f }, ImColor(31, 32, 34, 50), 0.f);

    draw_list->AddText({ p.x + 16.f, p.y + 16.f }, ImColor(234, 238, 245, 255),
        u8"Notification");

    draw_list->AddText({ p.x + 16.f, p.y + 39.f }, ImColor(179, 183, 195, 255),
        msg_text.c_str());

    ImGui::SetCursorPos({ 16.f, 86.f });

    if (ImGui::Button(u8"Îê", { 330.f, 42.f })) {
        msg_text = "";
    }

    ImGui::End();
}

void animated_rain(ImVec2 p)
{
    for (int i = 0; i < 100; i++)
    {
        if (rain_pos[i].y == 0)
        {
            rain_pos[i].x = rand__float(5, 790);
            rain_speed[i] = rand__float(50, 100);
        }

        rain_pos[i].y += rain_speed[i] * ImGui::GetIO().DeltaTime * 6.f;

        ImGui::GetWindowDrawList()->AddCircleFilled( p + rain_pos[i], 1.5f, ImColor(255, 161, 0, 200));

        if (rain_pos[i].y > 500)
        {
            rain_pos[i] = ImVec2(0, 0);
        }
    }
}

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
