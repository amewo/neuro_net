#ifndef neuron_factory_h
#define neuron_factory_h

#include <stdint.h>

class neuron;

//-----------------------------------------------------------------------------
enum class neuron_type : uint32_t
{
    input_neuron_type,

    linear_neuron_type,
    hyperbolic_neuron_type
};
//-----------------------------------------------------------------------------
class neuron_factory
{
public:
    neuron_factory() noexcept;
    neuron_factory(const neuron_factory&) = delete;

    virtual ~neuron_factory() noexcept;

    neuron_factory& operator=(const neuron_factory&) = delete;

    static neuron_factory& instance() noexcept;

    virtual neuron* create_neuron(neuron_type type, uint32_t id) noexcept;
};
//-----------------------------------------------------------------------------

#endif // #define neuron_factory_h
