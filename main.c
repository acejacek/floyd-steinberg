#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#define WIDTH 900
#define HEIGHT 580
#define IMAGE_NAME "image.png"

#define TITLE(MSG) { DrawText(MSG, 10, 10, 40, WHITE); }

enum F {ORIG = 1, GREY, TWO_COLOR, FS_BW, REDUCED2, FS2, REDUCED3, FS3, REDUCED8, FS8, FCount};

typedef struct {
    int errR;
    int errG;
    int errB;
} Error;

// reduce pixel color palete from 255 to 'factor' values
Color reduce(int factor, Color pixel, Error* e)
{
    unsigned char r = pixel.r;
    unsigned char g = pixel.g;
    unsigned char b = pixel.b;
    pixel.r = (unsigned char)roundf(factor * r / 255) * (255 / factor); 
    pixel.g = (unsigned char)roundf(factor * g / 255) * (255 / factor);
    pixel.b = (unsigned char)roundf(factor * b / 255) * (255 / factor); 

    e->errR = r - pixel.r;
    e->errG = g - pixel.g;
    e->errB = b - pixel.b;
    return pixel;
} 

// add error value to the pixel
Color add_error(Color pixel, Error e, int factor)
{
    pixel.r += e.errR * factor >> 4;
    pixel.g += e.errG * factor >> 4; 
    pixel.b += e.errB * factor >> 4;

    return pixel;
}

