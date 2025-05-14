#include <iostream>
#include <fstream>
#include <cmath>

#include "sweep.h"
#include "display.h"

int main()
{
    iscan::colormap_s map {.min = 0, .max = 6, .min_hue = 180, .max_hue = 0};
    // iscan::sweep cin_sweep(std::cin);
    std::ifstream in_file;
    in_file.open("./../scans/csv/eyescanGTY_0_0_0_2023-Mar-28_13:48:56.csv");
    iscan::sweep file_sweep(in_file);

    std::cout << "Sweep dimensions (w, h): " << file_sweep.get_width() << " " << file_sweep.get_height() << '\n';
    std::cout << "Terminal dimensions (w, h): " << ftxui::Dimension::Full().dimx << " " << ftxui::Dimension::Full().dimy << '\n';

    constexpr int sweep_min_w = 32;

    // Read columns as a CLI argument
    int columns = 2;
    int term_w = ftxui::Dimension::Full().dimx;
    int sweep_w = term_w / columns;
    
    auto document = iscan::draw_sweep(file_sweep.scale(31, 32), map);
    ftxui::Screen screen(40, 40);
    ftxui::Render(screen, document);

    screen.Print();
    return 0;
}
