#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#define WIDTH 900
#define HEIGHT 580
#define IMAGE_NAME "image.png"

#define TITLE(MSG) { DrawText(MSG, 10, 10, 40, WHITE); }

enum F {ORIG = 1,
        GREY, TWO_COLOR, FS_BW, ATKINSON,
        REDUCED2, FS2,
        REDUCED3, FS3,
        REDUCED8, FS8,
        FCount};

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

Color make_grey(Color* pixel)
{
    float r = pixel->r * 0.2126f;
    float g = pixel->g * 0.7152f;
    float b = pixel->b * 0.0722f;
    float luma = r + g + b; 
    return (Color){luma, luma, luma, 255};
} 

// FS - add error value to the pixel
Color add_error(Color pixel, Error e, int factor)
{
    pixel.r += e.errR * factor >> 4;
    pixel.g += e.errG * factor >> 4; 
    pixel.b += e.errB * factor >> 4;

    return pixel;
}

// Atkinson - add error value to the pixel
Color add_error_atk(Color pixel, Error e)
{
    pixel.r += e.errR >> 3;
    pixel.g += e.errG >> 3; 
    pixel.b += e.errB >> 3;

    return pixel;
}

void distribute_error(int x, int y, int width, Color* colors, int index, Error err)
{
	colors[index] = add_error(colors[index], err, 7);

	index = ((y + 1) * width + x - 1);
	colors[index] = add_error(colors[index], err, 3);

	index = ((y + 1) * width + x);
	colors[index] = add_error(colors[index], err, 5);

	index = ((y + 1) * width + x + 1);
	colors[index] = add_error(colors[index], err, 1);
}

