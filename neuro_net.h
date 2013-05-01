#ifndef neuro_net_h
#define neuro_net_h

#include <vector>

#include "neuron.h"

class neuro_net
{
public:
    neuro_net() noexcept;
    neuro_net(const neuro_net& nnet) noexcept;

    virtual ~neuro_net() noexcept;

    neuro_net& operator=(const neuro_net& nnet) noexcept;

protected:
    std::vector<neuron*> m_neurons;
};

#endif // #ifndef neuro_net_h
