#include <SDL.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cmath>

const int WIDTH = 800;
const int HEIGHT = 600;
const float bgMultiplier = 0.01f;

const uint32_t BLACK = 0xFF000000;
const uint32_t WHITE = 0xFFFFFFFF;

const uint8_t H[7] = {
    0b10001,
    0b10001,
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b10001
};

const uint8_t E[7] = {
    0b11111,
    0b10000,
    0b11110,
    0b10000,
    0b10000,
    0b10000,
    0b11111
};

const uint8_t L[7] = {
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b11111
};

const uint8_t O[7] = {
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b01110
};

const uint8_t W[7] = {
    0b10001,
    0b10001,
    0b10001,
    0b10101,
    0b10101,
    0b10101,
    0b01010
};

const uint8_t R[7] = {
    0b11110,
    0b10001,
    0b10001,
    0b11110,
    0b10100,
    0b10010,
    0b10001
};

const uint8_t D[7] = {
    0b11110,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b11110
};

const uint8_t* GetGlyph(char c)
{
    switch (c)
    {
        case 'H': return H;
        case 'E': return E;
        case 'L': return L;
        case 'O': return O;
        case 'W': return W;
        case 'R': return R;
        case 'D': return D;
        default: return nullptr;
    }
}

struct Texture
{
    int width;
    int height;
    std::vector<uint32_t> pixels;

};

Texture CreateCheckerTexture()
{
    Texture tex;

    tex.width = 256;
    tex.height = 256;

    tex.pixels.resize(tex.width * tex.height);

    for (int y = 0; y < tex.height; y++)
    {
        for (int x = 0; x < tex.width; x++)
        {
            bool white =
                ((x / 32) + (y / 32)) % 2;

            uint8_t c = white ? 255 : 0;

            tex.pixels[y * tex.width + x] =
                (255 << 24) |
                (c << 16) |
                (c << 8) |
                c;
        }
    }

    return tex;
}

void Clear(std::vector<uint32_t>& pixels, uint32_t color)
{
    std::fill(pixels.begin(), pixels.end(), color);
}

void PutPixel(std::vector<uint32_t>& pixels, int x, int y, uint32_t color)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    pixels[y * WIDTH + x] = color;
}

void DrawBlockPixel(std::vector<uint32_t>& pixels, int x, int y, int scale, uint32_t color)
{
    for (int py = 0; py < scale; py++)
    {
        for (int px = 0; px < scale; px++)
        {
            PutPixel(pixels, x + px, y + py, color);
        }
    }
}

void DrawGlyph(std::vector<uint32_t>& pixels, const uint8_t* glyph, int x, int y, int scale, uint32_t color)
{
    if (glyph == nullptr)
        return;

    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            bool pixelOn = glyph[row] & (1 << (4 - col));

            if (pixelOn)
            {
                DrawBlockPixel(
                    pixels,
                    x + col * scale,
                    y + row * scale,
                    scale,
                    color
                );
            }
        }
    }
}

void DrawText(std::vector<uint32_t>& pixels, const std::string& text, int x, int y, int scale, uint32_t color)
{
    int cursorX = x;

    for (char c : text)
    {
        if (c == ' ')
        {
            cursorX += 4 * scale;
            continue;
        }

        DrawGlyph(pixels, GetGlyph(c), cursorX, y, scale, color);
        cursorX += 6 * scale;
    }
}

void DrawGradient(std::vector<uint32_t>& pixels, float time)
{
    for (int y = 0; y < HEIGHT; y++)
    {

        for (int x = 0; x < WIDTH; x++)
        {
            uint8_t r = static_cast<uint8_t>((x / static_cast<float>(WIDTH - 1)) * 255.0f);
            uint8_t g = static_cast<uint8_t>((y / static_cast<float>(HEIGHT - 1)) * 255.0f);
            uint8_t b = 128;

            pixels[y * WIDTH + x] =
                0xFF000000 |
                (r << 16) |
                (g << 8) |
                b;
        }
    }
}

void DrawAnimatedGradient(std::vector<uint32_t>& pixels, float time)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            float u = x / static_cast<float>(WIDTH - 1);
            float v = y / static_cast<float>(HEIGHT - 1);

            uint8_t r = static_cast<uint8_t>((u + time * 0.1f) * 255.0f);
            uint8_t g = static_cast<uint8_t>((v + time * 0.1f) * 255.0f);
            uint8_t b = 128;

            pixels[y * WIDTH + x] =
                0xFF000000 |
                (r << 16) |
                (g << 8) |
                b;
        }
    }
}

