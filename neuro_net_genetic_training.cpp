#include "neuro_net_genetic_training.h"

//-----------------------------------------------------------------------------
time_stamp_distributor::time_stamp_distributor(uint32_t begin_free_node_time_stamp, uint32_t begin_free_link_time_stamp) noexcept
    : m_begin_free_node_time_stamp(begin_free_node_time_stamp)
    , m_begin_free_link_time_stamp(begin_free_link_time_stamp)
    , m_free_node_time_stamp(0)
    , m_free_link_time_stamp(0)
{
    reset();
}
//-----------------------------------------------------------------------------
void time_stamp_distributor::reset() noexcept
{
    m_node_time_stampes.clear();
    m_link_time_stampes.clear();

    m_free_node_time_stamp = m_begin_free_node_time_stamp;
    m_free_link_time_stamp = m_begin_free_link_time_stamp;
}
//-----------------------------------------------------------------------------
uint32_t time_stamp_distributor::get_time_stamp_for_node(uint32_t link_time_stamp,
                                                         uint32_t& link_in_time_stamp,
                                                         uint32_t& link_out_time_stamp)
{
    auto it = m_node_time_stampes.find(link_time_stamp);

    uint32_t node_time_stamp;

    if( it == m_node_time_stampes.end() )
    {
        node_time_stamp_struct _node_time_stamp_struct;

        node_time_stamp = m_free_node_time_stamp;
        ++m_free_node_time_stamp;

        link_in_time_stamp = m_free_link_time_stamp;
        ++m_free_link_time_stamp;

        link_out_time_stamp = m_free_link_time_stamp;
        ++m_free_link_time_stamp;

        _node_time_stamp_struct.time_stamp = node_time_stamp;
        _node_time_stamp_struct.link_in_time_stamp = link_in_time_stamp;
        _node_time_stamp_struct.link_out_time_stamp = link_out_time_stamp;

        m_node_time_stampes[link_time_stamp] = _node_time_stamp_struct;
    }
    else
    {
        node_time_stamp = it->second.time_stamp;

        link_in_time_stamp = it->second.link_in_time_stamp;
        link_out_time_stamp = it->second.link_out_time_stamp;
    }

    return node_time_stamp;
}
//-----------------------------------------------------------------------------
uint32_t time_stamp_distributor::get_time_stamp_for_link(uint32_t node_in_time_stamp, uint32_t node_out_time_stamp)
{
    auto it = m_link_time_stampes.find(node_in_time_stamp);

    uint32_t link_time_stamp;

    if( it == m_link_time_stampes.end() )
    {
        link_time_stamp = m_free_link_time_stamp;
        ++m_free_link_time_stamp;

        m_link_time_stampes[node_in_time_stamp][node_out_time_stamp] = link_time_stamp;
    }
    else
    {
        std::map<uint32_t, uint32_t>& map = it->second;

        auto it2 = map.find(node_out_time_stamp);

        if( it2 == map.end() )
        {
            link_time_stamp = m_free_link_time_stamp;
            ++m_free_link_time_stamp;

            map[node_out_time_stamp] = link_time_stamp;
        }
        else
        {
            link_time_stamp = it2->second;
        }
    }

    return link_time_stamp;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
population::population(uint32_t population_size, uint32_t in_signal_size, uint32_t out_signal_size) throw(std::runtime_error)
    : m_in_signal_size(in_signal_size)
    , m_out_signal_size(out_signal_size)
    , m_training_patterns(in_signal_size, out_signal_size)
    , m_time_stamp_distributor(in_signal_size + out_signal_size, in_signal_size * out_signal_size)
{
    reset(population_size);
}
//-----------------------------------------------------------------------------
void population::reset(uint32_t population_size) noexcept
{
    m_time_stamp_distributor.reset();
    m_individuals.clear();

    for( uint32_t i = 0; i < population_size; ++i )
    {
        individual new_individual;

        new_individual.nodes.reserve(m_in_signal_size + m_out_signal_size);
        new_individual.calc_queue.reserve(m_in_signal_size + m_out_signal_size);

        new_individual.links.reserve(m_in_signal_size * m_out_signal_size);

        for( uint32_t cur_in_node = 0; cur_in_node < m_in_signal_size; ++cur_in_node )
        {
            node new_node;

            new_node.activation_func = node_activation_func_type::linear_activation_func;
            new_node.type = node_type::input_node;

            new_node.time_stamp = cur_in_node;

            new_node.bias = 0.0f;
            new_node.sum = 0.0f;
            new_node.signal = 0.0f;

            new_individual.nodes.push_back(new_node);
        }

        for( uint32_t cur_out_node = m_in_signal_size; cur_out_node < m_in_signal_size + m_out_signal_size; ++cur_out_node )
        {
            node new_node;

            new_node.activation_func = node_activation_func_type::hyperbolic_activation_func;
            new_node.type = node_type::output_node;

            new_node.time_stamp = cur_out_node;

            new_node.bias = 0.0f;
            new_node.sum = 0.0f;
            new_node.signal = 0.0f;

            new_individual.nodes.push_back(new_node);
            new_individual.calc_queue.push_back(cur_out_node);
        }

        uint32_t link_time_stamp = 0;

        for( uint32_t cur_in_node = 0; cur_in_node < m_in_signal_size; ++cur_in_node )
        {
            for( uint32_t cur_out_node = m_in_signal_size; cur_out_node < m_in_signal_size + m_out_signal_size; ++cur_out_node )
            {
                link new_link;

                new_link.time_stamp = link_time_stamp;
                new_link.in = cur_in_node;
                new_link.out = cur_out_node;

                new_link.w = 0.0f;

                new_link.enabled = true;

                ++link_time_stamp;

                new_individual.links.push_back(new_link);
            }
        }

        m_individuals.push_back(new_individual);
    }
}
//-----------------------------------------------------------------------------
bool population::set_training_patterns(patterns &ptrns) noexcept
{
    if( m_training_patterns.get_in_size() != ptrns.get_in_size() ||
        m_training_patterns.get_out_size() != ptrns.get_out_size()
      )
    {
        return false;
    }

    m_training_patterns = ptrns;

    return true;
}
//-----------------------------------------------------------------------------