int main(void)
{
    enum F filter = ORIG;
    Error err;

    InitWindow(WIDTH, HEIGHT, "Floyd Steinberg");
    SetTargetFPS(60);

    Image img = LoadImage(IMAGE_NAME);
    ImageResize(&img, WIDTH, HEIGHT);

    printf("img: %d, %d\n", img.height, img.width);

    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(0))
            if (++filter >= FCount) filter = ORIG;
        if (IsMouseButtonPressed(1))
            if (--filter == 0) filter = FCount - 1;

        Color* colors = LoadImageColors(img);
        BeginDrawing();
        ClearBackground(BLACK);

        switch (filter)
        {
            case ORIG:
                for (int y = 0; y < img.height; ++y)
                    for (int x = 0; x < img.width; ++x)
                    {
                        int index = (y * img.width + x);
                        Color pixel = colors[index];
                        DrawPixel(x, y, pixel);
                    }
                TITLE("Original");
                break;

            case GREY:
                for (int y = 0; y < img.height; ++y)
                    for (int x = 0; x < img.width; ++x)
                    {
                        int index = (y * img.width + x);
                        Color grey = make_grey(colors + index);
                        DrawPixel(x, y, grey);
                    }
                TITLE("Grey");
                break;

            case TWO_COLOR:
                for (int y = 0; y < img.height; ++y)
                    for (int x = 0; x < img.width; ++x)
                    {
                        int index = (y * img.width + x);
                        Color grey = make_grey(colors + index);
                        grey = reduce(2, grey, &err);
                        DrawPixel(x, y, grey);
                    }
                TITLE("Two color");
                break;

            case REDUCED2:
                for (int y = 0; y < img.height; ++y)
                    for (int x = 0; x < img.width; ++x)
                    {
                        int index = (y * img.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(2, pixel, &err);
                        DrawPixel(x, y, pixel);
                    }
                TITLE("Reduced 2");
                break;

            case REDUCED3:
                for (int y = 0; y < img.height; ++y)
                    for (int x = 0; x < img.width; ++x)
                    {
                        int index = (y * img.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(3, pixel, &err);
                        DrawPixel(x, y, pixel);
                    }
                TITLE("Reduced 3");
                break;

            case REDUCED8:
                for (int y = 0; y < img.height; ++y)
                    for (int x = 0; x < img.width; ++x)
                    {
                        int index = (y * img.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(8, pixel, &err);
                        DrawPixel(x, y, pixel);
                    }
                TITLE("Reduced 8");
                break;

            case FS2:
                for (int y = 0; y < img.height - 1; ++y)
                    for (int x = 1; x < img.width - 1; ++x)
                    {
                        int index = (y * img.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(2, pixel, &err);
                        DrawPixel(x, y, pixel);

                        index = ( y      * img.width + x + 1);
                        colors[index] = add_error(colors[index], err, 7);

                        index = ((y + 1) * img.width + x - 1);
                        colors[index] = add_error(colors[index], err, 3);

                        index = ((y + 1) * img.width + x);
                        colors[index] = add_error(colors[index], err, 5);

                        index = ((y + 1) * img.width + x + 1);
                        colors[index] = add_error(colors[index], err, 1);
                    }
                TITLE("Floyd-Steinberg 2");
                break;

            case FS3:
                for (int y = 0; y < img.height-1; ++y)
                    for (int x = 1; x < img.width-1; ++x)
                    {
                        int index = (y * img.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(3, pixel, &err);
                        DrawPixel(x, y, pixel);

                        index = ( y      * img.width + x + 1);
                        colors[index] = add_error(colors[index], err, 7);

                        index = ((y + 1) * img.width + x - 1);
                        colors[index] = add_error(colors[index], err, 3);

                        index = ((y + 1) * img.width + x);
                        colors[index] = add_error(colors[index], err, 5);

                        index = ((y + 1) * img.width + x + 1);
                        colors[index] = add_error(colors[index], err, 1);
                    }
                TITLE("Floyd-Steinberg 3");
                break;

            case FS8:
                for (int y = 0; y < img.height-1; ++y)
                    for (int x = 1; x < img.width-1; ++x)
                    {
                        int index = (y * img.width + x);
                        Color pixel = colors[index];
                        pixel = reduce(8, pixel, &err);
                        DrawPixel(x, y, pixel);

                        index = ( y      * img.width + x + 1);
                        colors[index] = add_error(colors[index], err, 7);

                        index = ((y + 1) * img.width + x - 1);
                        colors[index] = add_error(colors[index], err, 3);

                        index = ((y + 1) * img.width + x);
                        colors[index] = add_error(colors[index], err, 5);

                        index = ((y + 1) * img.width + x + 1);
                        colors[index] = add_error(colors[index], err, 1);
                    }
                TITLE("Floyd-Steinberg 8");
                break;

            case FS_BW:
                for (int y = 0; y < img.height-1; ++y)
                    for (int x = 1; x < img.width-1; ++x)
                    {
                        int index = (y * img.width + x);
                        Color grey = make_grey(colors + index);
                        grey = reduce(2, grey, &err);
                        DrawPixel(x, y, grey);
						
						distribute_error(x, y, img.width, colors, index, err);

                    }
                TITLE("Floyd-Steinberg BW");
                break;

            case ATKINSON:
                for (int y = 0; y < img.height; ++y)
                    for (int x = 1; x < img.width; ++x)
                    {
                        int index = (y * img.width + x);
                        Color grey = make_grey(colors + index);
                        grey = reduce(2, grey, &err);
                        DrawPixel(x, y, grey);

                        if (x + 1 < img.width) {
                            index = ( y      * img.width + x + 1);
                            colors[index] = add_error_atk(colors[index], err);
                        }
                        if (x + 2 < img.width) {
                            index = ( y      * img.width + x + 2);
                            colors[index] = add_error_atk(colors[index], err);
                        }
                        if (y + 1 < img.height) {
                            index = ((y + 1) * img.width + x - 1);
                            colors[index] = add_error_atk(colors[index], err);


                            index = ((y + 1) * img.width + x);
                            colors[index] = add_error_atk(colors[index], err);

                            if (x + 1 < img.width) {
                                index = ((y + 1) * img.width + x + 1);
                                colors[index] = add_error_atk(colors[index], err);
                            }
                        }
                        if (y + 2 < img.height) {
                            index = ((y + 2) * img.width + x);
                            colors[index] = add_error_atk(colors[index], err);
                        }
                    }
                TITLE("Atkinson dithering BW");
                break;

            case FCount:
            break;
        }
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
