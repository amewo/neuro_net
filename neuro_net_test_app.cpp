#include <iostream>
#include <stdint.h>

#include <neuro_net.h>
#include <neuron_factory.h>

using namespace std;

int main()
{
    neuro_net nnet;

    neuron_factory& neu_factory = neuron_factory::instance();

    for( uint32_t i = 1; i <= 6; ++i )
    {
        if( i <= 2 )
        {
            nnet.add_neuron( neu_factory.create_neuron(input_neuron_type, i) );
        }
        else
        {
            nnet.add_neuron( neu_factory.create_neuron(linear_neuron_type, i) );
        }
    }

    vector<uint32_t> ids_from1;
    vector<uint32_t> ids_to1;
    vector<uint32_t> ids_to2;

    ids_from1.push_back(1);
    ids_from1.push_back(2);

    ids_to1.push_back(3);
    ids_to1.push_back(4);

    ids_to2.push_back(5);
    ids_to2.push_back(6);

    nnet.mark_neuron_as_input(1);
    nnet.mark_neuron_as_input(2);

    nnet.mark_neuron_as_output(5);
    nnet.mark_neuron_as_output(6);

    nnet.link_neurons(ids_from1, ids_to1, 1.0f);
    nnet.link_neurons(ids_to1, ids_to2, 1.0f);

    nnet.get_neuron(1)->set_signal(1.0f);
    nnet.get_neuron(2)->set_signal(1.0f);

    nnet.calc_signal();

    cout << nnet.get_neuron(5)->signal() << endl;
    cout << nnet.get_neuron(6)->signal() << endl;

    cin.get();

    return 0;
}