int main(void)
{
    enum F filter = ORIG;
    Error err;

    InitWindow(WIDTH, HEIGHT, "Floyd Steinberg");
    SetTargetFPS(60);

    Image orig = LoadImage(IMAGE_NAME);
    ImageResize(&orig, WIDTH, HEIGHT);

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(0))
            if (++filter >= FCount) filter = ORIG;
        if (IsMouseButtonPressed(1))
            if (--filter == 0) filter = FCount - 1;

        Color* colors = LoadImageColors(orig);
        BeginDrawing();
        ClearBackground(BLACK);

        switch (filter)
        {
            case ORIG:
                for (int y = 0; y < HEIGHT; ++y)
                    for (int x = 0; x < WIDTH; ++x)
                    {
                        int index = (y * orig.width + x);
                        Color pixel = colors[index];
                        DrawPixel(x, y, pixel);
                    }
                TITLE("Original");
                break;

            case GREY:
                for (int y = 0; y < HEIGHT; ++y)
                    for (int x = 0; x < WIDTH; ++x)
                    {
                        int index = (y * orig.width + x);
                        Color pixel = colors[index];
                        float r = pixel.r * 0.2126f;
                        float g = pixel.g * 0.7152f;
                        float b = pixel.b * 0.0722f;
                        float luma = r + g + b; 
                        Color grey = {luma, luma, luma, 255};
                        DrawPixel(x, y, grey);
                    }
                TITLE("Grey");
                break;
                
            case TWO_COLOR:
                for (int y = 0; y < HEIGHT; ++y)
                    for (int x = 0; x < WIDTH; ++x)
                    {
                        int index = (y * orig.width + x);
                        Color pixel = colors[index];
                        float r = pixel.r * 0.2126f;
                        float g = pixel.g * 0.7152f;
                        float b = pixel.b * 0.0722f;
                        float luma = r + g + b; 
                        Color grey = {luma, luma, luma, 255};
                        grey = reduce(2, grey, &err);
                        DrawPixel(x, y, grey);
                    }
                TITLE("Two color");
                break;

            case REDUCED2:
                for (int y = 0; y < HEIGHT; ++y)
                    for (int x = 0; x < WIDTH; ++x)
                    {
                        int index = (y * orig.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(2, pixel, &err);
                        DrawPixel(x, y, pixel);
                    }
                TITLE("Reduced 2");
                break;

            case REDUCED3:
                for (int y = 0; y < HEIGHT; ++y)
                    for (int x = 0; x < WIDTH; ++x)
                    {
                        int index = (y * orig.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(3, pixel, &err);
                        DrawPixel(x, y, pixel);
                    }
                TITLE("Reduced 3");
                break;

            case REDUCED8:
                for (int y = 0; y < HEIGHT; ++y)
                    for (int x = 0; x < WIDTH; ++x)
                    {
                        int index = (y * orig.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(8, pixel, &err);
                        DrawPixel(x, y, pixel);
                    }
                TITLE("Reduced 8");
                break;

            case FS2:
                for (int y = 0; y < HEIGHT-1; ++y)
                    for (int x = 1; x < WIDTH-1; ++x)
                    {
                        int index = (y * orig.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(2, pixel, &err);
                        DrawPixel(x, y, pixel);

                        index = ( y      * orig.width + x + 1);
                        colors[index] = add_error(colors[index], err, 7);

                        index = ((y + 1) * orig.width + x - 1);
                        colors[index] = add_error(colors[index], err, 3);

                        index = ((y + 1) * orig.width + x);
                        colors[index] = add_error(colors[index], err, 5);

                        index = ((y + 1) * orig.width + x + 1);
                        colors[index] = add_error(colors[index], err, 1);
                    }
                TITLE("Floyd-Steinberg 2");
                break;

            case FS3:
                for (int y = 0; y < HEIGHT-1; ++y)
                    for (int x = 1; x < WIDTH-1; ++x)
                    {
                        int index = (y * orig.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(3, pixel, &err);
                        DrawPixel(x, y, pixel);

                        index = ( y      * orig.width + x + 1);
                        colors[index] = add_error(colors[index], err, 7);

                        index = ((y + 1) * orig.width + x - 1);
                        colors[index] = add_error(colors[index], err, 3);

                        index = ((y + 1) * orig.width + x);
                        colors[index] = add_error(colors[index], err, 5);

                        index = ((y + 1) * orig.width + x + 1);
                        colors[index] = add_error(colors[index], err, 1);
                    }
                TITLE("Floyd-Steinberg 3");
                break;

            case FS8:
                for (int y = 0; y < HEIGHT-1; ++y)
                    for (int x = 1; x < WIDTH-1; ++x)
                    {
                        int index = (y * orig.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(8, pixel, &err);
                        DrawPixel(x, y, pixel);

                        index = ( y      * orig.width + x + 1);
                        colors[index] = add_error(colors[index], err, 7);

                        index = ((y + 1) * orig.width + x - 1);
                        colors[index] = add_error(colors[index], err, 3);

                        index = ((y + 1) * orig.width + x);
                        colors[index] = add_error(colors[index], err, 5);

                        index = ((y + 1) * orig.width + x + 1);
                        colors[index] = add_error(colors[index], err, 1);
                    }
                TITLE("Floyd-Steinberg 8");
                break;

            case FS_BW:
                for (int y = 0; y < HEIGHT-1; ++y)
                    for (int x = 1; x < WIDTH-1; ++x)
                    {
                        int index = (y * orig.width + x);
                        Color pixel = colors[index];
                        float r = pixel.r * 0.2126f;
                        float g = pixel.g * 0.7152f;
                        float b = pixel.b * 0.0722f;
                        unsigned char luma = r + g + b; 
                        Color grey = {luma, luma, luma, 255};
                        grey = reduce(2, grey, &err);
                        DrawPixel(x, y, grey);

                        index = ( y      * orig.width + x + 1);
                        colors[index] = add_error(colors[index], err, 7);

                        index = ((y + 1) * orig.width + x - 1);
                        colors[index] = add_error(colors[index], err, 3);

                        index = ((y + 1) * orig.width + x);
                        colors[index] = add_error(colors[index], err, 5);

                        index = ((y + 1) * orig.width + x + 1);
                        colors[index] = add_error(colors[index], err, 1);
                    }
                TITLE("Floyd-Steinberg BW");
                break;

            default:
        }
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
