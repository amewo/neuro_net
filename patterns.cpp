#include "patterns.h"

//-----------------------------------------------------------------------------
pattern::pattern(std::size_t in_size, std::size_t out_size) noexcept
    : m_in_size(in_size)
    , m_out_size(out_size)
    , m_in(in_size, 0.0f)
    , m_out(out_size, 0.0f)
{
}
//-----------------------------------------------------------------------------
std::size_t pattern::get_in_size() const noexcept
{
    return m_in_size;
}
//-----------------------------------------------------------------------------
std::size_t pattern::get_out_size() const noexcept
{
    return m_out_size;
}
//-----------------------------------------------------------------------------
const std::vector<float> pattern::get_in() const noexcept
{
    return m_in;
}
//-----------------------------------------------------------------------------
const std::vector<float> pattern::get_out() const noexcept
{
    return m_out;
}
//-----------------------------------------------------------------------------
bool pattern::set_in(const std::vector<float> in) noexcept
{
    if( in.size() != m_in_size )
    {
        return false;
    }

    m_in = in;

    return true;
}
//-----------------------------------------------------------------------------
bool pattern::set_out(const std::vector<float> out) noexcept
{
    if( out.size() != m_out_size )
    {
        return false;
    }

    m_out = out;

    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
patterns::patterns(std::size_t in_size, std::size_t out_size) noexcept
    : m_in_size(in_size)
    , m_out_size(out_size)
{
}
//-----------------------------------------------------------------------------
std::size_t patterns::get_in_size() const noexcept
{
    return m_in_size;
}
//-----------------------------------------------------------------------------
std::size_t patterns::get_out_size() const noexcept
{
    return m_out_size;
}
//-----------------------------------------------------------------------------
std::size_t patterns::get_size() const noexcept
{
    return m_patterns.size();
}
//-----------------------------------------------------------------------------
pattern& patterns::get_pattern(std::size_t indx) throw (std::range_error)
{
    if( indx >= m_patterns.size() )
    {
        throw std::range_error("indx > m_patterns.size()");
    }

    return m_patterns[indx];
}
//-----------------------------------------------------------------------------
pattern& patterns::operator[](std::size_t indx) throw (std::range_error)
{
    if( indx >= m_patterns.size() )
    {
        throw std::range_error("indx > m_patterns.size()");
    }

    return m_patterns[indx];
}
//-----------------------------------------------------------------------------
