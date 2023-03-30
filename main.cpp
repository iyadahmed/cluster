#include <cstdio>
#include <iostream>
#include <filesystem>
#include <thread>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#define UNICODE

#include <windows.h>

#include <glm/glm.hpp>

#include "indexed_mesh.hpp"
#include "bvh.hpp"

static BVH *bvh;

float width = 300;
float height = 200;
float camera[3] = {0, 0, 1};
float ratio = width / height;
float screen[4] = {-1, 1 / ratio, 1, -1 / ratio}; // left, top, right, bottom

typedef uint32_t u32;
BITMAPINFO BitmapInfo;
void *BitmapMemory;
int BitmapWidth;
int BitmapHeight;

int ClientWidth;
int ClientHeight;

// Draws a pixel at X, Y (from top left corner)
void DrawPixel(int X, int Y, u32 Color) {
    u32 *Pixel = (u32 *) BitmapMemory;
    Pixel += Y * BitmapWidth + X;
    *Pixel = Color;
}

void ClearScreen(u32 Color) {
    u32 *Pixel = (u32 *) BitmapMemory;
    for (int Index = 0;
         Index < BitmapWidth * BitmapHeight;
         ++Index) { *Pixel++ = Color; }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Expected arguments: path/to/stl/or/directory/of/STLs\n");
        return 0;
    }
    char *input_path = argv[1];

    IndexedMesh mesh;

    if (std::filesystem::is_directory(input_path)) {
        for (const auto &entry: std::filesystem::directory_iterator(input_path)) {
            const std::filesystem::path &path = entry.path();
            if (path.extension() == ".stl") {
                mesh = read_indexed_mesh_from_stl((const char *) path.string().c_str());
            }
        }
    } else {
        mesh = read_indexed_mesh_from_stl(input_path);
    }

    bvh = new BVH(mesh);
    std::cout << bvh->contains_point(mesh.unique_vertices[0]) << std::endl;

    FILE *ppm_image = fopen("output.ppm", "w");
    if (ppm_image == NULL) {
        std::cout << "Failed to create image file" << std::endl;
    }
    fputs("P3\n", ppm_image);
    int w = 300, h = 200;
    fprintf(ppm_image, "%d %d\n255\n", w, h);

    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            glm::vec2 coord((float) i / (float) w, (float) j / (float) h);
            coord = coord * 2.0f - 1.0f;

            glm::vec3 ray_origin(0.0f);
            glm::vec3 ray_direction = {coord.x, coord.y, -1.0f};
            ray_direction = glm::normalize(ray_direction);

            float t = 100000.0f;
            bool is_hit = bvh->intersect_ray(ray_origin, ray_direction, t);

            if (is_hit) {
                std::cout << "foo" << std::endl;
                uint8_t c = (uint8_t) std::clamp(t, 0.0f, 255.0f);
                fprintf(ppm_image, "%d %d %d\n", c, c, c);
            } else {
                fprintf(ppm_image, "0 0 0\n");
            }
        }
    }

    fclose(ppm_image);

    // WINDOW
    // ____________________________________________________________________________________

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"BVH";

    WNDCLASS wc = {};
    HINSTANCE hInstance = GetModuleHandle(NULL);

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"BVH",    // Window text
            WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            hInstance,  // Instance handle
            NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, SW_NORMAL);

    // BITMAP
    RECT ClientRect;
    GetClientRect(hwnd, &ClientRect);
    ClientWidth = ClientRect.right - ClientRect.left;
    ClientHeight = ClientRect.bottom - ClientRect.top;

    BitmapWidth = ClientWidth;
    BitmapHeight = ClientHeight;

    // Allocate memory for the bitmap

    int BytesPerPixel = 4;

    BitmapMemory = VirtualAlloc(0,
                                BitmapWidth * BitmapHeight * BytesPerPixel,
                                MEM_RESERVE | MEM_COMMIT,
                                PAGE_READWRITE
    );

    // BitmapInfo struct for StretchDIBits


    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    // Negative height makes top left as the coordinate system origin for the DrawPixel function, otherwise its bottom left
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    // __________________________________________________

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

//            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));

//            for (long x = ps.rcPaint.left; x < ps.rcPaint.right; x++) {
//                for (long y = ps.rcPaint.top; y < ps.rcPaint.bottom; y++) {
//                    SetPixel(hdc, x, y, RGB(255, 0, 0));
//                }
//            }
            ClearScreen(0x333333);

            // All painting occurs here, between BeginPaint and EndPaint.

            StretchDIBits(hdc,
                          0, 0,
                          BitmapWidth, BitmapHeight,
                          0, 0,
                          ClientWidth, ClientHeight,
                          BitmapMemory, &BitmapInfo,
                          DIB_RGB_COLORS, SRCCOPY);


            EndPaint(hwnd, &ps);
        }
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}