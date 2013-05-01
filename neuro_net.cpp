#include "neuro_net.h"

neuro_net::neuro_net() noexcept
{
}

neuro_net::~neuro_net() noexcept
{
    for(auto neu : m_neurons)
    {
        delete neu;
    }
}

bool neuro_net::add_neuron(neuron *neu) noexcept
{
    auto neu_iter = m_neurons.begin();

    while( neu_iter != m_neurons.end() )
    {
        if( (*neu_iter)->id() >= neu->id() )
        {
            if( (*neu_iter)->id() == neu->id() )
            {
                return false;
            }

            break;
        }

        ++neu_iter;
    }

    m_neurons.insert(neu_iter, neu);

    return true;
}

bool neuro_net::del_neuron(uint32_t id) noexcept
{
    auto neu_iter = m_neurons.begin();

    while( neu_iter != m_neurons.end() )
    {
        if( (*neu_iter)->id() == id )
        {
            delete (*neu_iter);
            m_neurons.erase(neu_iter);

            return false;
        }

        ++neu_iter;
    }

    return false;
}

neuron* neuro_net::get_neuron(uint32_t id) noexcept
{
    for(auto neu : m_neurons)
    {
        if( neu->id() == id )
        {
            return neu;
        }
    }

    return nullptr;
}
