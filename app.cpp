#include <iostream>
#include <list>
#include <SDL.h>
#undef main

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "bitmap.hpp"
#include "math.hpp"
#include "global.hpp"

#include "objloader.hpp"
#include "material.hpp"

r32 Viewport::width;
r32 Viewport::height;

int main() {
    int width = 1280;
    int height = 768;

    Viewport::width = width / 1;
    Viewport::height = height / 1;

    SDL_Window* window = SDL_CreateWindow("Title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, Viewport::width, Viewport::height);

    Bitmap bitmap = {};
    bitmap.width = Viewport::width;
    bitmap.height = Viewport::height;
    bitmap.depthBuffer = new r32[bitmap.width * bitmap.height];
    bitmap.InitializePerspective(90, 0.1f, 100.0f);
    for (int i = 0; i < bitmap.width * bitmap.height; ++i) {
        bitmap.depthBuffer[i] = 1;
    }

    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    OBJLoader::Load("models/baneling.obj", vertices, indices);

    Material material;
    material.diffuse = Bitmap::LoadFromFile("models/textures/DefaultMaterial_Base_Color.jpeg");
    material.normal = Bitmap::LoadFromFile("models/textures/DefaultMaterial_Normal_DirectX.png");
    material.metalic = Bitmap::LoadFromFile("models/textures/DefaultMaterial_Metallic.jpeg");
    material.roughness = Bitmap::LoadFromFile("models/textures/DefaultMaterial_Roughness.jpeg");
    material.emissive = Bitmap::LoadFromFile("models/textures/DefaultMaterial_Emissive.jpeg");
    material.ambientOcclusion = Bitmap::LoadFromFile("models/textures/DefaultMaterial_Mixed_AO.jpeg");

    r32 time = 0;
    bool done = false;
    SDL_Event event;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
                done = true;
                break;
            }
            }
        }

        int pitch = 0;
        SDL_LockTexture(screenTexture, nullptr, (void**)(&bitmap.data), &pitch);

        bitmap.Clear(v3(0.9, 0.9, 0.9));
        time += 0.01;

        m4 t0 = m4::Translation(v3(0, -0.2, 0.6));
        m4 r0 = m4::Rotation(time, Axis::Y);
        m4 ct0 = m4::Translation(v3(0, 0, 0));

        bitmap.SetViewTransform(ct0);
        bitmap.SetModelTransform(t0 * r0);

        bitmap.time = time;
        bitmap.DrawTriangles(vertices, indices, &material);

        SDL_UnlockTexture(screenTexture);

        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = bitmap.width;
        rect.h = bitmap.height;

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screenTexture, &rect, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