void DrawPlasma(std::vector<uint32_t>& pixels, float time)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            float value = 0.0f;

            value += sinf(x * bgMultiplier + time);
            value += sinf(y * bgMultiplier + time);
            value += sinf((x + y) * bgMultiplier + time);

            value /= 3.0f;

            uint8_t color =
                static_cast<uint8_t>((value + 1.0f) * 127.5f);

            pixels[y * WIDTH + x] =
                0xFF000000 |
                (color << 16) |
                (color << 8) |
                color;
        }
    }
}

void DrawColorPlasma(std::vector<uint32_t>& pixels, float time)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            float value = 0.0f;

            value += sinf(x * 0.03f + time);
            value += sinf(y * 0.03f + time);
            value += sinf((x + y) * 0.02f + time);
            value += sinf(sqrtf(x * x + y * y) * 0.03f + time);

            value /= 4.0f;

            uint8_t r = static_cast<uint8_t>((sinf(value * 3.0f + time) + 1.0f) * 127.5f);
            uint8_t g = static_cast<uint8_t>((sinf(value * 3.0f + time + 2.0f) + 1.0f) * 127.5f);
            uint8_t b = static_cast<uint8_t>((sinf(value * 3.0f + time + 4.0f) + 1.0f) * 127.5f);

            pixels[y * WIDTH + x] =
                0xFF000000 |
                (r << 16) |
                (g << 8) |
                b;
        }
    }
}

void DrawCircularWave(std::vector<uint32_t>& pixels, float time)
{
    float centerX = WIDTH * 0.5f;
    float centerY = HEIGHT * 0.5f;

    float frequency = 0.08f;
    float speed = 4.0f;

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            float dx = x - centerX;
            float dy = y - centerY;

            float distance = sqrtf(dx * dx + dy * dy);

            float wave = sinf(distance * frequency - time * speed);

            uint8_t brightness =
                static_cast<uint8_t>((wave + 1.0f) * 127.5f);

            pixels[y * WIDTH + x] =
                0xFF000000 |
                (brightness << 16) |
                (brightness << 8) |
                brightness;
        }
    }
}

void DrawColorCircularWave(std::vector<uint32_t>& pixels, float time)
{
    float centerX = WIDTH * 0.5f;
    float centerY = HEIGHT * 0.5f;

    float frequency = 0.08f;
    float speed = 4.0f;

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            float dx = x - centerX;
            float dy = y - centerY;

            float distance = sqrtf(dx * dx + dy * dy);

            float wave = sinf(distance * frequency - time * speed);

            uint8_t r = static_cast<uint8_t>((sinf(wave * 3.0f + time) + 1.0f) * 127.5f);
            uint8_t g = static_cast<uint8_t>((sinf(wave * 3.0f + time + 2.0f) + 1.0f) * 127.5f);
            uint8_t b = static_cast<uint8_t>((sinf(wave * 3.0f + time + 4.0f) + 1.0f) * 127.5f);

            pixels[y * WIDTH + x] =
                0xFF000000 |
                (r << 16) |
                (g << 8) |
                b;
        }
    }
}
void DrawTunnel(std::vector<uint32_t>& pixels, float time)
{
    float centerX = WIDTH * 0.5f;
    float centerY = HEIGHT * 0.5f;

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            float dx = x - centerX;
            float dy = y - centerY;

            float distance =
                sqrtf(dx * dx + dy * dy);

            float angle =
                atan2f(dy, dx);

            float value = sinf(distance * 0.05f - time * 4.0f) + sinf(angle * 10.0f);

            uint8_t color =
                static_cast<uint8_t>(
                    (value + 1.0f) * 127.5f
                );

            pixels[y * WIDTH + x] =
                0xFF000000 |
                (color << 16) |
                (color << 8) |
                color;
        }
    }
}

void DrawSpiral(uint32_t* pixels, int width, int height, float time)
{
    float centerX = width * 0.5f;
    float centerY = height * 0.5f;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float dx = x - centerX;
            float dy = y - centerY;

            float distance = sqrtf(dx * dx + dy * dy);
            float angle = atan2f(dy, dx);

            float value = sinf(distance * 0.05f + angle * 6.0f - time * 3.0f);

            uint8_t color = (uint8_t)((value + 1.0f) * 127.5f);

            pixels[y * width + x] =
                (255 << 24) |
                (color << 16) |
                (color << 8) |
                color;
        }
    }
}

