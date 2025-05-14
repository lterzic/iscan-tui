#include "sweep.h"
#include <cmath>
#include <string>
#include <sstream>

namespace iscan {

template <typename num_type>
static std::vector<num_type> read_number_list(std::istream& input)
{
    std::vector<num_type> vector;
    num_type num;
    char separator;

    while (input >> num) {
        vector.push_back(num);
        input >> separator;
    }
    return vector;
}

sweep::sweep(std::istream& input)
{
    std::string tmp;
    
    // Skip first 10 lines
    for (int i = 0; i < 10; i++) std::getline(input, tmp);

    // Read the scan type from the beginning of the line (can be 1d or 2d)
    // and then read (skip) the rest of the line as it's not needed
    std::string scan_type;
    input >> scan_type;
    std::getline(input, tmp);

    // Parse each line into a row of the sweep
    while (std::getline(input, tmp)) {
        if (tmp == "Scan End")
            break;
        
        // Convert the line into a stream and read all the numbers
        std::istringstream line_stream(tmp);
        auto line_data = read_number_list<float>(line_stream);
        
        // First number is the line index
        line_data.erase(line_data.begin());

        m_data.push_back(line_data);
    }
    
    m_width = m_data[0].size();
    m_height = m_data.size();

    // Make sure that all rows are the same size
    for (const auto& row : m_data)
        assert(row.size() == m_width);
}

static sweep::sample_t avg_mask(const sweep& sweep, int hor, int ver)
{
    constexpr int size = 1;
    sweep::sample_t result(0);

    size_t hor_low = std::max(0, hor - size);
    size_t ver_low = std::max(0, ver - size);
    size_t hor_high = std::min((int)sweep.get_width(), hor + size + 1);
    size_t ver_high = std::min((int)sweep.get_height(), ver + size + 1);

    for (size_t r = ver_low; r < ver_high; r++) {
        for (size_t c = hor_low; c < hor_high; c++) {
            result += sweep.get_sample(c, r);
        }
    }
    return result / sweep::sample_t((hor_high - hor_low) * (ver_high - ver_low));
}

sweep sweep::scale(size_t width, size_t height) const
{
    // Only downscaling allowed
    assert(width <= m_width);
    assert(height <= m_height);

    // Create a sweep with the new dimensions
    sweep scaled;
    scaled.m_width = width;
    scaled.m_height = height;
    scaled.m_data = data_t(height, row_t(width, 0));

    // Multiply a new position with these coefficients
    // to get the position in the original sweep
    float hor_scale = m_width / (float)width;
    float ver_scale = m_height / (float)height;

    for (size_t r = 0; r < height; r++) {
        for (size_t c = 0; c < width; c++) {
            size_t orig_r = std::round(r * ver_scale);
            size_t orig_c = std::round(c * hor_scale);
            // Get the average around (orig_r, orig_c)
            scaled.m_data[r][c] += avg_mask(*this, orig_c, orig_r);
        }
    }
    return scaled;
}

}