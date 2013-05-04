#include <cmath>

#include "neuron.h"

//-----------------------------------------------------------------------------
neuron::neuron(uint32_t id) noexcept
    : m_bias(0.0f)
    , m_sum(0.0f)
    , m_signal(0.0f)
    , m_id(id)
{
}
//-----------------------------------------------------------------------------
neuron::~neuron() noexcept
{
}
//-----------------------------------------------------------------------------
float neuron::bias() const noexcept
{
    return m_bias;
}
//-----------------------------------------------------------------------------
void neuron::set_bias(float bias) noexcept
{
    m_bias = bias;
}
//-----------------------------------------------------------------------------
float neuron::sum() const noexcept
{
    return m_sum;
}
//-----------------------------------------------------------------------------
float neuron::signal() const noexcept
{
    return m_signal;
}
//-----------------------------------------------------------------------------
void neuron::set_signal(float signal) noexcept
{
    m_signal = signal;
}
//-----------------------------------------------------------------------------
uint32_t neuron::id() const noexcept
{
    return m_id;
}
//-----------------------------------------------------------------------------
bool neuron::add_link(neuron *neu, float w) noexcept
{
    for( auto& link : m_links )
    {
        if( link.neu == neu )
        {
            return false;
        }
    }

    m_links.push_back(neuron_link(neu, w));

    return true;
}
//-----------------------------------------------------------------------------
bool neuron::del_link(neuron *neu) noexcept
{
    auto link_iter = m_links.begin();

    while( link_iter != m_links.end() )
    {
        if( link_iter->neu == neu )
        {
            m_links.erase(link_iter);

            return false;
        }

        ++link_iter;
    }

    return false;
}
//-----------------------------------------------------------------------------
float neuron::calc_sum() noexcept
{
    float sum = 0.0f;

    for( auto& link : m_links )
    {
        sum += link.neu->signal() * link.w;
    }

    m_sum = sum;

    return m_sum;
}
//-----------------------------------------------------------------------------
float neuron::calc_signal() noexcept
{
    return m_signal = activation_function(calc_sum());
}
//-----------------------------------------------------------------------------
float neuron::activation_function(float sum) const noexcept
{
    return sum;
}
//-----------------------------------------------------------------------------
void neuron::save_state(neuron_state *state) noexcept
{
    state->Clear();

    state->set_bias(m_bias);
    state->set_sum(m_sum);
    state->set_signal(m_signal);

    for( auto& link : m_links )
    {
        neuron_link_state *link_state = state->add_links();

        link_state->set_id(link.neu->id());
        link_state->set_w(link.w);
    }
}
//-----------------------------------------------------------------------------
std::vector<uint32_t> neuron::restore_state(const neuron_state *state) noexcept
{
    m_bias = state->bias();
    m_sum = state->sum();
    m_signal = state->signal();

    std::vector<uint32_t> ids;

    for( int i = 0; i < state->links_size(); ++i )
    {
        ids.push_back(state->links(i).id());
    }

    return ids;
}
//-----------------------------------------------------------------------------
void neuron::restore_links_state(const neuron_state *state, const std::vector<neuron*> neus) throw(std::runtime_error)
{
    m_links.clear();

    if( state->links_size() != (int) neus.size() )
    {
        throw std::runtime_error("error size of vector \"neus\"");
    }

    for( int i = 0; i < state->links_size(); ++i )
    {
        const neuron_link_state &link_state = state->links(i);

        if( link_state.id() != neus[i]->id() )
        {
            throw std::runtime_error("can't restore neuron link - link_state.id() != neus[i]->id()");
        }

        if( add_link(neus[i], link_state.w()) == false )
        {
            throw std::runtime_error("can't restore neuron link - error add new link");
        }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
input_neuron::input_neuron(uint32_t id) noexcept
    : neuron(id)
{
}
//-----------------------------------------------------------------------------
float input_neuron::calc_sum() noexcept
{
    return m_sum;
}
//-----------------------------------------------------------------------------
float input_neuron::calc_signal() noexcept
{
    return m_signal;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
hyperbolic_neuron::hyperbolic_neuron(uint32_t id) noexcept
    : neuron(id)
{
}
//-----------------------------------------------------------------------------
float hyperbolic_neuron::activation_function(float sum) const noexcept
{
    return tanh(sum);
}
//-----------------------------------------------------------------------------
