#include <iostream>
#include <stdint.h>

#include <neuro_net.h>
#include <neuron_factory.h>

using namespace std;

int main()
{
    neuro_net nnet;

    neuron_factory& neu_factory = neuron_factory::instance();

    for( uint32_t i = 1; i <= 100; ++i )
    {
        nnet.add_neuron( neu_factory.create_neuron(linear_neuron, i) );
    }

    cout << "Test end!" << endl;

    cin.get();

    return 0;
}
