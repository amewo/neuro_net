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

    // Если была добавлена новая связь или нейрон, то следует построить очередь расчета заново.
    bool changed;
};
//-----------------------------------------------------------------------------
class time_stamp_distributor
{
public:
    time_stamp_distributor() noexcept;

    void reset() noexcept;

    uint32_t get_time_stamp_for_node(uint32_t link_time_stamp, uint32_t& link_in_time_stamp, uint32_t& link_out_time_stamp);
    uint32_t get_time_stamp_for_link(uint32_t node_in_time_stamp, uint32_t node_out_time_stamp);

protected:
    struct node_
    {

    };

    std::map<uint32_t, uint32_t> m_node_time_stampes;
    std::map<uint32_t, std::map<uint32_t, uint32_t>> m_link_time_stampes;

    uint32_t m_free_link_time_stamp;
    uint32_t m_free_node_time_stamp;
};
//-----------------------------------------------------------------------------
class population
{
public:
    population(uint32_t size, uint32_t in_signal_size, uint32_t out_signal_size) throw(std::runtime_error);

    bool set_training_patterns(patterns &ptrns) noexcept;

protected:
    patterns                m_training_patterns;
    time_stamp_distributor  m_time_stamp_distributor;

    std::vector<individual> m_individuals;
};
//-----------------------------------------------------------------------------

#endif // #define neuro_net_genetic_training_h
