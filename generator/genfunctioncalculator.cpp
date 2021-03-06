#include "genfunctioncalculator.h"
#include <limits>
#include <QElapsedTimer>
#include <QVector>

#include <QtMath>

GenFunctionCalculator::GenFunctionCalculator(GenParameters *params)
{
    m_params = params;
    unsigned long long int length = params->length();

    unsigned int numThreads = static_cast<unsigned int>(QThread::idealThreadCount());

    unsigned long long int interval = length / numThreads;


    for (unsigned int i = 0; i < numThreads - 1; i++) {
        m_tmpThread = new GenFunctionCalculatorThread(m_params,
                                                      i * interval,
                                                      (i+1)* interval);
        m_threads.push_back(m_tmpThread);
        connect(m_tmpThread, SIGNAL(finished()),
                m_tmpThread, SLOT(deleteLater()));
        m_tmpThread->start();
    }

    m_tmpThread = new GenFunctionCalculatorThread(m_params,
                                                  (numThreads - 1) * interval,
                                                  length);
    m_threads.push_back(m_tmpThread);
    connect(m_tmpThread, SIGNAL(finished()),
            m_tmpThread, SLOT(deleteLater()));
    m_tmpThread->start();

    for (unsigned int i = 0; i < numThreads; i++) {
        m_threads.at(i)->wait();
    }
}

GenFunctionCalculatorThread::GenFunctionCalculatorThread(GenParameters *params,
                                                         unsigned long long first,
                                                         unsigned long long last)
{
    m_params = params;
    m_first = first;
    m_last = last;

#ifndef Q_OS_WIN

    symbol_table.add_variable("x", m_x);
    symbol_table.add_constant("pi", M_PI);
    symbol_table.add_constant("e", M_E);
    symbol_table.add_constants();

    parser_expression.register_symbol_table(symbol_table);
#endif
}

void GenFunctionCalculatorThread::run()
{
    double start = m_params->start();
    double step = m_params->step();
    double *functionValues = m_params->functionValues();
    unsigned long long int i = 0;
    double result;

    QString expression = m_params->expression();
    std::string exp = expression.toStdString();

#ifdef Q_OS_WIN
    double x;
    m_fparser.AddConstant("pi", M_PI);
    m_fparser.AddConstant("e", M_E);

    double vals[] = { 0 };
    m_fparser.Parse(exp, "x");

    for (i = m_first; i < m_last; i++) {
        x = start + i * step;
        vals[0] = x;
        result = m_fparser.Eval(vals);

        if (result > m_params->maxY())
            result = m_params->maxY();
        if (result < m_params->minY())
            result = m_params->minY();

        functionValues[i] = result;

        if (is_nan(result)) {
            functionValues[i] = 0;
        }
    }
#else



    //    m_fparser.AddConstant("pi", M_PI);
    //    m_fparser.AddConstant("e", M_E);

    //    double vals[] = { 0 };

    //    size_t err = parser.parse(byteCode, exp, "x");
    //    if ( err  )

    //    m_fparser.Parse(exp, "x");

    //    if(res >= 0 || exp == "") {
    //        emit error(tr("Cannot understand expression.\n") + m_fparser.ErrorMsg());
    //        return false;
    //    }

    typedef exprtk::parser<double>::settings_t settings_t;

    std::size_t compile_options = settings_t::e_joiner            +
            settings_t::e_commutative_check +
            settings_t::e_strength_reduction;

    parser_t parser(compile_options);
    parser.compile(exp, parser_expression);


    for (i = m_first; i < m_last; i++) {
        m_x = start + i * step;
        //byteCode.var[0] = x;   // x is 1st in the above variables list, so it has index 0
        //result = byteCode.run();
        //        vals[0] = x;

        if (m_params->mode() == 0)
            result = parser_expression.value();//m_fparser.Eval(vals);
        else if (m_params->mode() == 1)
            result = exprtk::derivative(parser_expression, m_x);
        else if (m_params->mode() == 2)
            result = exprtk::second_derivative(parser_expression, m_x);
        //        res = m_fparser.EvalError();

        if (result > 1 * m_params->maxY())
            result = 1 * m_params->maxY();
        if (result < 1 * m_params->minY())
            result = 1 * m_params->minY();


        //        if (x > -4.0 && x < -3.0)
        //            result = 440.0;
        //        if (x >= -3.0 && x < -2.0)
        //            result = 493.88;
        //        if (x >= -2.0 && x < -1.0)
        //            result = 523.25;
        //        if (x >= -1.0 && x < 0.0)
        //            result = 587.33;
        //        if (x >= 0.0 && x < 1.0)
        //            result = 659.25;
        //        if (x >= 1.0 && x < 2.0)
        //            result = 698.46;
        //        if (x >= 2.0 && x < 3.0)
        //            result = 783.99;
        //        if (x >= 3.0 && x <= 4.0)
        //            result = 880.00;

        functionValues[i] = result;

        //        if (is_positive_infinite(result)) {
        //            functionValues[i] = std::numeric_limits<double>::max();
        //        }

        //        if (is_negative_infinite(result)) {
        //            functionValues[i] = -std::numeric_limits<double>::max();
        //        }

        if (is_nan(result)) {
            functionValues[i] = 0;
        }
        //        m_functionValues[i] = sin(x)*x*x*x - x*x*sin(x);
        //        functionValues[i] = -5/(x*x + 1);
        //        m_functionValues[i] = x;
        //        m_functionValues[i] = 5 - x;
        //        functionValues[i] = 5;
        //         functionValues[i] = sin(x)*log(x)*cos(x/3)*x^3
        //          functionValues[i] = sin(x)*log(abs(x))*x;
        //        if (x != 0.0)
        //            functionValues[i] = sin(x*x) / x;
        //        functionValues[i] = sin(x*x*x) / x;
        //        m_functionValues[i] = cos(x*x) / x;
        //        functionValues[i] = 1 / abs(x);
        //            functionValues[i] = 1 / x;

    }

#endif
}

bool GenFunctionCalculatorThread::is_positive_infinite(const double &value)
{
    double max_value = std::numeric_limits<double>::max();

    return ! ( value <= max_value );
}

bool GenFunctionCalculatorThread::is_negative_infinite(const double &value)
{
    double min_value = - std::numeric_limits<double>::max();

    return ! ( min_value <= value  );
}

bool GenFunctionCalculatorThread::is_nan(const double &value)
{
    // True if NAN
    return value != value;
}

bool GenFunctionCalculatorThread::is_valid(const double &value)
{
    return ! is_positive_infinite(value) && is_negative_infinite(value) && ! is_nan(value);
}
