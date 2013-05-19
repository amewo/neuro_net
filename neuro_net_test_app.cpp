#include <iostream>
#include <iomanip>
#include <ctime>
#include <stdint.h>
#include <random>

#include <neuro_net.h>
#include <neuron_factory.h>
#include <neuro_net_genetic_training.h>
#include <bars.h>

#include <boost/math/special_functions.hpp>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <boost/lexical_cast.hpp>

#include <QApplication>
#include <qcp.h>

boost::numeric::ublas::vector<float> decompose(boost::numeric::ublas::vector<float> sig)
{
    if( sig.size() % 2 == 1 )
    {
        throw 1;
    }

    boost::numeric::ublas::matrix<float> p(sig.size(), sig.size(), 0.0f);

    for(int i = 0; i < sig.size() / 2; ++i)
    {
        p(i*2,i*2) = 1.0f / sqrt(2.0f);
        p(i*2,i*2+1) = 1.0f / sqrt(2.0f);
        p(i*2+1,i*2) = - 1.0f / sqrt(2.0f);
        p(i*2+1,i*2+1) = 1.0f / sqrt(2.0f);
    }

    boost::numeric::ublas::vector<float> res = boost::numeric::ublas::prod(sig, p);

    return res;
}

boost::numeric::ublas::vector<float> compose(boost::numeric::ublas::vector<float> coefs)
{
    if( coefs.size() % 2 == 1 )
    {
        throw 2;
    }

    boost::numeric::ublas::matrix<float> p(coefs.size(), coefs.size(), 0.0f);

    for(int i = 0; i < coefs.size() / 2; ++i)
    {
        p(i*2,i*2) = 1.0f / sqrt(2.0f);
        p(i*2,i*2+1) = 1.0f / sqrt(2.0f);
        p(i*2+1,i*2) = - 1.0f / sqrt(2.0f);
        p(i*2+1,i*2+1) = 1.0f / sqrt(2.0f);
    }

    p = trans(p);

    boost::numeric::ublas::vector<float> res = boost::numeric::ublas::prod(coefs, p);

    return res;
}

boost::numeric::ublas::vector<float> filter(int lvl, boost::numeric::ublas::vector<float> sig)
{
    if(lvl == 0)
    {
        return sig;
    }

    boost::numeric::ublas::vector<float> dec = decompose(sig);
    boost::numeric::ublas::vector<float> sig2(dec.size()/2);

    for( size_t i = 0; i < dec.size() / 2; ++i )
    {
        sig2(i) = dec(i*2+1);

        if( dec(i*2) <= 0.0002f )
        {
            dec(i*2) = 0.0f;
        }
    }

    sig2 = filter(lvl - 1, sig2);

    for( size_t i = 0; i < dec.size() / 2; ++i )
    {
        dec(i*2+1) = sig2(i);
    }

    return compose(dec);
}

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

    population p(1024, 2, 1);

    return 0;

    /*
    bars b;
    b.load_from_csv("/home/amewo/Documents/neural networks/test.csv");

    boost::numeric::ublas::vector<float> sig1(b.get_size() / 20), sig2(b.get_size() / 20);

    for( size_t i = 0; i < b.get_size() / 20; ++i )
    {
        sig1(i) = (b[i].high);
        sig2(i) = (b[i].high);
    }

    sig1 = filter(3, sig1);

    QVector<double> x, y1, y2;

    for( size_t i = 0; i < b.get_size() / 20; ++i )
    {
        x.push_back(i);
        y1.push_back(sig1(i));
        y2.push_back(sig2(i));
    }

    QApplication a(argc, argv);

    QCustomPlot *customPlot = new QCustomPlot();

    customPlot->addGraph();
    customPlot->addGraph();

    customPlot->graph(0)->setPen(QPen(Qt::black)); // line color blue for first graph
    customPlot->graph(1)->setPen(QPen(Qt::red)); // line color blue for first graph

    customPlot->xAxis->setLabel("bar num");
    customPlot->yAxis->setLabel("price");

    customPlot->xAxis2->setVisible(true);
    customPlot->xAxis2->setTickLabels(true);
    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setTickLabels(true);

    QObject::connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    QObject::connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    customPlot->graph(0)->setData(x, y1);
    customPlot->graph(1)->setData(x, y2);

    customPlot->rescaleAxes();

    customPlot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    customPlot->setInteraction(QCustomPlot::iSelectPlottables); // allow selection of graphs via mouse click

    customPlot->replot();

    customPlot->show();

    return a.exec();
    */
}
