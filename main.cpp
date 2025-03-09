// Dear ImGui: standalone example application for DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMSPINNER_DEMO

#define WINDOW_WIDTH 850
#define WINDOW_HEIGHT 500

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "implot/implot.h"
#include "implot/implot_internal.h"
#include "implot/implot.cpp"
#include "implot/implot_items.cpp"
#include "implot/implot_demo.cpp"
#include <d3d11.h>
#include <tchar.h>
#include <cstring>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "bytes.hpp"
#include "iconhashes.hpp"
#include "imgui_toggle.h"
#include "imspinner.hpp"
#include "buffers.h"
#include "framework.h"
#include "loader.h"
#include <iostream>
#include <fstream>
#include <d3dx11tex.h>
#pragma comment( lib, "d3dx11.lib")
#pragma comment(lib, "D3DCompiler.lib") // Add this if you are compiling with Visual Studio


static Framework*               framework = (Framework*)malloc(sizeof(Framework));
static Loader*                  loader  = (Loader*)malloc(sizeof(Loader));

static ID3D11Device*           g_pd3dDevice = nullptr;
static ID3D11DeviceContext*    g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*         g_pSwapChain = nullptr;
static UINT                    g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::vector<unsigned char> ReadFileToBytes(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return std::vector<unsigned char>();
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Failed to read file: " << filename << std::endl;
        return std::vector<unsigned char>();
    }

    return buffer;
}

int main(int, char**)
{
    // Entry Point

    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"xua", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"xua", WS_POPUP | WS_OVERLAPPED, 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    io.Fonts->AddFontFromMemoryTTF(&inter_bold, sizeof(inter_bold), 25.f);
    
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    io.Fonts->AddFontFromMemoryTTF(&font_awesome_bin, sizeof(font_awesome_bin), 15.f, &config, icon_ranges);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ID3D11ShaderResourceView* bug;
    ID3D11ShaderResourceView* pfp;

    D3DX11_IMAGE_LOAD_INFO info;
    ID3DX11ThreadPump* pump{ nullptr };

    D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, avatar_bin, sizeof(avatar_bin), &info, pump, &bug, 0);


    D3DX11_IMAGE_LOAD_INFO infop;
    ID3DX11ThreadPump* pumpp{ nullptr };

    D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, profp, sizeof(profp), &infop, pumpp, &pfp, 0);

    ImTextureID background = bug;
    ImTextureID profilePic = pfp;


    bool done = false;

    int test_x = 0;
    int test_y = 0;

    loader->currentStage = LOGIN;
    loader->ticks = 0;

    while (!done)
    {
        {
            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    done = true;
            }
            if (done)
                break;
        }
        
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }
#ifdef DEBUG
        std::cout << "[Resize]" << std::endl;
#endif
        // Start the Dear ImGui frame
        {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
            ImGui::SetNextWindowPos(ImVec2(0, 0));
        }
#ifdef DEBUG
        std::cout << "[ImGui Implementation]" << std::endl;
#endif
        // Style
        framework->default_style();

        

        ImGui::Begin(
            "verahook || I spent actual time. Please, just close out of ida and let us strive.",
            nullptr,
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar
        );

#ifdef DEBUG
        std::cout << "[Window Begin]" << std::endl;
#endif

        ImVec2 cp = ImGui::GetCursorPos();


        ImGui::SetCursorPos(ImVec2(WINDOW_WIDTH - 500, WINDOW_HEIGHT - 450));
        ImGui::Image(background, ImVec2(500, 450));

        ImGui::SetCursorPos(cp);

        framework->move(15, 10);
        ImGui::Text(ICON_FA_CHESS_BISHOP "  euliot loader");

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (ImGui::GetStyle().FramePadding.x));
        framework->move(0, 200);

        switch (loader->currentStage)
        {

        case LOGIN:

            ImGui::PushStyleColor(ImGuiCol_Text, framework->Style::FetchImVec4_RGB(42, 42, 42));

            framework->SecondaryTextInput(175, "##Username", Login::username, "Username", ICON_FA_USER, sizeof(Login::username));
            framework->SecondaryTextInput(225, "##Password", Login::password, "Password", ICON_FA_KEY, sizeof(Login::password));

            ImGui::PopStyleColor();

            if (framework->CenterX_Button("       Login       ", 350) == true)
                loader->currentStage = LOADING;

            break;
        case LOADING:

            break;
        case DASHBOARD:

            framework->set(22, 50);
            ImGui::Text("Welcome, %s", Login::username);

            framework->setY(100);

            framework->CenterX_Text("Dashboard");
            framework->CenterX_Text("Here you can view the products you purchased.");

            // Avatar frame background

            ImGui::GetForegroundDrawList()->AddImageRounded(
                profilePic,
                ImVec2(10, WINDOW_HEIGHT - 50),
                ImVec2(45, WINDOW_HEIGHT - 10),
                ImVec2(0, 0),
                ImVec2(1, 1),
                IM_COL32(255, 255, 255, 255),
                35.f
            );

            /*  issue with loading texture, ignore
            framework->set(WINDOW_WIDTH - 50, WINDOW_HEIGHT);
            ImGui::GetForegroundDrawList()->AddImageRounded(
                profilePic,
                ImVec2(WINDOW_WIDTH - 50, WINDOW_HEIGHT),
                ImVec2(WINDOW_WIDTH - 150, WINDOW_HEIGHT - 100),
                ImVec2(0, 0),
                ImVec2(1, 1),
                IM_COL32(255, 255, 255, 255),
                4.f
            );
            */

            break;
        }

        ImGui::SetCursorPos(ImVec2(WINDOW_WIDTH - 45, 15));
        ImGuiStyle& style = ImGui::GetStyle();
        
        style.Colors[ImGuiCol_Button] = ImVec4(0.35f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.8f, 0.0f, 0.0f, 1.0f);

        style.FramePadding = ImVec2(5, 5);

        if (ImGui::Button(ICON_FA_DOOR_OPEN))
            exit(0);

        framework->style();

#ifdef DEBUG
        std::cout << "[Loader Button + Widgets Done]" << std::endl;
#endif

        ImGui::End();

#ifdef DEBUG
        if (GetAsyncKeyState(VK_SPACE))
        {
            ImGui::Begin("Debug");
            ImGui::SliderInt("X", &test_x, 0, ImGui::GetWindowSize().x);
            ImGui::SliderInt("Y", &test_y, 0, ImGui::GetWindowSize().y);
            ImGui::End();
        }

        ImGui::GetForegroundDrawList()->AddCircleFilled(
ImVec2(test_x + ImGui::GetCursorPosX(), test_y + ImGui::GetCursorPosY()), 5, ImGui::GetColorU32(ImVec4(1, 0, 0, 1)));
#endif
#ifdef DEBUG
        std::cout << "[Render Call]" << std::endl;
#endif
        
		loader->Render();
        // Rendering
        ImGui::Render();

#ifdef DEBUG
        std::cout << "[Render Called]" << std::endl;
#endif
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

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
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler - modified by worldtoscreen#0 to mimic internal cheat design behavior
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_NCHITTEST:

        if (!ImGui::IsAnyItemHovered() && !GetAsyncKeyState(VK_SPACE))
        {
            return HTCAPTION;
        }
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
