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
    , m_mt19937(m_random_device())
{
    reset(population_size);
    random_init();
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

    for( uint32_t i = 0; i < population_size; ++i )
    {
        individual new_individual;

        new_individual.in_signal_size = m_in_signal_size;
        new_individual.out_signal_size = m_out_signal_size;

        new_individual.nodes.reserve(m_in_signal_size + m_out_signal_size);
        new_individual.calc_queue.reserve(m_in_signal_size + m_out_signal_size);

        new_individual.links.reserve(m_in_signal_size * m_out_signal_size);

        new_individual.species = 0;

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

        rebuild_links_queue(new_individual);

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
void population::make_test() noexcept
{
    individual &ndvdl = m_individuals[0];

    rebuild_links_queue(ndvdl);

    std::vector<float> in(2, 0.13f);
    pattern ptrn(2, 1);
    ptrn.set_in(in);

    set_input_pattern(ptrn, ndvdl);

    calc_signals(ndvdl);

    std::cout << "add ok: " << add_node(ndvdl, 0) << std::endl;

    calc_signals(ndvdl);

    std::vector<float> v;

    get_output_signal(ndvdl, v);

    for( auto val : v ) std::cout << val << ' '; std::cout << std::endl;


    std::cout << "beg split" << std::endl;
    for( size_t i = 0; i < 1000; ++i )
    {
        split_into_species();
    }
    std::cout << "end split" << std::endl;

    std::cout << "species num: " << m_species.size() << std::endl;

    in.clear();

    patterns ptrns(2,1);

    std::vector<float> out;

    in.push_back(0.8f);
    in.push_back(0.8f);
    out.push_back(-0.8f);
    ptrn.set_in(in);
    ptrn.set_out(out);
    ptrns.push_back(ptrn);
    in.clear();
    out.clear();

    in.push_back(-0.8f);
    in.push_back(-0.8f);
    out.push_back(-0.8f);
    ptrn.set_in(in);
    ptrn.set_out(out);
    ptrns.push_back(ptrn);
    in.clear();
    out.clear();

    in.push_back(0.8f);
    in.push_back(-0.8f);
    out.push_back(0.8f);
    ptrn.set_in(in);
    ptrn.set_out(out);
    ptrns.push_back(ptrn);
    in.clear();
    out.clear();

    in.push_back(-0.8f);
    in.push_back(0.8f);
    out.push_back(0.8f);
    ptrn.set_in(in);
    ptrn.set_out(out);
    ptrns.push_back(ptrn);
    in.clear();
    out.clear();

    set_training_patterns(ptrns);

    float error = calc_averaged_square_error(m_individuals[0]);

    std::cout << "error: " << error << std::endl;
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
float population::calc_averaged_square_error(individual &p) noexcept
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

    return e;
}
//-----------------------------------------------------------------------------
void population::random_init() noexcept
{
    std::uniform_real_distribution<float> float_dis(-1.0f, 1.0f);

    for( individual& p : m_individuals )
    {
        for( node& pn : p.nodes )
        {
            pn.bias = float_dis(m_mt19937);
        }

        for( link& pl : p.links )
        {
            pl.w = float_dis(m_mt19937);
        }
    }
}
//-----------------------------------------------------------------------------
void population::split_into_species() noexcept
{
    for( individual& p : m_individuals )
    {
        p.species = 0;
    }

    m_species.clear();

    uint32_t frst_ndvdl_ndx = 0, scnd_ndvdl_ndx = 0, free_species = 1;

    m_individuals[0].species = free_species++; // Поместим первую особь сразу в первый вид.

    species new_species;

    new_species.individuals.push_back(0);
    new_species.avg_fitness = 0.0f;

    m_species.push_back(new_species);

    while( frst_ndvdl_ndx < m_individuals.size() )
    {
        individual &frst_ndvdl = m_individuals[frst_ndvdl_ndx];

        if( m_individuals[frst_ndvdl_ndx].species == 0 )
        {
            scnd_ndvdl_ndx = 0;

            while( scnd_ndvdl_ndx < m_individuals.size() )
            {
                if( frst_ndvdl_ndx == scnd_ndvdl_ndx )
                {
                    ++scnd_ndvdl_ndx;
                    continue;
                }

                individual &scnd_ndvdl = m_individuals[scnd_ndvdl_ndx];

                if( calc_distance_between_parents( frst_ndvdl, scnd_ndvdl, m_c1, m_c2, m_c3) <= m_max_distance_between_species )
                {
                    if( scnd_ndvdl.species == 0 )
                    {
                        scnd_ndvdl.species = free_species++;

                        species new_species;

                        new_species.avg_fitness = 0.0f;
                        new_species.individuals.push_back(scnd_ndvdl_ndx);

                        m_species.push_back(new_species);
                    }

                    frst_ndvdl.species = scnd_ndvdl.species;

                    m_species[frst_ndvdl.species - 1].individuals.push_back(frst_ndvdl_ndx);

                    break;
                }

                ++scnd_ndvdl_ndx;
            }
        }

        ++frst_ndvdl_ndx;
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

    uint32_t new_link_in_time_stamp;
    uint32_t new_link_out_time_stamp;

    uint32_t new_node_time_stamp =
        m_time_stamp_distributor.get_time_stamp_for_node(lnk.time_stamp, new_link_in_time_stamp, new_link_out_time_stamp);

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

    new_node.bias = 0.0f;
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

    auto ins_it = std::find(p.calc_queue.begin(), p.calc_queue.end(), ins_ndx);

    for( uint32_t i = 0; i < p.calc_queue.size(); ++i )
    {
        if( p.nodes[p.calc_queue[i]].time_stamp > new_node_time_stamp )
        {
            ++p.calc_queue[i];
        }
    }

    p.nodes.insert(p.nodes.begin() + ins_ndx, new_node);

    p.calc_queue.insert(ins_it, ins_ndx),

    rebuild_links_queue(p);

    return true;
}
//-----------------------------------------------------------------------------
bool population::add_link(individual& p, uint32_t neu_in, uint32_t neu_out) noexcept
{
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

    new_link.w = 0.0f;

    bool new_link_inserted = false;

    auto ins_it = p.links.begin();

    while( ins_it != p.links.end() )
    {
        if( new_link_time_stamp < ins_it->time_stamp )
        {
            p.links.insert(ins_it, new_link);

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
