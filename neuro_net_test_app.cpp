#include <iostream>
#include <iomanip>
#include <ctime>
#include <stdint.h>
#include <random>

#include <neuro_net.h>
#include <neuron_factory.h>
#include <neuro_net_genetic_training.h>
#include <bars.h>

#include <boost/lexical_cast.hpp>

#include <QApplication>
#include <qcp.h>

using namespace std;

int main(int argc, char *argv[])
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

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 9);
    std::uniform_real_distribution<float> dis2(0, 1);

    QApplication a(argc, argv);

    QCustomPlot *customPlot = new QCustomPlot();

    // For simplicity we'll just setup all data and plotting options here
    // add two new graphs and set their look:
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
    // generate some points of data (y0 for first, y1 for second graph):
    QVector<double> x(250), y0(250), y1(250);
    for (int i=0; i<250; ++i)
    {
      x[i] = i;
      y0[i] = exp(-i/150.0)*cos(i/10.0); // exponentially decaying cosine
      y1[i] = exp(-i/150.0);             // exponential envelope
    }
    // configure right and top axis to show ticks but no labels (could've also just called customPlot->setupFullAxesBox):
    customPlot->xAxis2->setVisible(true);
    customPlot->xAxis2->setTickLabels(false);
    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    QObject::connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    QObject::connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graphs:
    customPlot->graph(0)->setData(x, y0);
    customPlot->graph(1)->setData(x, y1);
    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    customPlot->graph(0)->rescaleAxes();
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    customPlot->graph(1)->rescaleAxes(true);
    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // make range moveable by mouse interaction (click and drag):
    customPlot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    customPlot->setInteraction(QCustomPlot::iSelectPlottables); // allow selection of graphs via mouse click
    customPlot->replot();

    customPlot->show();

    return a.exec();
}
