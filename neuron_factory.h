#ifndef neuron_factory_h
#define neuron_factory_h

#include <stdint.h>

#include "neuron.h"

enum neuron_type : uint32_t
{
    input_neuron,

    linear_neuron,
    hyperbolic_neuron
};

class neuron_factory
{
public:
    neuron_factory() noexcept;
    neuron_factory(const neuron_factory&) = delete;

    virtual ~neuron_factory() noexcept;

    neuron_factory& operator=(const neuron_factory&) = delete;

    static neuron_factory& instance() noexcept;

    virtual neuron* create_neuron(neuron_type type) noexcept;
};

#endif // #define neuron_factory_h
