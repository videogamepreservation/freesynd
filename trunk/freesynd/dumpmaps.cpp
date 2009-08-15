
#include "screen.h"
#include "mapmanager.h"
#include "file.h"
#include <png.h>
#include <sys/stat.h>
#include <sys/types.h>

class App : public Singleton<App> {
public:
    App();
};

int screen_width = 8000;
int screen_height = 4000;
png_byte **screen_data = NULL;
png_color palette[256];

int write_png(int a) {
    char tmp[100];
    sprintf(tmp, "maps/map%i.png", a);
    FILE *fp = fopen(tmp, "wb");

    if (!fp) {
        printf("cannot write to %s\n", tmp);
        return 1;
    }

    png_structp png_ptr = png_create_write_struct(
            PNG_LIBPNG_VER_STRING, 0, 0, 0);

    if (!png_ptr) {
        printf("cannot create png write struct\n");
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr) {
        printf("cannot create png info struct\n");
        return 1;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, screen_width, screen_height, 8,
            PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_set_PLTE(png_ptr, info_ptr, palette, 256);

    png_write_info(png_ptr, info_ptr);

    png_write_image(png_ptr, screen_data);

    png_write_end(png_ptr, info_ptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);

    return 0;
}

void clear_screen() {
    for (int i = 0; i < screen_height; i++)
        memset(screen_data[i], 255, screen_width);
}

int main(int argc, char **argv) {
    App *app = new App();
    Screen *screen = new Screen(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
    MapManager maps;

    screen_data = (png_byte **) calloc(1, sizeof(png_byte *) * screen_height);

    for (int i = 0; i < screen_height; i++)
        screen_data[i] = (png_byte *)calloc(1, sizeof(png_byte) * screen_width);

    int palsize;
    uint8 *pal = File::loadFile("hpal02.dat", palsize);
    assert(palsize == 768);

    for (int i = 0; i < palsize / 3; ++i) {
        uint8 r = pal[i * 3 + 0];
        uint8 g = pal[i * 3 + 1];
        uint8 b = pal[i * 3 + 2];

        palette[i].red = (r << 2) | (r >> 4);
        palette[i].green = (g << 2) | (g >> 4);
        palette[i].blue = (b << 2) | (b >> 4);
	}

    for (int i = 1; i < 100; i++) {
        char tmp[100];
        sprintf(tmp, "data/map%02d.dat", i);
        FILE *f = fopen(tmp, "r");

        if (f == NULL)
            continue;

        fclose(f);

        if (!maps.loadMap(i))
            continue;

        clear_screen();
        printf("drawing map %i\n", i);
        maps.drawMap(i, screen_width / 2, screen_height / 2);
        write_png(i);
    }

    delete screen;
    delete app;
    return 0;
}

App::App() {

}

Screen::Screen(int width, int height) {

}

Screen::~Screen() {

}

int Screen::gameScreenHeight() {
    return screen_height;
}

int Screen::gameScreenWidth() {
    return screen_width;
}

int Screen::gameScreenLeftMargin() {
    return 0;
}

void Screen::blit(int x, int y, int width, int height, const uint8 *pixeldata,
        bool flipped, int stride) {
    width_ = screen_width;
    height_ = screen_height;

    if (x + width < 0 || y + height < 0 || x >= width_ || y >= height_)
        return;

    int dx = x < 0 ? 0 : x;
    int dy = y < 0 ? 0 : y;

    int sx = x < 0 ? -x : 0;
    int sy = y < 0 ? -y : 0;

    int w = x < 0 ? x + width : x + width > width_ ? width_ - x : width;
    int h = y < 0 ? y + height : y + height > height_ ? height_ - y : height;

    stride = (stride == 0 ? width : stride);
    int ofs = flipped ? w - 1 : 0;
    int inc = flipped ? -1 : 1;

    for (int j = 0; j < h; ++j) {
        uint8 *d = screen_data[dy + j] + dx + ofs;
        const uint8 *s = pixeldata + (sy + j) * stride + sx;

        for (int i = 0; i < w; ++i) {
            uint8 c = *s++;

            if (c != 255)
                *d = c;

            d += inc;
        }
    }
}

void Screen::scale2x(int x, int y, int width, int height,
        const uint8 *pixeldata, int stride, bool transp) {

}
