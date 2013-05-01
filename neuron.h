#ifndef neuron_h
#define neuron_h

#include <stdint.h>
#include <vector>

class neuron
{
public:
    neuron(uint32_t id) noexcept;
    neuron(const neuron& neu) noexcept;

    virtual ~neuron() noexcept;

    neuron& operator=(const neuron& neu) noexcept;

    float bias() const noexcept;
    void  set_bias(float bias) noexcept;

    float sum() const noexcept;

    float signal() const noexcept;
    void  set_signal(float signal) noexcept;

    uint32_t id() const noexcept;

    bool  add_link(neuron *neu, float w) noexcept;
    bool  del_link(neuron *neu) noexcept;

    virtual float calc_sum() noexcept;
    virtual float calc_signal() noexcept;

    virtual float activation_function(float sum) const noexcept;

protected:
    float m_bias;   // Смещение нейрона.
    float m_sum;    // Взвешенная сумма входных сигналов нейрона.
    float m_signal; // Рассчитанный сигнал нейрона.

    uint32_t m_id;  // Идентификатор нейрона.

    struct neuron_in_link
    {
        neuron_in_link(neuron *_neu, float _w) : neu(_neu), w(_w) { }

        neuron *neu;
        float   w;
    };

    std::vector<neuron_in_link> m_in_links;
};

#endif // #define neuron_h
