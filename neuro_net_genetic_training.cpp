#include <iostream>
#include <cmath>
#include <list>
#include <algorithm>

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
                                                         uint32_t node_in_time_stamp,
                                                         uint32_t node_out_time_stamp,
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

        link_in_time_stamp = get_time_stamp_for_link(node_in_time_stamp, node_time_stamp);
        link_out_time_stamp = get_time_stamp_for_link(node_time_stamp, node_out_time_stamp);

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
    , m_time_stamp_distributor(in_signal_size + out_signal_size, 0)
    , m_mt19937(m_random_device())
{
    reset(population_size);
}
//-----------------------------------------------------------------------------
void population::set_params(const population_params &params) noexcept
{
    m_params = params;
}
//-----------------------------------------------------------------------------
const population_params& population::get_params() const noexcept
{
    return m_params;
}
//-----------------------------------------------------------------------------
void population::reset(uint32_t population_size) throw(std::runtime_error)
{
    if( population_size == 0 )
    {
        throw std::runtime_error("population_size == 0");
    }

    m_time_stamp_distributor.reset();
    m_individuals.clear();

    m_current_epoch = 0;

    for( uint32_t i = 0; i < population_size; ++i )
    {
        individual new_individual;
        reset_individual(new_individual);
        m_individuals.push_back(new_individual);
    }
}
//-----------------------------------------------------------------------------
void population::reset_individual(individual &p) throw(std::runtime_error)
{
    p.in_signal_size = m_in_signal_size;
    p.out_signal_size = m_out_signal_size;

    p.nodes.reserve(m_in_signal_size + m_out_signal_size);
    p.calc_queue.reserve(m_in_signal_size + m_out_signal_size);

    p.links.reserve(m_in_signal_size * m_out_signal_size);

    for( uint32_t cur_in_node = 0; cur_in_node < m_in_signal_size; ++cur_in_node )
    {
        node new_node;

        new_node.activation_func = node_activation_func_type::linear_activation_func;
        new_node.type = node_type::input_node;

        new_node.time_stamp = cur_in_node;

        new_node.bias = 0.0f;
        new_node.sum = 0.0f;
        new_node.signal = 0.0f;

        p.nodes.push_back(new_node);
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

        p.nodes.push_back(new_node);
        p.calc_queue.push_back(cur_out_node);
    }

    uint32_t link_time_stamp = 0;

    for( uint32_t cur_in_node = 0; cur_in_node < m_in_signal_size; ++cur_in_node )
    {
        for( uint32_t cur_out_node = m_in_signal_size; cur_out_node < m_in_signal_size + m_out_signal_size; ++cur_out_node )
        {
            link new_link;

            uint32_t in_node_time_stamp = p.nodes[cur_in_node].time_stamp;
            uint32_t out_node_time_stamp = p.nodes[cur_out_node].time_stamp;

            new_link.time_stamp = m_time_stamp_distributor.get_time_stamp_for_link(in_node_time_stamp, out_node_time_stamp);
            new_link.in = cur_in_node;
            new_link.out = cur_out_node;

            new_link.w = 0.0f;

            new_link.enabled = true;

            ++link_time_stamp;

            p.links.push_back(new_link);
        }
    }

    std::uniform_real_distribution<float> weight_dis(m_params.reset_weight_down, m_params.reset_weight_up);

    for( node& pn : p.nodes )
    {
        pn.bias = weight_dis(m_mt19937);
    }

    for( link& pl : p.links )
    {
        pl.w = weight_dis(m_mt19937);
    }

    rebuild_links_queue(p);
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
uint32_t population::get_current_epoch() const noexcept
{
    return m_current_epoch;
}
//-----------------------------------------------------------------------------
uint32_t population::get_species_num() const noexcept
{
    return m_species.size();
}
//-----------------------------------------------------------------------------
float population::get_least_error_val() const noexcept
{
    if( m_individuals.size() == 0 )
    {
        return 0.0f;
    }

    return std::min_element(m_individuals.begin(), m_individuals.end(), [](const individual &p1, const individual &p2) {
        return p1.fitness < p2.fitness;
    })->fitness;
}
//-----------------------------------------------------------------------------
uint32_t population::get_nodes_num_in_best() const noexcept
{
    if( m_individuals.size() == 0 )
    {
        return 0;
    }

    return std::min_element(m_individuals.begin(), m_individuals.end(), [](const individual &p1, const individual &p2) {
        return p1.fitness < p2.fitness;
    })->nodes.size();
}
//-----------------------------------------------------------------------------
void population::next_epoch() noexcept
{
    ++m_current_epoch;

    calc_all_fitness();
    split_into_species();
    correct_max_distance_between_species();

    // Тут проверить условия останова. Если не выполнены, то кроссоверы и мутации.
    if( false )
    {
        return;
    }

    transfer_best_individuals_from_species();
    do_mutations();
    cross_parents_in_species();

    std::swap(m_individuals, m_temporary_pool);

    calc_all_fitness();
}
//-----------------------------------------------------------------------------
void population::make_test() noexcept
{
    add_node(m_individuals[0], 0);
}
//-----------------------------------------------------------------------------
bool population::check_links(const individual &p) noexcept
{
    for( uint32_t i = 0; i < p.links.size(); ++i )
    {
        for( uint32_t j = 0; j < p.links.size(); ++j )
        {
            if( i != j )
            {
                if( p.links[i].time_stamp == p.links[j].time_stamp )
                {
                    return false;
                }
            }
        }
    }

    uint32_t ndx1 = 0;
    uint32_t ndx2 = 0;

    for( uint32_t i = 0; i < p.links.size(); ++i )
    {
        ndx1 = p.links[i].in;
        ndx2 = p.links[i].out;

        for( uint32_t j = 0; j < p.links.size(); ++j )
        {
            if( i != j )
            {
                if( p.links[j].in == ndx1 && p.links[j].out == ndx2 )
                {
                    return false;
                }
            }
        }
    }

    return true;
}
//-----------------------------------------------------------------------------
void population::cross_parents(const individual &p1, const individual &p2, individual &offspring) noexcept
{
    std::uniform_real_distribution<float> rate_dis(0.0f, 1.0f);

    offspring.nodes.clear();
    offspring.links.clear();
    offspring.calc_queue.clear();

    offspring.nodes.reserve(p1.nodes.size() + p2.nodes.size());
    offspring.links.reserve(p1.links.size() + p2.links.size());
    offspring.calc_queue.reserve(p1.calc_queue.size() + p2.calc_queue.size());

    offspring.in_signal_size = m_in_signal_size;
    offspring.out_signal_size = m_out_signal_size;

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

            if( rate_dis(m_mt19937) < 0.5f )
            {
                offspring.nodes[njp].bias = p1.nodes[i].bias;
            }
            else
            {
                offspring.nodes[njp].bias = p2.nodes[j].bias;
            }

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

            if( rate_dis(m_mt19937) < 0.5f )
            {
                nl.w = p1.links[i].w;
            }
            else
            {
                nl.w = p2.links[j].w;
            }

            nl.in = p1ngp[nl.in];
            nl.out = p1ngp[nl.out];

            if( p1.links[i].enabled && p2.links[j].enabled )
            {
                nl.enabled = true;
            }
            else if( p1.links[i].enabled || p2.links[j].enabled )
            {
                // Если у одного из родителей всеже включен ген веса,
                // то включим его в потомке с вероятностью m_enable_weight_rate.
                nl.enabled = ( rate_dis(m_mt19937) < m_params.enable_link_rate ) ? true : false;
            }

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

    uint32_t jj = 0;
    i = 0, j = 0;

    while( i < p1.calc_queue.size() && j < p2.calc_queue.size() )
    {
        uint32_t ii = i;

        bool found = false;

        while( ii < p1.calc_queue.size() )
        {
            jj = j;

            while( jj < p2.calc_queue.size() )
            {
                if( p1.nodes[p1.calc_queue[ii]].time_stamp == p2.nodes[p2.calc_queue[jj]].time_stamp )
                {
                    found = true;
                    break;
                }

                ++jj;
            }

            if( found == true )
            {
                break;
            }

            ++ii;
        }

        if( found == false )
        {
            break;
        }

        while( i < ii )
        {
            offspring.calc_queue.push_back(p1ngp[p1.calc_queue[i]]);

            ++i;
        }

        while( j < jj )
        {
            offspring.calc_queue.push_back(p2ngp[p2.calc_queue[j]]);

            ++j;
        }

        // Добавим в очередь только один одинаковый узел.
        offspring.calc_queue.push_back(p1ngp[p1.calc_queue[ii]]);

        ++i, ++j;
    }

    while( i < p1.calc_queue.size() )
    {
        offspring.calc_queue.push_back(p1ngp[p1.calc_queue[i]]);

        ++i;
    }

    while( j < p2.calc_queue.size() )
    {
        offspring.calc_queue.push_back(p2ngp[p2.calc_queue[j]]);

        ++j;
    }

    // Удалим одинаковые элементы из очереди.
    auto it1 = offspring.calc_queue.begin();

    while( it1 != offspring.calc_queue.end() )
    {
        auto it2 = it1 + 1;

        while( it2 != offspring.calc_queue.end() )
        {
            if( *it1 == *it2 )
            {
                it2 = offspring.calc_queue.erase(it2);
            }

            if( it2 != offspring.calc_queue.end() )
            {
                ++it2;
            }
            else
            {
                break;
            }
        }

        ++it1;
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
void population::calc_fitness(individual &p) noexcept
{
    reset_signals(p);

    std::vector<float> out;
    out.reserve(p.out_signal_size);

    float sum = 0.0f;
    float n = 0.0f;

    for( uint32_t i = 0; i < m_training_patterns.get_size(); ++i )
    {
        pattern& ptrn = m_training_patterns.get(i);

        set_input_pattern(ptrn, p);
        calc_signals(p);
        get_output_signal(p, out);

        const std::vector<float> &ptrn_out = ptrn.get_out();

        float cur_sum = 0.0f;
        float cur_n = 0.0f;

        for( uint32_t i = 0; i < out.size(); ++i )
        {
            cur_sum += (out[i] - ptrn_out[i]) * (out[i] - ptrn_out[i]);
            cur_n += 1.0f;
        }

        float cur_e = cur_sum / cur_n;

        sum += cur_e;
        n += 1.0f;
    }

    float e = sum / n;

    p.fitness = e;
}
//-----------------------------------------------------------------------------
void population::calc_all_fitness() noexcept
{
    for( individual &p : m_individuals )
    {
        calc_fitness(p);
    }
}
//-----------------------------------------------------------------------------
void population::do_mutations() noexcept
{
    std::uniform_real_distribution<float>  rate_dis(0.0f, 1.0f);

    std::uniform_real_distribution<float>  reset_weight_dis(m_params.reset_weight_down, m_params.reset_weight_up);
    std::uniform_real_distribution<float>  change_weight_dis(m_params.change_weight_down, m_params.change_weight_up);

    for( individual &p : m_individuals )
    {
        for( node &cur_node : p.nodes )
        {
            if( rate_dis(m_mt19937) < m_params.change_link_rate )
            {
                if( rate_dis(m_mt19937) < m_params.resete_link_rate )
                {
                    cur_node.bias = reset_weight_dis(m_mt19937);
                }
                else
                {
                    cur_node.bias += change_weight_dis(m_mt19937);
                }
            }
        }

        for( link &cur_link : p.links )
        {
            if( cur_link.enabled == true )
            {
                if( rate_dis(m_mt19937) < m_params.change_link_rate )
                {
                    if( rate_dis(m_mt19937) < m_params.resete_link_rate )
                    {
                        cur_link.w = reset_weight_dis(m_mt19937);
                    }
                    else
                    {
                        cur_link.w += change_weight_dis(m_mt19937);
                    }
                }
            }
        }

        if( rate_dis(m_mt19937) < m_params.add_node_rate )
        {
            std::uniform_int_distribution<int32_t> num_link_dis(0, p.links.size() - 1);

            add_node(p, num_link_dis(m_mt19937));
        }

        if( rate_dis(m_mt19937) < m_params.add_link_rate )
        {
            std::uniform_int_distribution<int32_t> num_node_dis(0, p.nodes.size() - 1);

            add_link(p, num_node_dis(m_mt19937), num_node_dis(m_mt19937));
        }
    }
}
//-----------------------------------------------------------------------------
void population::transfer_best_individuals_from_species() noexcept
{
    m_temporary_pool.clear();
    m_temporary_pool.reserve(m_individuals.size());

    // Перенесем лучшую особь каждого вида в следующее поколение.
    for( species &s : m_species )
    {
        m_temporary_pool.push_back( m_individuals[s.individuals[0]] );
    }
}
//-----------------------------------------------------------------------------
void population::cross_parents_in_species() noexcept
{
    float sum_species_adapted_fitness = 0.0f;

    for( species &s : m_species )
    {
        s.avg_fitness = 0.0f;

        for( uint32_t i = 0; i < s.individuals.size(); ++i )
        {
            s.avg_fitness += m_individuals[s.individuals[i]].fitness;
        }

        s.avg_fitness = s.avg_fitness / (float) s.individuals.size();
    }

    float max_fitness = std::max_element(m_species.begin(), m_species.end(), [](const species &s1, const species &s2) {
        return s1.avg_fitness < s2.avg_fitness;
    })->avg_fitness;

    std::vector<float> species_adapted_fitness(m_species.size(), 0.0f);

    for( uint32_t i = 0; i < species_adapted_fitness.size(); ++i )
    {
        species_adapted_fitness[i] = m_species[i].avg_fitness + max_fitness;
        sum_species_adapted_fitness += species_adapted_fitness[i];
    }

    for( uint32_t i = 0; i < species_adapted_fitness.size(); ++i )
    {
        m_species[i].number_of_children = (uint32_t ) std::round( (species_adapted_fitness[i] / sum_species_adapted_fitness) *
                                                                  (float) m_individuals.size() );
        m_species[i].number_of_children -= 1; // Один ребенок уже есть - это лучшая особь, которую мы перенесли без изменений.
    }

    for( species &s : m_species )
    {
        if( s.individuals.size() > 1 )
        {
            std::uniform_int_distribution<int32_t> num_parents_dis(0, s.individuals.size() - 1);

            for(uint32_t i = 0; i < s.number_of_children; ++i)
            {
                individual offspring;
                uint32_t p1ndx, p2ndx;

                while( true )
                {
                    p1ndx = num_parents_dis(m_mt19937);
                    p2ndx = num_parents_dis(m_mt19937);

                    if( p1ndx != p2ndx )
                    {
                        break;
                    }
                }

                cross_parents(m_individuals[s.individuals[p1ndx]], m_individuals[s.individuals[p2ndx]], offspring);
                m_temporary_pool.push_back(offspring);
            }
        }
        else
        {
            for(uint32_t i = 0; i < s.number_of_children; ++i)
            {
                m_temporary_pool.push_back(m_individuals[s.individuals[0]]);
            }
        }
    }
}
//-----------------------------------------------------------------------------
void population::split_into_species() noexcept
{
    m_species.clear();

    uint32_t ndvdl_ndx = 1;

    species new_species;

    new_species.individuals.push_back(0);
    new_species.avg_fitness = 0.0f;

    m_species.push_back(new_species);

    while( ndvdl_ndx < m_individuals.size() )
    {
        bool placed = false;

        for( species &s : m_species )
        {
            if( calc_distance_between_parents(m_individuals[s.individuals[0]],
                                              m_individuals[ndvdl_ndx],
                                              m_params.c1, m_params.c2, m_params.c3) <= m_params.max_distance_between_species )
            {
                s.individuals.push_back(ndvdl_ndx);

                placed = true;
                break;
            }
        }

        if( placed == false )
        {
            species new_species;

            new_species.individuals.push_back(ndvdl_ndx);
            new_species.avg_fitness = 0.0f;

            m_species.push_back(new_species);
        }

        ++ndvdl_ndx;
    }

    for( species &s : m_species )
    {
        std::sort(s.individuals.begin(), s.individuals.end(), [&](uint32_t a, uint32_t b) {
            return m_individuals[a].fitness < m_individuals[b].fitness;
        });
    }
}
//-----------------------------------------------------------------------------
void population::correct_max_distance_between_species() noexcept
{
    uint32_t species_num = get_species_num();

    if( species_num > m_params.species_num )
    {
        m_params.max_distance_between_species += m_params.distance_up_step;
        std::cout << "new distance: " << m_params.max_distance_between_species << std::endl;
    }
    else if( species_num < m_params.species_num )
    {
        m_params.max_distance_between_species -= m_params.distance_down_step;
        std::cout << "new distance: " << m_params.max_distance_between_species << std::endl;
    }
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
void population::calc_signals(individual& p) noexcept
{
    uint32_t neu_ndx = 0, lnk_ndx = 0;

    while( neu_ndx < p.calc_queue.size() )
    {
        node& cur_node = p.nodes[p.calc_queue[neu_ndx]];

        cur_node.sum = 0.0f;

        while( lnk_ndx < p.links_queue.size() && p.links[p.links_queue[lnk_ndx]].out == p.calc_queue[neu_ndx] )
        {
            link& lnk = p.links[p.links_queue[lnk_ndx]];

            if( lnk.enabled )
            {
                cur_node.sum += p.nodes[lnk.in].signal * lnk.w;
            }

            ++lnk_ndx;
        }

        if( cur_node.type == node_type::hidden_node ||
            cur_node.type == node_type::output_node
          )
        {
            switch( cur_node.activation_func )
            {
                case node_activation_func_type::linear_activation_func:
                    cur_node.signal = cur_node.sum;
                    break;
                case node_activation_func_type::hyperbolic_activation_func:
                    cur_node.signal = tanh(cur_node.sum);
                    break;
            }
        }

        ++neu_ndx;
    }
}
//-----------------------------------------------------------------------------
void population::get_output_signal(individual& p, std::vector<float>& sgnls) noexcept
{
    sgnls.clear();
    sgnls.reserve(m_out_signal_size);

    for( uint32_t i = m_in_signal_size; i < m_in_signal_size + m_out_signal_size; ++i )
    {
        sgnls.push_back(p.nodes[i].signal);
    }
}
//-----------------------------------------------------------------------------
bool population::add_node(individual& p, uint32_t link_num) noexcept
{
    link& lnk = p.links[link_num];

    uint32_t new_link_in_time_stamp = 0;
    uint32_t new_link_out_time_stamp = 0;

    uint32_t node_in_time_stamp = p.nodes[lnk.in].time_stamp;
    uint32_t node_out_time_stamp = p.nodes[lnk.out].time_stamp;

    uint32_t new_node_time_stamp = m_time_stamp_distributor.get_time_stamp_for_node(
                                       lnk.time_stamp, node_in_time_stamp, node_out_time_stamp,
                                       new_link_in_time_stamp, new_link_out_time_stamp);

    for( node& cur_node : p.nodes )
    {
        if( cur_node.time_stamp == new_node_time_stamp )
        {
            return false;
        }
    }

    lnk.enabled = false;

    node new_node;

    new_node.time_stamp = new_node_time_stamp;

    new_node.type = node_type::hidden_node;
    new_node.activation_func = node_activation_func_type::hyperbolic_activation_func;

    std::uniform_real_distribution<float>  weight_dis(m_params.reset_weight_down, m_params.reset_weight_up);

    new_node.bias = weight_dis(m_mt19937);
    new_node.sum = 0.0f;
    new_node.signal = 0.0f;

    link new_link_in, new_link_out;

    new_link_in.time_stamp = new_link_in_time_stamp;
    new_link_out.time_stamp = new_link_out_time_stamp;

    new_link_in.w = new_link_out.w = lnk.w;
    new_link_in.enabled = new_link_out.enabled = true;

    uint32_t ins_ndx = 0;

    while( new_node_time_stamp > p.nodes[ins_ndx].time_stamp && ins_ndx < p.nodes.size() )
    {
        ++ins_ndx;
    }

    for( link& cur_link : p.links )
    {
        if( p.nodes[cur_link.in].time_stamp > new_node_time_stamp )
        {
            ++cur_link.in;
        }

        if( p.nodes[cur_link.out].time_stamp > new_node_time_stamp )
        {
            ++cur_link.out;
        }
    }

    new_link_in.in = lnk.in;
    new_link_out.in = ins_ndx;

    new_link_in.out = ins_ndx;
    new_link_out.out = lnk.out;

    auto ins_link_it = p.links.begin();

    bool new_link_in_inserted = false;
    bool new_link_out_inserted = false;

    while( ins_link_it != p.links.end() )
    {
        if( !new_link_in_inserted && new_link_in.time_stamp < ins_link_it->time_stamp)
        {
            ins_link_it = p.links.insert(ins_link_it, new_link_in);
            new_link_in_inserted = true;
        }

        if( !new_link_out_inserted && new_link_out.time_stamp < ins_link_it->time_stamp)
        {
            ins_link_it = p.links.insert(ins_link_it, new_link_out);
            new_link_out_inserted = true;
        }

        ++ins_link_it;
    }

    if( new_link_in_inserted == false )
    {
        if( new_link_out_inserted == false )
        {
            if( new_link_in.time_stamp < new_link_out.time_stamp )
            {
                p.links.push_back(new_link_in);
                p.links.push_back(new_link_out);
            }
            else
            {
                p.links.push_back(new_link_out);
                p.links.push_back(new_link_in);
            }
        }
        else
        {
            p.links.push_back(new_link_in);
        }
    }
    else if( new_link_out_inserted == false )
    {
        p.links.push_back(new_link_out);
    }

    for( uint32_t i = 0; i < p.calc_queue.size(); ++i )
    {
        if( p.nodes[p.calc_queue[i]].time_stamp > new_node_time_stamp )
        {
            ++p.calc_queue[i];
        }
    }

    auto ins_it = std::find(p.calc_queue.begin(), p.calc_queue.end(), new_link_out.out);

    p.nodes.insert(p.nodes.begin() + ins_ndx, new_node);

    p.calc_queue.insert(ins_it, ins_ndx),

    rebuild_links_queue(p);

    return true;
}
//-----------------------------------------------------------------------------
bool population::add_link(individual& p, uint32_t neu_in, uint32_t neu_out) noexcept
{
    // Не передавать сигнал входным нейронам.
    if( neu_out < m_in_signal_size )
    {
        return false;
    }

    // Если параметрами алгоритма запрещены рекурсивные соединения, то не будем добавлять такие.
    if( m_params.enable_recursive_links == false )
    {
        if( new_link_is_recursive(p, neu_in, neu_out) == true )
        {
            return false;
        }
    }

    uint32_t neu_in_time_stamp = p.nodes[neu_in].time_stamp;
    uint32_t neu_out_time_stamp = p.nodes[neu_out].time_stamp;

    uint32_t new_link_time_stamp = m_time_stamp_distributor.get_time_stamp_for_link(neu_in_time_stamp, neu_out_time_stamp);

    for( link& cur_link : p.links )
    {
        if( cur_link.time_stamp == new_link_time_stamp )
        {
            return false;
        }
    }

    link new_link;

    new_link.time_stamp = new_link_time_stamp;

    new_link.enabled = true;

    new_link.in = neu_in;
    new_link.out = neu_out;

    std::uniform_real_distribution<float>  weight_dis(m_params.reset_weight_down, m_params.reset_weight_up);

    new_link.w = weight_dis(m_mt19937);

    bool new_link_inserted = false;

    auto ins_it = p.links.begin();

    while( ins_it != p.links.end() )
    {
        if( new_link_time_stamp < ins_it->time_stamp )
        {
            p.links.insert(ins_it, new_link);
            new_link_inserted = true;

            break;
        }

        ++ins_it;
    }

    if( new_link_inserted == false )
    {
        p.links.push_back(new_link);
    }

    rebuild_links_queue(p);

    return true;
}
//-----------------------------------------------------------------------------
bool population::new_link_is_recursive(const individual& p, uint32_t in_ndx, uint32_t out_ndx)
{
    if( in_ndx == out_ndx )
    {
        return true;
    }

    bool in_found = in_ndx < m_in_signal_size ? true : false;

    for( uint32_t i = 0; i < p.calc_queue.size(); ++i )
    {
        if( in_found == false && p.calc_queue[i] == in_ndx )
        {
            in_found = true;
        }
        else if( p.calc_queue[i] == out_ndx )
        {
            if( in_found == true )
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }

    return false;
}
//-----------------------------------------------------------------------------
