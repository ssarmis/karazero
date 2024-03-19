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
#include "mesh.hpp"
#include "assimp_wrapper.hpp"

r32 Viewport::width;
r32 Viewport::height;

int main() {
    int width = 680;
    int height = 680;

    Viewport::width = width / 2;
    Viewport::height = height / 2;

    SDL_Window* window = SDL_CreateWindow("3D Software Renderer Karazero", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, Viewport::width, Viewport::height);

    Bitmap bitmap;
    bitmap.width = Viewport::width;
    bitmap.height = Viewport::height;
    bitmap.depthBuffer = new r32[bitmap.width * bitmap.height];
    bitmap.InitializePerspective(20, 0.1f, 100.0f);
    for (int i = 0; i < bitmap.width * bitmap.height; ++i) {
        bitmap.depthBuffer[i] = 1;
    }

    Mesh* mesh = AssimpImportModel("models/rumba_dancing.fbx");

    bool keys[65536] = {false};
    r32 time = 0;

    r32 cameraRotation = 0;
    v3 cameraPosition(0, 0, 0);
    bool done = false;
    r32 timeScale = 1;
    SDL_Event event;

    Bitmap lightPassFilter;
    lightPassFilter.width = bitmap.width;
    lightPassFilter.height = bitmap.height;
    lightPassFilter.data = new u8[bitmap.width * bitmap.height * 4];

    Bitmap blurFilter;
    blurFilter.width = bitmap.width;
    blurFilter.height = bitmap.height;
    blurFilter.data = new u8[bitmap.width * bitmap.height * 4];
    int animationIndex = 0;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    done = true;
                    break;
                }
                case SDL_KEYDOWN:{
                    keys[event.key.keysym.sym & ~SDLK_SCANCODE_MASK] = true;
                    break;
                }
                case SDL_KEYUP:{
                    keys[event.key.keysym.sym & ~SDLK_SCANCODE_MASK] = false;
                    break;
                }
            }
        }

        if (keys[(SDLK_RIGHT & ~SDLK_SCANCODE_MASK)]) {
            ++animationIndex;
            keys[(SDLK_RIGHT & ~SDLK_SCANCODE_MASK)] = false;
        }
        if (keys[(SDLK_LEFT & ~SDLK_SCANCODE_MASK)]) {
            --animationIndex;
            keys[(SDLK_RIGHT & ~SDLK_SCANCODE_MASK)] = false;
        }
        if (keys[SDLK_p]) {
            timeScale += 0.1;
        }
        if (keys[SDLK_m]) {
            timeScale -= 0.1;
        }
        if(keys[SDLK_s]){
            cameraPosition.z += 0.1;
        }
        if(keys[SDLK_w]){
            cameraPosition.z -= 0.1;
        }
        if (keys[SDLK_a]) {
            cameraPosition.x += 0.1;
        }
        if (keys[SDLK_d]) {
            cameraPosition.x -= 0.1;
        }
        if (keys[SDLK_q]) {
            cameraRotation += 1;
        }
        if (keys[SDLK_e]) {
            cameraRotation -= 1;
        }
        int pitch = 0;
        SDL_LockTexture(screenTexture, nullptr, (void**)(&bitmap.data), &pitch);

        bitmap.Clear(v3(0.1, 0.1, 0.1));
        time += 1;

        m4 s0 = m4::Scale(v3(0.01, 0.01, 0.01));
        m4 t0 = m4::Translation(v3(0, -1, 5));
        m4 r0 = m4::Rotation(180 + cameraRotation, Axis::Y);

        m4 ct0 = m4::Translation(cameraPosition);
        m4 cr0 = m4::Rotation(0, Axis::Y);

        bitmap.SetViewTransform(ct0 * cr0);
        bitmap.SetModelTransform(t0 * r0 * s0);

        animationIndex = Math::Clamp(animationIndex, 0, mesh->animations.size() - 1);

        if (mesh->animations.size() > 0) {
            auto it = mesh->animations.begin();
            std::advance(it, animationIndex);
            Animation* animation = it->second;
            animation->Advance(timeScale);

            bitmap.UploadBones(animation->CreatePoseTransforms());
        }

        bitmap.time = time * 0.1;
        bitmap.DrawTriangles(mesh->vertices, mesh->indices, mesh->materials);

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
