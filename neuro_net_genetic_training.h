#ifndef neuro_net_genetic_training_h
#define neuro_net_genetic_training_h

#include <stdint.h>
#include <vector>
#include <stdexcept>
#include <map>

#include "patterns.h"

//-----------------------------------------------------------------------------
enum class node_activation_func_type : uint8_t
{
    linear_activation_func,
    hyperbolic_activation_func
};
//-----------------------------------------------------------------------------
enum class node_type : uint8_t
{
    input_node,
    hidden_node,
    output_node
};
//-----------------------------------------------------------------------------
struct node
{
    node_activation_func_type activation_func;
    node_type type;

    uint32_t time_stamp;

    float bias;
    float sum;
    float signal;
};
//-----------------------------------------------------------------------------
struct link
{
    uint32_t time_stamp;

    uint32_t in;
    uint32_t out;

    float w;

    bool enabled;
};
//-----------------------------------------------------------------------------
struct individual
{
    std::vector<node> nodes;
    std::vector<link> links;

    // Очередь, в соответствии с которой следует вычислять сигналы узлов.
    std::vector<uint32_t> calc_queue;
    std::vector<uint32_t> links_queue;

    uint32_t in_signal_size;
    uint32_t out_signal_size;
};
//-----------------------------------------------------------------------------
class time_stamp_distributor
{
public:
    time_stamp_distributor(uint32_t begin_free_node_time_stamp, uint32_t begin_link_node_time_stamp) noexcept;

    void reset() noexcept;

    uint32_t get_time_stamp_for_node(uint32_t link_time_stamp, uint32_t& link_in_time_stamp, uint32_t& link_out_time_stamp);
    uint32_t get_time_stamp_for_link(uint32_t node_in_time_stamp, uint32_t node_out_time_stamp);

protected:
    struct node_time_stamp_struct
    {
        uint32_t time_stamp;

        uint32_t link_in_time_stamp;
        uint32_t link_out_time_stamp;
    };

    std::map<uint32_t, node_time_stamp_struct> m_node_time_stampes;
    std::map<uint32_t, std::map<uint32_t, uint32_t>> m_link_time_stampes;

    uint32_t m_begin_free_node_time_stamp;
    uint32_t m_begin_free_link_time_stamp;

    uint32_t m_free_node_time_stamp;
    uint32_t m_free_link_time_stamp;
};
//-----------------------------------------------------------------------------
class population
{
public:
    population(uint32_t population_size, uint32_t in_signal_size, uint32_t out_signal_size) throw(std::runtime_error);

    void reset(uint32_t population_size) noexcept;

    bool set_training_patterns(patterns &ptrns) noexcept;

    void make_test() noexcept;

protected:
    void  cross_parents(const individual& p1, const individual& p2, individual &offspring) noexcept;
    float calc_distance_between_parents(const individual& p1, const individual& p2, float c1, float c2, float c3) noexcept;
    float calc_averaged_square_error(individual& p) noexcept;

    void  rebuild_links_queue(individual& p) noexcept;

    void  set_input_pattern(const pattern& ptrn, individual& p) noexcept;
    void  reset_signals(individual& p) noexcept;
    void  calc_signals(individual& p) noexcept;
    void  get_output_signal(individual& p, std::vector<float>& sgnls) noexcept;

    bool  add_node(individual& p, uint32_t link_num) noexcept;
    bool  add_link(individual& p, uint32_t neu_in, uint32_t neu_out) noexcept;

    uint32_t m_in_signal_size;
    uint32_t m_out_signal_size;

    patterns                m_training_patterns;
    time_stamp_distributor  m_time_stamp_distributor;

    std::vector<individual> m_individuals;
};
//-----------------------------------------------------------------------------

#endif // #define neuro_net_genetic_training_h
