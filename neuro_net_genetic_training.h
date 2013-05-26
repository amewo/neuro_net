#ifndef neuro_net_genetic_training_h
#define neuro_net_genetic_training_h

#include <stdint.h>
#include <vector>
#include <stdexcept>
#include <map>
#include <random>

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

    float fitness;
};
//-----------------------------------------------------------------------------
struct species
{
    std::vector<uint32_t> individuals;

    uint32_t number_of_children; // Количество детей, которое будет взято от этого вида.
                                 // Здесь не учитывается одна лучшая особь, которая переносится без изменений.
    float avg_fitness; // Средняя приспособленность особей, принадлежащих данному виду.
                       // При выполнении кроссовера данное значение будет перерасчитано и большее значение будет
                       // соответствовать большей приспособленности, тогда как у отдельного индивидуума
                       // меньшее значение соответствует меньшей ошибке индивида на обучающей выборке.
};
//-----------------------------------------------------------------------------
class time_stamp_distributor
{
public:
    time_stamp_distributor(uint32_t begin_free_node_time_stamp, uint32_t begin_link_node_time_stamp) noexcept;

    void reset() noexcept;

    uint32_t get_time_stamp_for_node(uint32_t link_time_stamp, uint32_t node_in_time_stamp, uint32_t node_out_time_stamp,
                                     uint32_t& link_in_time_stamp, uint32_t& link_out_time_stamp);
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
struct population_params
{
    float add_node_rate      = 0.001f;
    float add_link_rate      = 0.05f;

    float change_link_rate   = 0.30f;  // Вероятность изменения веса.
    float resete_link_rate   = 0.01f;  // Вероятность сбросить значение веса на новое из диапазона [-1;1),
                                       // в противном случае его значение изменится на число из диапазона [-1.0;1.0).

    float enable_link_rate   = 0.50f;

    float max_distance_between_species = 1.0f; // Максимальное расстояние между особями одного вида.

    float c1 = 1.0f;
    float c2 = 1.0f;
    float c3 = 1.0f;

    float reset_weight_down     = -1.0f;
    float reset_weight_up       = 1.0f;

    float change_weight_down    = -1.0f;
    float change_weight_up      = 1.0f;

    uint32_t species_num        = 20;
    float    distance_up_step   = 0.1f;
    float    distance_down_step = 0.1f;

    bool     enable_recursive_links = false;
};
//-----------------------------------------------------------------------------
class population
{
public:
    population(uint32_t population_size, uint32_t in_signal_size, uint32_t out_signal_size) throw(std::runtime_error);

    void     set_params(const population_params &params) noexcept;
    const population_params& get_params() const noexcept;

    void     reset(uint32_t population_size) throw(std::runtime_error);
    void     reset_individual(individual &p) throw(std::runtime_error);

    bool     set_training_patterns(patterns &ptrns) noexcept;

    uint32_t get_current_epoch() const noexcept;
    uint32_t get_species_num() const noexcept;
    float    get_least_error_val() const noexcept;
    uint32_t get_nodes_num_in_best() const noexcept;

    void     next_epoch() noexcept;

    void     make_test() noexcept;

    bool     check_links(const individual &p) noexcept;

protected:
    void     cross_parents(const individual& p1, const individual& p2, individual &offspring) noexcept;

    float    calc_distance_between_parents(const individual& p1, const individual& p2, float c1, float c2, float c3) noexcept;
    void     calc_fitness(individual& p) noexcept;
    void     calc_all_fitness() noexcept;

    void     do_mutations() noexcept;

    void     transfer_best_individuals_from_species() noexcept;
    void     cross_parents_in_species() noexcept;

    void     split_into_species() noexcept;
    void     correct_max_distance_between_species() noexcept;

    void     rebuild_links_queue(individual& p) noexcept;

    void     set_input_pattern(const pattern& ptrn, individual& p) noexcept;
    void     reset_signals(individual& p) noexcept;
    void     calc_signals(individual& p) noexcept;
    void     get_output_signal(individual& p, std::vector<float>& sgnls) noexcept;

    bool     add_node(individual& p, uint32_t link_num) noexcept;
    bool     add_link(individual& p, uint32_t neu_in, uint32_t neu_out) noexcept;
    bool     new_link_is_recursive(const individual& p, uint32_t in_ndx, uint32_t out_ndx);

    uint32_t m_in_signal_size;
    uint32_t m_out_signal_size;

    patterns                m_training_patterns;
    time_stamp_distributor  m_time_stamp_distributor;

    std::vector<individual> m_individuals;
    std::vector<individual> m_temporary_pool;

    std::vector<species>    m_species;

    std::random_device m_random_device;
    std::mt19937 m_mt19937;
//    std::uniform_int_distribution<int32_t> dis(-1, 1);
//    std::uniform_real_distribution<double> dis2(-0.3, 0.3);

    uint32_t m_current_epoch; // Текущая эпоха алгоритма.

    population_params m_params;
};
//-----------------------------------------------------------------------------

#endif // #define neuro_net_genetic_training_h
