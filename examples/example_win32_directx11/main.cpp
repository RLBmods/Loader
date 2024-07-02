#include "main.h"

#include "uxtheme.h"
#include "dwmapi.h"


static bool checkbox = false;

char login[64];
char password[64];

static int menu_state = 0;

static bool active_notifis[15];

static int iTabs;
static int iProduct;

static bool have_notifications;

static int msg_box_return;

float select_offset;

static float inject_offset;
static int inject_status;
static float inject_loading;

HWND hwnd;
RECT rc;

ImVec2 menu_size = ImVec2(730, 500);

void move_window() {

    ImGui::SetCursorPos(ImVec2(0, 0));
    if (ImGui::InvisibleButton("Move_detector", ImVec2(730, 500)));
    if (ImGui::IsItemActive()) {

        GetWindowRect(hwnd, &rc);
        MoveWindow(hwnd, rc.left + ImGui::GetMouseDragDelta().x, rc.top + ImGui::GetMouseDragDelta().y, 730, 500, TRUE);
    }
}
const char* product_name[6] = { "Valorant", "Cyberpunk", "CS GO", "RUST", "Minecraft", "Fortnite" };


int main(int, char**)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    ::RegisterClassExW(&wc);
    hwnd = CreateWindowEx(NULL, wc.lpszClassName, L"Example", WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) / 2) - (menu_size.x / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (menu_size.y / 2), menu_size.x, menu_size.y, 0, 0, 0, 0);

    SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    MARGINS margins = { -1 };

    DwmExtendFrameIntoClientArea(hwnd, &margins);

    POINT mosue;
    rc = { 0 };
    GetWindowRect(hwnd, &rc);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.Fonts->AddFontFromMemoryTTF(&RobotoRegular, sizeof RobotoRegular, 22, NULL, io.Fonts->GetGlyphRangesCyrillic());

    big_font = io.Fonts->AddFontFromMemoryTTF(&RobotoRegular, sizeof RobotoRegular, 25, NULL, io.Fonts->GetGlyphRangesCyrillic());

    logo_font = io.Fonts->AddFontFromMemoryTTF(&BrunoAceRegular, sizeof BrunoAceRegular, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ubuntu_font = io.Fonts->AddFontFromMemoryTTF(&UbuntuRegular, sizeof UbuntuRegular, 20, NULL, io.Fonts->GetGlyphRangesCyrillic());

    bold_font = io.Fonts->AddFontFromMemoryTTF(&RalewayExtraBold, sizeof RalewayExtraBold, 35, NULL, io.Fonts->GetGlyphRangesCyrillic());

    icon_font = io.Fonts->AddFontFromMemoryTTF(&icomoon, sizeof icomoon, 25, NULL, io.Fonts->GetGlyphRangesCyrillic());

    tabs_font = io.Fonts->AddFontFromMemoryTTF(&RalewaySemiBold, sizeof RalewaySemiBold, 18, NULL, io.Fonts->GetGlyphRangesCyrillic());

    descript_font = io.Fonts->AddFontFromMemoryTTF(&RalewaySemiBold, sizeof RalewaySemiBold, 25, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f);

    ImGuiStyle& s = ImGui::GetStyle();

    s.FramePadding = ImVec2(0,0);
    s.WindowPadding = ImVec2(0, 0);
    s.FrameRounding = 1.f;
    s.WindowRounding = 4.f;
    s.WindowBorderSize = 0.f;
    s.PopupBorderSize = 0.f;
    s.WindowPadding = ImVec2(0, 0);
    s.ChildBorderSize = 10;
    s.ItemSpacing = ImVec2(0, 5);
    s.WindowShadowSize = 0;

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            LoadImages();
            ImGui::SetNextWindowSize(ImVec2(730, 500));
            ImGui::Begin("General", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
            {
                auto draw = ImGui::GetWindowDrawList();
                const auto& p = ImGui::GetWindowPos();

                draw->AddImage(menu_bg, p, p + ImVec2(800, 500), ImVec2(0, 0), ImVec2(1, 1), main_color);

                draw->AddImage(menu_bg, p, p + ImVec2(800, 500), ImVec2(0, 0), ImVec2(1, 1), main_color);
                draw->AddRectFilled(p, p + ImVec2(800, 500), ImColor(0.05f, 0.05f, 0.05f, 0.9f));

                ImRect logotype_bb(p, p + ImVec2(730, 250));

                LoadingBackground(p);

                animated_rain(p);

                if (menu_state < 2)
                {
                    draw->AddImage(logo, logotype_bb.GetCenter() - ImVec2(80, 80), logotype_bb.GetCenter() + ImVec2(80, 80), ImVec2(0, 0), ImVec2(1, 1), ImColor(1.f, 1.f, 1.f, 1.f));

                    ImGui::SetCursorPos(ImVec2(165, 200));
                    ImGui::BeginGroup();

                        ImGui::InputTextEx("Login", NULL, login, 64, ImVec2(400, 40), 0);
                        ImGui::InputTextEx("Password", NULL, password, 64, ImVec2(400, 40), ImGuiInputTextFlags_Password);

                    if (ImGui::Button("SIGN IN", ImVec2(400, 40)))
                        menu_state = 1;

                    loading_radius = ImLinearSweep(loading_radius, menu_state == 1 ? 1100 : 0.f, ImGui::GetIO().DeltaTime * 500.f);

                    if (loading_radius == 1100)
                        loading_radius = 0;

                    if (ImGui::IsKeyDown(ImGuiKey_0)) {
                        menu_state = 2;
                        msg_text = "Successful autorization";
                        loading_radius = 1100;
                    }

                    ImGui::EndGroup();
                }
                if (menu_state >= 2)
                {
                    draw->AddText(logo_font, 40, p + ImVec2(0, 0), ImColor(1.f, 1.f, 1.f, 1.f), "WEXIZE");

                    draw->AddRectFilled(p + ImVec2(180, 0), p + ImVec2(181, 500), second_color);

                    draw->AddLine(p + ImVec2(0, 440), p + ImVec2(180, 440), second_color);

                    draw->AddImageRounded(avatar, p + ImVec2(10, 450), p + ImVec2(50, 490), ImVec2(0, 0), ImVec2(1, 1), ImColor(1.f, 1.f, 1.f, 1.f), 360);

                    draw->AddText(p + ImVec2(60, 450), ImColor(1.f, 1.f, 1.f, 1.f), "Lyapos");
                    draw->AddText(ubuntu_font, 18, p + ImVec2(60, 470), ImColor(0.6f, 0.6f, 0.6f, 1.f), "#1337");

                    draw->AddCircleFilled(p + ImVec2(45, 485), 7.f, second_color, 360);
                    draw->AddCircleFilled(p + ImVec2(45, 485), 4.f, ImColor(0.f, 1.f, 0.f, 1.f), 360);

                    ImGui::SetCursorPos(ImVec2(10, 60));
                    ImGui::BeginChild("Tabs", { 170, 300 });

                    ImGui::Tab("k", "not", &iTabs, 0);
                    ImGui::Tab("j", "Products", &iTabs, 1);
                    ImGui::Tab("l", "Settings", &iTabs, 2);
                    
                    ImGui::EndChild();
                }

                if (menu_state == 2) {
                    if (iTabs == 0) {
                        ImGui::SetCursorPos(ImVec2(190, 10));
                        ImGui::BeginChild("News");

                        if (ImGui::News(image[0], "Updating the cheat for cyberpunk", "Click for more information", &active_notifis[0])) msg_text = "we will redirect you to https://...";
                        ImGui::News(image[1], "Updating the cheat for valorant", "Click for more information", &active_notifis[1]);
                        ImGui::News(image[2], "Updating the cheat for cs go", "Click for more information", &active_notifis[2]);
                        ImGui::News(image[3], "Updating the cheat for rust", "Click for more information", &active_notifis[3]);
                        ImGui::News(image[4], "Updating the cheat for minecraft", "Click for more information", &active_notifis[4]);

                        have_notifications = ImGui::GetCurrentWindow()->ContentSize.y == 0;

                        if (have_notifications)
                        {
                            std::string empty = "There will be something here soon.";
                            ImGui::GetWindowDrawList()->AddText(p + ImVec2(460, 250) - ImGui::CalcTextSize("Your notifications will be here, they are not there yet") / 2, ImColor(1.f, 1.f, 1.f, 1.f), "Your notifications will be here, they are not there yet");
                        }
                        ImGui::EndChild();
                    }

                    if (iTabs == 1) {
                        select_offset = ImLerp(select_offset, iProduct != 0 ? 500.f : 0.f, ImGui::GetIO().DeltaTime * 5.f);

                        if (select_offset > 490 && iProduct != 0)
                            menu_state = 3;

                        ImGui::SetCursorPos(ImVec2(190, 15 + select_offset));
                        ImGui::BeginGroup();

                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
                        ImGui::Product(image[0], "Valorant", &iProduct, 1); ImGui::SameLine(270);
                        ImGui::Product(image[1], "Cyberpunk", &iProduct, 2);

                        ImGui::Product(image[2], "CS GO", &iProduct, 3); ImGui::SameLine(270);
                        ImGui::Product(image[3], "RUST", &iProduct, 4);

                        ImGui::Product(image[4], "Minecraft", &iProduct, 5); ImGui::SameLine(270);
                        ImGui::Product(image[5], "Fortnite", &iProduct, 6);
                        ImGui::PopStyleVar();

                        ImGui::EndGroup();
                    }
                }
                if(menu_state == 3)
                {
                    select_offset = ImLerp(select_offset, iProduct > 0 ? 0.f : 500.f, ImGui::GetIO().DeltaTime * 5.f);
                    inject_offset = ImLerp(inject_offset, inject_status == 1 ?  0.f : 200.f, ImGui::GetIO().DeltaTime * 5.f);
                    inject_loading = ImLerp(inject_loading, inject_offset < 10.f ? 510 : 0.f, ImGui::GetIO().DeltaTime * 2.f);

                    draw->AddImageRounded(image[iProduct - 1], p + ImVec2(190, 15 - select_offset), p + ImVec2(720, 190 - select_offset), ImVec2(0, 0.f), ImVec2(1, 0.6f), ImColor(1.f, 1.f, 1.f, 1.f), 4.f);
                    draw->AddRectFilled(p + ImVec2(190, 15 - select_offset), p + ImVec2(720, 190 - select_offset), ImColor(0.05f, 0.05f, 0.05f, 0.6f), 4.f);
                    ImGui::SetCursorPos(ImVec2(190, 15));
                    if (ImGui::InvisibleButton("Back", ImVec2(40, 40)))
                        iProduct = 0;

                    if (select_offset > 490 && iProduct == 0)
                        menu_state = 2;

                    ImGui::SetCursorPos(ImVec2(550, 140 - select_offset));

                    if (ImGui::Button("Launch", ImVec2(160, 40)))
                        inject_status = true;

                    if (inject_loading >= 509.f) {
                        inject_status = 0.f;
                        msg_text = "Successful inject";
                    }

                    draw->AddText(bold_font, 30, p + ImVec2(190, 190 - select_offset), ImColor(1.f, 1.f, 1.f, 1.f), "DETAILED INFORMATION:");
                    draw->AddText(descript_font, 20, p + ImVec2(190, 220 - select_offset), ImColor(0.6f, 0.6f, 0.6f, 1.f), "This product has many features that are suitable \nfor both Legit play and Rage. You can play \nalong with the screen recording \nand no one will understand that you have cheats present.\nFor the settings, you can go to the forum \nHave a good game!");

                    draw->AddText(icon_font, 25, p + ImVec2(200, 26 - select_offset), ImColor(1.f, 1.f, 1.f, 1.f), "1");

                    draw->AddText(bold_font, 35, p + ImVec2(230, 20 - select_offset), ImColor(1.f, 1.f, 1.f, 1.f), iProduct > 0 ? product_name[iProduct - 1] : "");


                    draw->AddLine(p + ImVec2(180, 440 + inject_offset), p + ImVec2(730, 440 + inject_offset), second_color);

                    ImGui::PushClipRect(p + ImVec2(200, 460 + inject_offset), p + ImVec2(200 + inject_loading, 480 + inject_offset), true);
                    draw->AddRectFilled(p + ImVec2(200, 460 + inject_offset), p + ImVec2(710, 480 + inject_offset), main_color, 30.f);
                    ImGui::PopClipRect();
                    draw->AddRect(p + ImVec2(200, 460 + inject_offset), p + ImVec2(710, 480 + inject_offset), ImColor(1.f, 1.f, 1.f, 1.f), 30.f);
                }
                msg_box_return = ImDialogBoxBase(p);
                move_window();
            }
            ImGui::End();
        }

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

