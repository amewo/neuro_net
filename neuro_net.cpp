#include <boost/lexical_cast.hpp>

#include "neuro_net.h"

//-----------------------------------------------------------------------------
neuro_net::neuro_net() noexcept
{
}
//-----------------------------------------------------------------------------
neuro_net::~neuro_net() noexcept
{
    for(auto neu : m_neurons)
    {
        delete neu;
    }
}
//-----------------------------------------------------------------------------
const std::string& neuro_net::get_name() const noexcept
{
    return m_name;
}
//-----------------------------------------------------------------------------
void neuro_net::set_name(const std::string& name) noexcept
{
    m_name = name;
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
uint32_t neuro_net::get_in_size() const noexcept
{
    return m_in_neurons.size();
}
//-----------------------------------------------------------------------------
uint32_t neuro_net::get_out_size() const noexcept
{
    return m_out_neurons.size();
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
void neuro_net::mark_neurons_as_input(const std::vector<uint32_t>& ids) throw(std::runtime_error)
{
    m_in_neurons.clear();

    for( auto id : ids )
    {
        if( mark_neuron_as_input(id) == false )
        {
            throw std::runtime_error(std::string("can't mark neuron - ") + boost::lexical_cast<std::string>(id) + "as input");
        }
    }
}
//-----------------------------------------------------------------------------
void neuro_net::mark_neurons_as_output(const std::vector<uint32_t>& ids) throw(std::runtime_error)
{
    m_out_neurons.clear();

    for( auto id : ids )
    {
        if( mark_neuron_as_output(id) == false )
        {
            throw std::runtime_error(std::string("can't mark neuron - ") + boost::lexical_cast<std::string>(id) + "as output");
        }
    }
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
const std::vector<neuron*>& neuro_net::get_input_neurons() const noexcept
{
    return m_in_neurons;
}
//-----------------------------------------------------------------------------
const std::vector<neuron*>& neuro_net::get_output_neurons() const noexcept
{
    return m_out_neurons;
}
//-----------------------------------------------------------------------------
std::vector<uint32_t> neuro_net::get_ids_of_input_neurons() const noexcept
{
    std::vector<uint32_t> ids;

    for( auto neu : m_in_neurons )
    {
        ids.push_back(neu->id());
    }

    return ids;
}
//-----------------------------------------------------------------------------
std::vector<uint32_t> neuro_net::get_ids_of_output_neurons() const noexcept
{
    std::vector<uint32_t> ids;

    for( auto neu : m_out_neurons )
    {
        ids.push_back(neu->id());
    }

    return ids;
}
//-----------------------------------------------------------------------------
bool neuro_net::link_neurons(uint32_t id_from, uint32_t id_to, float w) noexcept
{
    neuron *neu_from = get_neuron(id_from);
    neuron *neu_to = get_neuron(id_to);

    if( neu_from == nullptr || neu_to == nullptr )
    {
        return false;
    }

    return neu_to->add_link(neu_from, w);
}
//-----------------------------------------------------------------------------
void neuro_net::link_neurons(const std::vector<uint32_t>& ids_from,
                             const std::vector<uint32_t>& ids_to,
                             float w
                             ) throw (std::runtime_error)
{
    for( auto id_to : ids_to )
    {
        neuron *neu_to = get_neuron(id_to);

        if( neu_to == nullptr )
        {
            throw std::runtime_error(std::string("bad neuron id - ") + boost::lexical_cast<std::string>(id_to));
        }

        for( auto id_from : ids_from )
        {
            neuron *neu_from = get_neuron(id_from);

            if( neu_from == nullptr )
            {
                throw std::runtime_error(std::string("bad neuron id - ") + boost::lexical_cast<std::string>(neu_from));
            }

            bool res = neu_to->add_link(neu_from, w);

            if( res == false )
            {
                throw std::runtime_error(std::string("error link neuron - ") + boost::lexical_cast<std::string>(neu_to) +
                                         std::string(" with neuron ") + boost::lexical_cast<std::string>(neu_from));
            }
        }
    }
}
//-----------------------------------------------------------------------------
void neuro_net::link_neurons(const std::vector<uint32_t>& ids_from,
                             const std::vector<uint32_t>& ids_to,
                             const std::vector<std::vector<float>>& ws
                             ) throw (std::runtime_error)
{
    if( ws.size() != ids_to.size() )
    {
        throw std::runtime_error("error size of vector ws");
    }

    for( auto& w : ws )
    {
        if( w.size() != ids_from.size() )
        {
            throw std::runtime_error("error size of vector ws");
        }
    }

    size_t i = 0;

    for( auto id_to : ids_to )
    {
        neuron *neu_to = get_neuron(id_to);

        if( neu_to == nullptr )
        {
            throw std::runtime_error(std::string("bad neuron id - ") + boost::lexical_cast<std::string>(id_to));
        }

        size_t j = 0;

        for( auto id_from : ids_from )
        {
            neuron *neu_from = get_neuron(id_from);

            if( neu_from == nullptr )
            {
                throw std::runtime_error(std::string("bad neuron id - ") + boost::lexical_cast<std::string>(neu_from));
            }

            bool res = neu_to->add_link(neu_from, ws[i][j]);

            if( res == false )
            {
                throw std::runtime_error(std::string("error link neuron - ") + boost::lexical_cast<std::string>(neu_to) +
                                         std::string(" with neuron ") + boost::lexical_cast<std::string>(neu_from));
            }

            ++j;
        }

        ++i;
    }
}
//-----------------------------------------------------------------------------
bool neuro_net::unlink_neurons(uint32_t id_from, uint32_t id_to) noexcept
{
    neuron *neu_from = get_neuron(id_from);
    neuron *neu_to = get_neuron(id_to);

    if( neu_from == nullptr || neu_to == nullptr )
    {
        return false;
    }

    return neu_to->del_link(neu_from);
}
//-----------------------------------------------------------------------------
void unlink_neurons(const std::vector<uint32_t>& ids_from,
                    const std::vector<uint32_t>& ids_to
                    ) throw (std::runtime_error)
{
    //todo:???
}
//-----------------------------------------------------------------------------
void neuro_net::calc_signal() noexcept
{
    for(auto neu : m_neurons)
    {
        neu->calc_signal();
    }
}
//-----------------------------------------------------------------------------
void neuro_net::save_state(neuro_net_state *state) const noexcept
{
    state->Clear();

    state->set_name(m_name);

    for(auto neu : m_neurons)
    {
        neuron_state *neu_state = state->add_neurons();
        neu->save_state(neu_state);
    }

    for( auto neu : m_in_neurons )
    {
        state->add_in_neurons_ids(neu->id());
    }

    for( auto neu : m_out_neurons )
    {
        state->add_out_neurons_ids(neu->id());
    }
}
//-----------------------------------------------------------------------------
void neuro_net::restore_state(const neuro_net_state *state) throw (std::runtime_error)
{
    for( auto neu : m_neurons )
    {
        delete neu;
    }

    m_neurons.clear();

    m_name = state->name();

    std::vector<std::vector<uint32_t>> neu_links_ids;
    std::vector<std::vector<neuron*>> neu_links_neus;

    // Файбрика для создания нейронов.
    neuron_factory& factory = neuron_factory::instance();

    // Сначала восстановим нейроны и их состояния без восстановления связей.
    for( int i = 0; i < state->neurons_size(); ++i )
    {
        const neuron_state& neu_state = state->neurons(i);
        neuron *neu = factory.create_neuron((neuron_type) neu_state.type(), neu_state.id());

        if( neu == nullptr )
        {
            throw std::runtime_error("can't restore neuro_net state - error create neuron");
        }

        add_neuron(neu);
        neu_links_ids.push_back(neu->restore_state(&neu_state));
    }

    // Теперь на основе векторов идентификаторов построим вектор нейронов с соответствующими идернтификаторами.
    for( auto& links_ids : neu_links_ids )
    {
        std::vector<neuron*> links_neus;

        for( auto id : links_ids )
        {
            neuron *neu = get_neuron(id);

            if( neu == nullptr )
            {
                throw std::runtime_error("can't restore neuro_net state - error get neuron with id " +
                                         boost::lexical_cast<std::string>(id));
            }

            links_neus.push_back(neu);
        }

        neu_links_neus.push_back(links_neus);
    }

    // Восстановим связи нейронов.
    for( int i = 0; i < state->neurons_size(); ++i )
    {
        const neuron_state& neu_state = state->neurons(i);
        neuron *neu = get_neuron(neu_state.id());

        neu->restore_links_state(&neu_state, neu_links_neus[i]);
    }

    restore_input_neurons_state(state);
    restore_output_neurons_state(state);
}
//-----------------------------------------------------------------------------
void neuro_net::restore_input_neurons_state(const neuro_net_state *state) throw (std::runtime_error)
{
    m_in_neurons.clear();

    std::vector<uint32_t> ids;

    for( int i = 0; i < state->in_neurons_ids_size(); ++i )
    {
        ids.push_back(state->in_neurons_ids(i));
    }

    mark_neurons_as_input(ids);
}
//-----------------------------------------------------------------------------
void neuro_net::restore_output_neurons_state(const neuro_net_state *state) throw (std::runtime_error)
{
    m_out_neurons.clear();

    std::vector<uint32_t> ids;

    for( int i = 0; i < state->out_neurons_ids_size(); ++i )
    {
        ids.push_back(state->out_neurons_ids(i));
    }

    mark_neurons_as_output(ids);
}
//-----------------------------------------------------------------------------
