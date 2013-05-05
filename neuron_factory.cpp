#include "neuron_factory.h"
#include "neuron.h"

neuron_factory::neuron_factory() noexcept
{
}

neuron_factory::~neuron_factory() noexcept
{
}

neuron_factory& neuron_factory::instance() noexcept
{
    static neuron_factory _neuron_factory;

    return _neuron_factory;
}

neuron* neuron_factory::create_neuron(neuron_type type, uint32_t id) noexcept
{
    switch(type)
    {
        case neuron_type::input_neuron_type:
            return new input_neuron(id);

        case neuron_type::linear_neuron_type:
            return new neuron(id);

        case neuron_type::hyperbolic_neuron_type:
            return new hyperbolic_neuron(id);

        default:
            return nullptr;
    };
}
