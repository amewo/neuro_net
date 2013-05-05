#ifndef neuron_h
#define neuron_h

#include <stdint.h>
#include <vector>
#include <stdexcept>

#include "neuron_factory.h"
#include "states.pb.h"

//-----------------------------------------------------------------------------
class neuron
{
public:
    neuron(uint32_t id) noexcept;
    neuron(const neuron& neu) = delete;

    virtual ~neuron() noexcept;

    neuron& operator=(const neuron& neu) = delete;

    float bias() const noexcept;
    void  set_bias(float bias) noexcept;

    float sum() const noexcept;

    float signal() const noexcept;
    void  set_signal(float signal) noexcept;

    uint32_t id() const noexcept;

    bool add_link(neuron *neu, float w) noexcept;
    bool del_link(neuron *neu) noexcept;

    virtual float calc_sum() noexcept;
    virtual float calc_signal() noexcept;

    virtual float activation_function(float sum) const noexcept;

    virtual void save_state(neuron_state *state) noexcept;

    // Восстанавливает только значение смещения, суммы и сигнала нейрона.
    // Возвращает вектор идентификаторов нейронов, которые необходимы для восстановления связей.
    virtual std::vector<uint32_t> restore_state(const neuron_state *state) noexcept;

    // Восстанавливает связи нейрона. Принимает состояние нейрона, которое было передано в
    // метод restore_state и вектор указателей на нейроны, в котором элементы соответстуют элемента
    // вектора, возвращенного методом restore_state.
    virtual void restore_links_state(const neuron_state *state, const std::vector<neuron*> neus) throw(std::runtime_error);

protected:
    neuron(uint32_t id, neuron_type type) noexcept;

    neuron_type m_type;

    float m_bias;   // Смещение нейрона.
    float m_sum;    // Взвешенная сумма входных сигналов нейрона.
    float m_signal; // Рассчитанный сигнал нейрона.

    uint32_t m_id;  // Идентификатор нейрона.

    struct neuron_link
    {
        neuron_link(neuron *_neu, float _w) : neu(_neu), w(_w) { }

        neuron *neu;
        float   w;
    };

    std::vector<neuron_link> m_links;
};
//-----------------------------------------------------------------------------
class input_neuron : public neuron
{
public:
    input_neuron(uint32_t id) noexcept;
    input_neuron(const input_neuron& neu) = delete;

    input_neuron& operator=(const input_neuron& neu) = delete;

    virtual float calc_sum() noexcept override;
    virtual float calc_signal() noexcept override;
};
//-----------------------------------------------------------------------------
class hyperbolic_neuron : public neuron
{
public:
    hyperbolic_neuron(uint32_t id) noexcept;
    hyperbolic_neuron(const hyperbolic_neuron& neu) = delete;

    hyperbolic_neuron& operator=(const hyperbolic_neuron& neu) = delete;

    virtual float activation_function(float sum) const noexcept override;
};
//-----------------------------------------------------------------------------

#endif // #define neuron_h