void DrawSpiralColored(std::vector<uint32_t>& pixels, int width, int height, float time)
{
    float centerX = width * 0.5f;
    float centerY = height * 0.5f;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float dx = x - centerX;
            float dy = y - centerY;

            float distance = sqrtf(dx * dx + dy * dy);
            float angle = atan2f(dy, dx);

            float value = sinf(distance * 0.05f + angle * 6.0f - time * 3.0f);

            uint8_t r = static_cast<uint8_t>((sinf(value * 3.0f + time) + 1.0f) * 127.5f);
            uint8_t g = static_cast<uint8_t>((sinf(value * 3.0f + time + 2.0f) + 1.0f) * 127.5f);
            uint8_t b = static_cast<uint8_t>((sinf(value * 3.0f + time + 4.0f) + 1.0f) * 127.5f);

            pixels[y * WIDTH + x] =
                0xFF000000 |
                (r << 16) |
                (g << 8) |
                b;
        }
    }
}

void DrawTexture(std::vector<uint32_t>& screenPixels, const Texture& texture, int screenWidth, int screenHeight)
{

    for (int y = 0; y < screenHeight; y++)
    {
        for (int x = 0; x < screenWidth; x++)
        {
            float u = (float)x/screenWidth;
            float v = (float)y/screenHeight;

            int tx = (int)(u*texture.width);
            int ty = (int)(v*texture.height);

            screenPixels[y*screenWidth+x] = texture.pixels[ty*texture.width + tx];
        }
        
    }
    
}

void DrawSprialCheckerboard(std::vector<uint32_t>& screenPixels, const Texture& texture, int screenWidth, int screenHeight,float time)
{
    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.5f;

    for (int y = 0; y < screenHeight; y++)
    {
        for (int x = 0; x < screenWidth; x++)
        {
            float dx = x - centerX;
            float dy = y - centerY;

            float distance = sqrtf(dx * dx + dy * dy);
            float angle = atan2f(dy, dx);

            float value = sinf(distance * 0.05f + angle * 6.0f - time * 3.0f);

            int tx = static_cast<int>(value + texture.width * 0.5f);
            int ty = static_cast<int>(value + texture.height * 0.5f);

            tx = ((tx % texture.width) + texture.width) % texture.width;
            ty = ((ty % texture.height) + texture.height) % texture.height;

            screenPixels[y * screenWidth + x] =
                texture.pixels[ty * texture.width + tx];

        }
    }

}

void DrawRotoZoomer(
    std::vector<uint32_t>& screenPixels,
    const Texture& texture,
    int screenWidth,
    int screenHeight,
    float time)
{
    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.5f;

    float angle = time;
    float zoom = 1.5f + sinf(time) * 0.8f;

    float cosA = cosf(angle);
    float sinA = sinf(angle);

    for (int y = 0; y < screenHeight; y++)
    {
        for (int x = 0; x < screenWidth; x++)
        {
            float dx = x - centerX;
            float dy = y - centerY;

            float rx = dx * cosA - dy * sinA;
            float ry = dx * sinA + dy * cosA;

            rx /= zoom;
            ry /= zoom;

            int tx = static_cast<int>(rx + texture.width * 0.5f);
            int ty = static_cast<int>(ry + texture.height * 0.5f);

            tx = ((tx % texture.width) + texture.width) % texture.width;
            ty = ((ty % texture.height) + texture.height) % texture.height;

            screenPixels[y * screenWidth + x] =
                texture.pixels[ty * texture.width + tx];
        }
    }
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "DemoSceneLab - Framebuffer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );

    std::vector<uint32_t> pixels(WIDTH * HEIGHT);

    bool running = true;
    SDL_Event event;
    
    Texture checkerTexture = CreateCheckerTexture();
    float textY = -80.0f;
    const float speed = 80.0f;

    Uint32 lastTime = SDL_GetTicks();

    while (running)
    {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
        }

        textY += speed * deltaTime;

        if (textY > HEIGHT)
            textY = -80.0f;

        float time = currentTime / 1000.0f;

        DrawSprialCheckerboard(pixels,checkerTexture,WIDTH,HEIGHT,time);
        //DrawRotoZoomer(pixels,checkerTexture,WIDTH,HEIGHT,time);
        //DrawSpiral(pixels,WIDTH,HEIGHT,time);
        //DrawTunnel(pixels,time);
        //DrawColorCircularWave(pixels, time);
        //DrawColorPlasma(pixels, time);
        
        // DrawText(
        //     pixels,
        //     "HELLO WORLD",
        //     120,
        //     static_cast<int>(textY),
        //     10,
        //     WHITE
        // );

        SDL_UpdateTexture(
            texture,
            nullptr,
            pixels.data(),
            WIDTH * sizeof(uint32_t)
        );

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}