#include "neuron.h"

neuron::neuron(uint32_t id) noexcept
    : m_bias(0.0f)
    , m_sum(0.0f)
    , m_signal(0.0f)
    , m_id(id)
{
}

neuron::~neuron() noexcept
{
}

float neuron::bias() const noexcept
{
    return m_bias;
}

void neuron::set_bias(float bias) noexcept
{
    m_bias = bias;
}

float neuron::sum() const noexcept
{
    return m_sum;
}

float neuron::signal() const noexcept
{
    return m_signal;
}

void neuron::set_signal(float signal) noexcept
{
    m_signal = signal;
}

uint32_t neuron::id() const noexcept
{
    return m_id;
}

bool neuron::add_link(neuron *neu, float w) noexcept
{
    for(auto& link : m_in_links)
    {
        if( link.neu == neu )
        {
            return false;
        }
    }

    m_in_links.push_back(neuron_in_link(neu, w));

    return true;
}

bool neuron::del_link(neuron *neu) noexcept
{
    auto link = m_in_links.begin();

    while(link != m_in_links.end())
    {
        if( link->neu == neu )
        {
            m_in_links.erase(link);

            return false;
        }
    }

    return false;
}

float neuron::calc_sum() noexcept
{
    float sum = 0.0f;

    for(auto& link : m_in_links)
    {
        sum += link.neu->signal() * link.w;
    }

    m_sum = sum;

    return m_sum;
}

float neuron::calc_signal() noexcept
{
    return m_signal = activation_function(calc_sum());
}

float neuron::activation_function(float sum) const noexcept
{
    return sum;
}
