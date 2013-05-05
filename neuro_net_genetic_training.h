#ifndef neuro_net_genetic_training_h
#define neuro_net_genetic_training_h

#include <stdint.h>
#include <vector>

enum class node_activation_func_type : uint8_t
{
    linear_activation_func,
    hyperbolic_activation_func
};

enum class node_type : uint8_t
{
    input_node,
    hidden_node,
    output_node
};

struct node
{
    node_activation_func_type activation_func;
    node_type type;

    uint32_t time_stamp;

    float bias;
    float sum;
    float signal;
};

struct link
{
    uint32_t time_stamp;

    uint32_t in;
    uint32_t out;

    float w;

    bool enabled;
};

struct individual
{
    std::vector<node> nodes;
    std::vector<link> links;
};

class time_stamp_distributor
{
public:
    time_stamp_distributor() noexcept;
    virtual ~time_stamp_distributor() noexcept;

    void reset_full() noexcept;
    void reset_to_new_iteration() noexcept;

    uint32_t get_time_stamp_for_node(uint32_t link_time_stamp, uint32_t& link_from_time_stamp, uint32_t& link_to_time_stamp);
    uint32_t get_time_stamp_for_link(uint32_t node_from_time_stamp, uint32_t node_to_time_stamp);
};

#endif // #define neuro_net_genetic_training_h
