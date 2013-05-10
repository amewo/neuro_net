#include <cmath>
#include <list>

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
void population::cross_parents(const individual &p1, const individual &p2, individual &offspring) noexcept
{
    offspring.nodes.clear();
    offspring.links.clear();
    offspring.calc_queue.clear();

    offspring.nodes.reserve(p1.nodes.size() + p2.nodes.size());
    offspring.links.reserve(p1.links.size() + p2.links.size());
    offspring.calc_queue.reserve(p1.calc_queue.size() + p2.calc_queue.size());

    std::vector<uint32_t> p1ngp, p2ngp; // p1, p2 new genes pos

    p1ngp.reserve(p1.nodes.size());
    p2ngp.reserve(p2.nodes.size());

    uint32_t i = 0, j = 0, njp = 0;

    while( i < p1.nodes.size() && j < p2.nodes.size() )
    {
        if( p1.nodes[i].time_stamp == p2.nodes[j].time_stamp )
        {
            p1ngp.push_back(njp);
            p2ngp.push_back(njp);

            offspring.nodes.push_back(p1.nodes[i]);
            offspring.nodes[njp].bias = (p1.nodes[i].bias + p2.nodes[j].bias) / 2.0f;

            ++i, ++j, ++njp;
        }
        else
        {
            if( p1.nodes[i].time_stamp < p2.nodes[j].time_stamp )
            {
                p1ngp.push_back(njp);

                offspring.nodes.push_back(p1.nodes[i]);

                ++i, ++njp;
            }
            else
            {
                p2ngp.push_back(njp);

                offspring.nodes.push_back(p2.nodes[j]);

                ++j, ++njp;
            }
        }
    }

    while( i < p1.nodes.size() )
    {
        p1ngp.push_back(njp);

        offspring.nodes.push_back(p1.nodes[i]);

        ++i, ++njp;
    }

    while( j < p2.nodes.size() )
    {
        p2ngp.push_back(njp);

        offspring.nodes.push_back(p2.nodes[j]);

        ++j, ++njp;
    }

    i = 0, j = 0, njp = 0;

    while( i < p1.links.size() && j < p2.links.size() )
    {
        if( p1.links[i].time_stamp == p2.links[j].time_stamp )
        {
            offspring.links.push_back(p1.links[i]);

            link& nl = offspring.links[njp];

            nl.w = (p1.links[i].w + p2.links[j].w) / 2.0f;

            nl.in = p1ngp[nl.in];
            nl.out = p1ngp[nl.out];

            //todo: ???
            nl.enabled = p1.links[i].enabled || p2.links[j].enabled;

            ++i, ++j, ++njp;
        }
        else
        {
            if( p1.links[i].time_stamp < p2.links[j].time_stamp )
            {
                offspring.links.push_back(p1.links[i]);

                link& nl = offspring.links[njp];

                nl.in = p1ngp[nl.in];
                nl.out = p1ngp[nl.out];

                ++i, ++njp;
            }
            else
            {
                offspring.links.push_back(p2.links[j]);

                link& nl = offspring.links[njp];

                nl.in = p2ngp[nl.in];
                nl.out = p2ngp[nl.out];

                ++j, ++njp;
            }
        }
    }

    while( i < p1.links.size() )
    {
        offspring.links.push_back(p1.links[i]);

        link& nl = offspring.links[njp];

        nl.in = p1ngp[nl.in];
        nl.out = p1ngp[nl.out];

        ++i, ++njp;
    }

    while( j < p2.links.size() )
    {
        offspring.links.push_back(p2.links[j]);

        link& nl = offspring.links[njp];

        nl.in = p2ngp[nl.in];
        nl.out = p2ngp[nl.out];

        ++j, ++njp;
    }

    uint32_t ilst = 0, jlst = 0;
    i = 0, j = 0, njp = 0;

    while( i < p1.calc_queue.size() && j < p2.calc_queue.size() )
    {
        if( p1.nodes[p1.calc_queue[i]].time_stamp == p2.nodes[p2.calc_queue[j]].time_stamp )
        {
            for( uint32_t ii = ilst; ii <= i; ++i )
            {
                offspring.calc_queue.push_back( p1ngp[p1.calc_queue[ii]] );
            }

            for( uint32_t jj = jlst; jj <= j; ++j )
            {
                offspring.calc_queue.push_back( p2ngp[p2.calc_queue[jj]] );
            }

            ilst = i + 1;
            jlst = j + 1;
        }
        else
        {
            if( p1.nodes[p1.calc_queue[i]].time_stamp < p2.nodes[p2.calc_queue[j]].time_stamp )
            {
                ++i;
            }
            else
            {
                ++j;
            }
        }

        ++i, ++j;
    }

    while( ilst < p1.calc_queue.size() )
    {
        offspring.calc_queue.push_back(p1ngp[p1.calc_queue[ilst]]);

        ++ilst;
    }

    while( jlst < p2.calc_queue.size() )
    {
        offspring.calc_queue.push_back(p2ngp[p2.calc_queue[jlst]]);

        ++jlst;
    }

    rebuild_links_queue(offspring);
}
//-----------------------------------------------------------------------------
float population::calc_distance_between_parents(const individual& p1, const individual& p2, float c1, float c2, float c3) noexcept
{
    // Формула расстояния между индивидами.
    // E - число избыточных генов.
    // D - число непересекающихся генов.
    // N - размер наибольшего генома.

    float E = p1.links.size() > p2.links.size() ? (p1.links.size() - p2.links.size()) : (p2.links.size() - p1.links.size());

    float D = 0.0f;
    float Dcnt = 0.0f;

    uint32_t i = 0, j = 0;

    while( i < p1.links.size() && j < p2.links.size() )
    {
        if( p1.links[i].time_stamp == p2.links[j].time_stamp )
        {
            D  += std::abs(p1.links[i].w - p2.links[j].w);
            Dcnt += 1.0f;

            ++i;
            ++j;
        }
        else
        {
            D += 1.0f;

            if( p1.links[i].time_stamp < p2.links[j].time_stamp )
            {
                ++i;
            }
            else
            {
                ++j;
            }
        }
    }

    float N = p1.links.size() > p2.links.size() ? p1.links.size() : p2.links.size();

    float avgdw = Dcnt > 0.0f ? (D / Dcnt) : 0.0f;

    float distance = (c1 * E) / N + (c2 * D) / N + c3 * avgdw;

    return distance;
}
//-----------------------------------------------------------------------------
float population::calc_averaged_square_error(const individual &ndvdl) noexcept
{
    //

    return .0f;
}
//-----------------------------------------------------------------------------
void population::rebuild_links_queue(individual &p) noexcept
{
    p.links_queue.clear();
    p.links_queue.reserve(p.links.size());

    std::list<uint32_t> left_links;

    for( uint32_t i = 0; i < p.links.size(); ++i )
    {
        left_links.push_back(i);
    }

    for( uint32_t i = 0; i < p.calc_queue.size(); ++i )
    {
        auto link_it = left_links.begin();

        while( link_it != left_links.end() )
        {
            uint32_t link_num = *link_it;

            if( p.links[link_num].out == p.calc_queue[i] )
            {
                p.links_queue.push_back(link_num);

                link_it = left_links.erase(link_it);
            }
            else
            {
                ++link_it;
            }
        }
    }
}
//-----------------------------------------------------------------------------
void population::set_input_pattern(const pattern& ptrn, individual& p) noexcept
{
    const std::vector<float> &in = ptrn.get_in();

    for( uint32_t i = 0; i < in.size(); ++i )
    {
        p.nodes[i].signal = in[i];
    }
}
//-----------------------------------------------------------------------------
void population::reset_signals(individual& p) noexcept
{
    for( node& cur_node: p.nodes )
    {
        cur_node.sum = 0.0f;
        cur_node.signal = 0.0f;
    }
}
//-----------------------------------------------------------------------------
void population::calc_signals(const pattern &ptrn, individual& p) noexcept
{
    //
}
//-----------------------------------------------------------------------------