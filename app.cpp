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
#include "bitmap_cube.hpp"

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
    bitmap.InitializePerspective(20, 0.1f, 100.0f);
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
 
    std::vector<Vertex> verticesFloor = {
        Vertex(v3(-1, 0, 1), v2(0, 0), v3(0, 1, 0), v3(1, 1, 1)),
        Vertex(v3(1, 0, 0), v2(1, 1), v3(0, 1, 0), v3(1, 1, 1)),
        Vertex(v3(-1, 0, 0), v2(0, 1), v3(0, 1, 0), v3(1, 1, 1)),
        Vertex(v3(1, 0, 1), v2(1, 0), v3(0, 1, 0), v3(1, 1, 1))
    };
    std::vector<u32> indicesFloor = {0, 1, 2, 0, 3, 1};
    Material materialFloor;
    materialFloor.diffuse = Bitmap::LoadFromFile("models/textures/internal_ground_ao_texture.jpeg");

    BitmapCube* environment = new BitmapCube();
    environment->directions[0] = Bitmap::LoadFromFile("models/NissiBeach2/posx.jpg");
    environment->directions[1] = Bitmap::LoadFromFile("models/NissiBeach2/negx.jpg");
    environment->directions[2] = Bitmap::LoadFromFile("models/NissiBeach2/posy.jpg");
    environment->directions[3] = Bitmap::LoadFromFile("models/NissiBeach2/negy.jpg");
    environment->directions[4] = Bitmap::LoadFromFile("models/NissiBeach2/posz.jpg");
    environment->directions[5] = Bitmap::LoadFromFile("models/NissiBeach2/negz.jpg");

    bitmap.environment = environment;

    bool keys[322] = {false};  // 322 is the number of SDLK_DOWN events
    r32 time = 0;
    r32 cameraRotation = -30;
    v3 cameraPosition(-0.3, 0, 0.5);
    bool done = false;
    SDL_Event event;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    done = true;
                    break;
                }
                case SDL_KEYDOWN:{
                    keys[event.key.keysym.sym] = true;
                    break;
                }
                case SDL_KEYUP:{
                    keys[event.key.keysym.sym] = false;
                    break;
                }
            }
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

        bitmap.Clear(v3(0, 0, 0));
        time += 0.1;
        m4 t0 = m4::Translation(v3(0, -0.2, 0.6));
        m4 r0 = m4::Rotation(time * 0.09, Axis::Y);

        m4 ct0 = m4::Translation(cameraPosition);
        m4 cr0 = m4::Rotation(cameraRotation, Axis::Y);

        bitmap.SetViewTransform(ct0 * cr0);
        bitmap.SetModelTransform(t0 * r0);

        bitmap.time = time * 0.1;
        bitmap.DrawTrianglesPBR(vertices, indices, &material);

        //m4 tf0 = m4::Translation(v3(0, -0.2, 0));
        //m4 sf0 = m4::Scale(v3(0.9, 1, 1));
        //bitmap.SetModelTransform((sf0 * tf0) * r0);
        //bitmap.DrawTriangles(verticesFloor, indicesFloor, &materialFloor);

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
