#ifndef patterns_h
#define patterns_h

#include <vector>

//-----------------------------------------------------------------------------
class pattern
{
public:
    pattern(std::size_t in_size, std::size_t out_size) noexcept;

    std::size_t get_in_size() const noexcept;
    std::size_t get_out_size() const noexcept;

    const std::vector<float> get_in() const noexcept;
    const std::vector<float> get_out() const noexcept;

    bool set_in(const std::vector<float> in) noexcept;
    bool set_out(const std::vector<float> out) noexcept;

protected:
    std::size_t m_in_size;
    std::size_t m_out_size;

    std::vector<float> m_in;
    std::vector<float> m_out;
};
//-----------------------------------------------------------------------------
class patterns
{
public:
    patterns(std::size_t in_size, std::size_t out_size) noexcept;

    std::size_t get_in_size() const noexcept;
    std::size_t get_out_size() const noexcept;

    std::size_t get_size() const noexcept;

protected:
    std::size_t m_in_size;
    std::size_t m_out_size;

    std::vector<pattern> m_patterns;
};
//-----------------------------------------------------------------------------

#endif // #define patterns_h
