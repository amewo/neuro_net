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
void pattern::save_state(pattern_state *state) noexcept
{
    state->Clear();

    for( auto in_val : m_in )
    {
        state->add_in(in_val);
    }

    for( auto out_val : m_out )
    {
        state->add_out(out_val);
    }
}
//-----------------------------------------------------------------------------
void pattern::restore_state(const pattern_state *state) noexcept
{
    m_in.clear();
    m_out.clear();

    m_in_size = state->in_size();
    m_out_size = state->out_size();

    for( int i = 0; i < state->in_size(); ++i )
    {
        m_in.push_back(state->in(i));
    }

    for( int i = 0; i < state->out_size(); ++i )
    {
        m_out.push_back(state->out(i));
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
patterns::patterns(std::size_t in_size, std::size_t out_size) noexcept
    : m_in_size(in_size)
    , m_out_size(out_size)
{
}
//-----------------------------------------------------------------------------
const std::string& patterns::get_name() const noexcept
{
    return m_name;
}
//-----------------------------------------------------------------------------
void patterns::set_name(const std::string& name) noexcept
{
    m_name = name;
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
pattern& patterns::get(std::size_t indx) throw (std::range_error)
{
    if( indx >= m_patterns.size() )
    {
        throw std::range_error("indx >= m_patterns.size()");
    }

    return m_patterns[indx];
}
//-----------------------------------------------------------------------------
pattern& patterns::operator[](std::size_t indx) throw (std::range_error)
{
    if( indx >= m_patterns.size() )
    {
        throw std::range_error("indx >= m_patterns.size()");
    }

    return m_patterns[indx];
}
//-----------------------------------------------------------------------------
void patterns::push_back(const pattern& p) noexcept
{
    m_patterns.push_back(p);
}
//-----------------------------------------------------------------------------
void patterns::insert(std::size_t indx, const pattern& p) throw (std::range_error)
{
    if( indx > m_patterns.size() )
    {
        throw std::range_error("can't insert pattern - error indx");
    }

    m_patterns.insert(m_patterns.begin() + indx, p);
}
//-----------------------------------------------------------------------------
void patterns::erase(size_t indx) throw (std::range_error)
{
    if( indx >= m_patterns.size() )
    {
        throw std::range_error("can't erase pattern - error indx");
    }

    m_patterns.erase(m_patterns.begin() + indx);
}
//-----------------------------------------------------------------------------
void patterns::save_state(patterns_state *state) noexcept
{
    state->Clear();

    state->set_name(m_name);

    state->set_size_of_in(m_in_size);
    state->set_size_of_out(m_out_size);

    for( auto& cur_pattern : m_patterns )
    {
        pattern_state *cur_pattern_state = state->add_patterns();

        cur_pattern.save_state(cur_pattern_state);
    }
}
//-----------------------------------------------------------------------------
void patterns::restore_state(const patterns_state *state) throw (std::runtime_error)
{
    m_patterns.clear();

    m_in_size = state->size_of_in();
    m_out_size = state->size_of_out();

    for( int i = 0; i < state->patterns_size(); ++i )
    {
        const pattern_state& cur_pattern_state = state->patterns(i);

        if( cur_pattern_state.in_size() != (int) m_in_size )
        {
            throw std::runtime_error("can't restore patterns state - cur_pattern_state.in_size() != (int) m_in_size");
        }
        else if( cur_pattern_state.out_size() != (int) m_out_size )
        {
            throw std::runtime_error("can't restore patterns state - cur_pattern_state.out_size() != (int) m_out_size");
        }

        pattern cur_pattern(m_in_size, m_out_size);
        cur_pattern.restore_state(&cur_pattern_state);

        push_back(cur_pattern);
    }
}
//-----------------------------------------------------------------------------
