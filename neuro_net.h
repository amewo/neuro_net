#ifndef neuro_net_h
#define neuro_net_h

#include <vector>
#include <stdexcept>

#include "neuron.h"
#include "patterns.h"

//-----------------------------------------------------------------------------
// Класс нейронной сети.
//
// Все нейроны сети хранятся в векторе. Экземпляры класса нейрона хранятся в упорядоченном
// состоянии по идентификатору в порядке возврастания значения идентификатора.
// Вычисление сигнала нейронной сети выполняется также в соответствии с упорядоченнойстью
// вектора нейронов, т.е. сначала будет вычислен сигнал нейрона с самым малым значением
// идентификатора, а самым последний нейрон, сигнал которого будет вычислен, будет иметь
// самый большой идентификатор.
//
// Экземпляр класса нейронной сети не может хранить экземпляры нейронов с
// одинаковыми идентификаторами.
class neuro_net
{
public:
    neuro_net() noexcept;
    neuro_net(const neuro_net& nnet) = delete;

    virtual ~neuro_net() noexcept;

    neuro_net& operator=(const neuro_net& nnet) = delete;

    const std::string& get_name() const noexcept;
    void set_name(const std::string& name) noexcept;

    // Право владения экземпляром класса нейрона передается экземпляру класса нейронной сети.
    // Добавленный экземпляр класса нейрона будет удален при вызове деструктора нейронной сети.
    bool add_neuron(neuron *neu) noexcept;

    // Удаляет нейрон из вектора нейронов нейронной сети. Также будет удален и сам
    // экземпляр классай нейрона.
    bool del_neuron(uint32_t id) noexcept;

    neuron* get_neuron(uint32_t id) noexcept;

    // Размер вектора входного сигнала.
    uint32_t get_in_size() const noexcept;

    // Размер вектора выходного сигнала.
    uint32_t get_out_size() const noexcept;

    bool mark_neuron_as_input(uint32_t id) noexcept;
    bool mark_neuron_as_output(uint32_t id) noexcept;

    bool unmark_neuron_as_input(uint32_t id) noexcept;
    bool unmark_neuron_as_output(uint32_t id) noexcept;

    void mark_neurons_as_input(const std::vector<uint32_t>& ids) throw(std::runtime_error);
    void mark_neurons_as_output(const std::vector<uint32_t>& ids) throw(std::runtime_error);

    bool neuron_is_input(uint32_t id) const noexcept;
    bool neuron_is_output(uint32_t id) const noexcept;

    const std::vector<neuron*>& get_input_neurons() const noexcept;
    const std::vector<neuron*>& get_output_neurons() const noexcept;

    std::vector<uint32_t> get_ids_of_input_neurons() const noexcept;
    std::vector<uint32_t> get_ids_of_output_neurons() const noexcept;

    bool link_neurons(uint32_t id_from, uint32_t id_to, float w) noexcept;

    void link_neurons(const std::vector<uint32_t>& ids_from,
                      const std::vector<uint32_t>& ids_to,
                      float w
                      ) throw (std::runtime_error);

    void link_neurons(const std::vector<uint32_t>& ids_from,
                      const std::vector<uint32_t>& ids_to,
                      const std::vector<std::vector<float>>& ws
                      ) throw (std::runtime_error);

    bool unlink_neurons(uint32_t id_from, uint32_t id_to) noexcept;

    void unlink_neurons(const std::vector<uint32_t>& ids_from,
                      const std::vector<uint32_t>& ids_to
                      ) throw (std::runtime_error);

    void set_input_signal(const std::vector<float> sgnls) throw (std::runtime_error);
    const std::vector<float> get_output_signals() const noexcept;

    void calc_signal() noexcept;

    void save_state(neuro_net_state *state) const noexcept;
    void restore_state(const neuro_net_state *state) throw (std::runtime_error);

protected:
    void restore_input_neurons_state(const neuro_net_state *state) throw (std::runtime_error);
    void restore_output_neurons_state(const neuro_net_state *state) throw (std::runtime_error);

    std::string m_name;

    std::vector<neuron*> m_neurons;

    std::vector<neuron*> m_in_neurons;
    std::vector<neuron*> m_out_neurons;
};
//-----------------------------------------------------------------------------

#endif // #ifndef neuro_net_h
