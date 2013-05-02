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
            if( neuron_is_input(id) || neuron_is_output(id) )
            {
                return false;
            }

            delete (*neu_iter);
            m_neurons.erase(neu_iter);

            return true;
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

uint32_t neuro_net::get_in_size() const noexcept
{
    return m_in_neurons.size();
}

uint32_t neuro_net::get_out_size() const noexcept
{
    return m_out_neurons.size();
}

bool neuro_net::mark_neuron_as_input(uint32_t id) noexcept
{
    //todo: добавить проверку по типу нейрона. runtime_cast.

    neuron *neu = get_neuron(id);

    if( neu == nullptr || neuron_is_input(id) )
    {
        return false;
    }

    m_in_neurons.push_back(neu);

    return true;
}

bool neuro_net::mark_neuron_as_output(uint32_t id) noexcept
{
    neuron *neu = get_neuron(id);

    if( neu==nullptr || neuron_is_output(id) )
    {
        return false;
    }

    m_out_neurons.push_back(neu);

    return true;
}

bool neuro_net::unmark_neuron_as_input(uint32_t id) noexcept
{
    auto neu_iter = m_in_neurons.begin();

    while( neu_iter != m_in_neurons.end() )
    {
        if( (*neu_iter)->id() == id )
        {
            m_in_neurons.erase(neu_iter);

            return true;
        }

        ++neu_iter;
    }

    return false;
}

bool neuro_net::unmark_neuron_as_output(uint32_t id) noexcept
{
    auto neu_iter = m_out_neurons.begin();

    while( neu_iter != m_out_neurons.end() )
    {
        if( (*neu_iter)->id() == id )
        {
            m_out_neurons.erase(neu_iter);

            return true;
        }

        ++neu_iter;
    }

    return false;
}

bool neuro_net::neuron_is_input(uint32_t id) const noexcept
{
    for( auto neu : m_in_neurons )
    {
        if( neu->id() == id )
        {
            return true;
        }
    }

    return false;
}

bool neuro_net::neuron_is_output(uint32_t id) const noexcept
{
    for( auto neu : m_out_neurons )
    {
        if( neu->id() == id )
        {
            return true;
        }
    }

    return false;
}

const std::vector<neuron*>& neuro_net::get_input_neurons() const noexcept
{
    return m_in_neurons;
}

const std::vector<neuron*>& neuro_net::get_output_neurons() const noexcept
{
    return m_out_neurons;
}

std::vector<uint32_t> neuro_net::get_ids_of_input_neurons() const noexcept
{
    std::vector<uint32_t> ids;

    for( auto neu : m_in_neurons )
    {
        ids.push_back(neu->id());
    }

    return ids;
}

std::vector<uint32_t> neuro_net::get_ids_of_output_neurons() const noexcept
{
    std::vector<uint32_t> ids;

    for( auto neu : m_out_neurons )
    {
        ids.push_back(neu->id());
    }

    return ids;
}

void neuro_net::calc_signal() noexcept
{
    for(auto neu : m_neurons)
    {
        neu->calc_signal();
    }
}
