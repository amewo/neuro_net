#include <iostream>
#include <iomanip>
#include <ctime>

#include <stdint.h>

#include <neuro_net.h>
#include <neuron_factory.h>
#include <neuro_net_genetic_training.h>

#include <boost/lexical_cast.hpp>

#include <bars.h>

using namespace std;

int main()
{
    /*
    neuro_net nnet;

    neuron_factory& neu_factory = neuron_factory::instance();

    for( uint32_t i = 6; i >= 1; --i )
    {
        if( i <= 2 )
        {
            nnet.add_neuron( neu_factory.create_neuron(neuron_type::input_neuron_type, i) );
        }
        else
        {
            nnet.add_neuron( neu_factory.create_neuron(neuron_type::hyperbolic_neuron_type, i) );
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

    nnet.link_neurons(ids_from1, ids_to1, 0.38f);
    nnet.link_neurons(ids_to1, ids_to2, 1.73f);

    vector<float> in_signals(2, 0.01f);
    nnet.set_input_signal(in_signals);

    nnet.calc_signal();

    for( auto val : nnet.get_output_signals() ) cout << val << endl;

    cin.get();

    cout << "save neuron_net state..." << endl;

    neuro_net_state state;
    nnet.save_state(&state);

    cout << "state size: " << state.ByteSize() << endl;

    cout << "load neuron_net state..." << endl << endl;

    neuro_net nnet2;
    nnet2.restore_state(&state);

    nnet2.calc_signal();

    for( auto val : nnet2.get_output_signals() ) cout << val << endl;

    cin.get();
    */

    /*
    bars b;
    b.load_from_csv("/home/amewo/Documents/neural networks/test.csv");

    cout << "bars count: " << b.get_size() << endl;

    cin.get();
    */

    population pop(320, 2, 1);

    pop.make_test();

    return 0;
}
