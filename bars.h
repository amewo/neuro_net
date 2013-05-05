#ifndef bars_h
#define bars_h

#include <stdint.h>
#include <ctime>
#include <stdexcept>
#include <vector>

//-----------------------------------------------------------------------------
struct bar
{
    time_t time;

    float open;
    float high;
    float low;
    float close;
};
//-----------------------------------------------------------------------------
class bars
{
public:
    bars() noexcept;
    virtual ~bars() noexcept;

    size_t get_size() const noexcept;

    bar& get_bar(size_t indx) throw (std::range_error);
    bar& operator[](size_t indx) throw (std::range_error);

    void push_back(const bar& b) noexcept;

    void insert(std::size_t indx, const bar& b) throw (std::range_error);

    void erase(size_t indx) throw (std::range_error);

    bool load_from_csv(const std::string& file_name) noexcept;
protected:
    std::vector<bar> m_bars;
};
//-----------------------------------------------------------------------------

#endif // #define bars_h
