#include "audionotes.h"
#include "math.h"
#include "constants.h"

#include <QDebug>

AudioNotes::AudioNotes()
{
    m_model = nullptr;
    m_timer.setTimerType(Qt::PreciseTimer);
    m_timer.setInterval(50);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerExpired()));
    m_audioPoints = new AudioPoints();
    connect(m_audioPoints, SIGNAL(finished()), this, SIGNAL(finished()));
}

AudioNotes::~AudioNotes()
{
    delete m_audioPoints;
}

void AudioNotes::startNotes(FunctionModel *model,
                            int fmin,
                            int fmax,
                            int duration,
                            int mode)
{
    m_model = model;
    m_fmin = fmin;
    m_fmax = fmax;
    m_duration = duration * 1000;
    m_mode = mode;
    m_timeElapsed = 0;
    m_timer.start();
}

void AudioNotes::setNoteFromMouse(FunctionModel *model, int mouseX, int width, int fmin, int fmax, bool useNotes, int mode)
{
    m_model = model;

    if (m_model == nullptr)
        return;
    if (m_model->lineSize() == 0)
        return;

    m_mouseX = mouseX;
    m_fmin = fmin;
    m_fmax = fmax;

    if (m_mouseX < 0)
        m_mouseX = 0;
    if (m_mouseX > width)
        m_mouseX = width;

    double ratio = (double) m_mouseX / width;

    int i = round((1.0 * m_mouseX / width) * LINE_POINTS);
    if (i < 0)
        i = 0;
    if (i >= LINE_POINTS)
        i = LINE_POINTS - 1;

    double min = m_model->minValue();
    double max = m_model->maxValue();

    if (max > 1 * m_model->maxY())
        max = 1 * m_model->maxY();
    if (min < 1 * m_model->minY())
        min = 1 * m_model->minY();

    double a;
    double b;
    double l;
    double freq;
    bool n = true;
    if (max != min) {
        m_fmin = 110;
        m_fmax = 830;
        a =  (m_fmax-m_fmin)/(max - min);
        b = m_fmax - a * max;
        if (mode == 0)
            l = m_model->y(i);
        else if (mode == 1)
            l = m_model->derivative(i);
        else if (mode == 2)
            l = m_model->derivative2(i);

        if (l >= 0)
            n = true;
        else
            n = false;
        freq = a * l + b;

        if (freq > m_fmax)
            freq = m_fmax;
        if (freq < m_fmin)
            freq = m_fmin;

        if (m_model->isValid(i))
            m_audioPoints->setFreq(freq, useNotes, n, ratio);
        else
            m_audioPoints->setFreq(0, useNotes, n, ratio);
    } else {
        m_audioPoints->setFreq((m_fmax - m_fmin) / 2, useNotes, n, ratio);
    }
}

void AudioNotes::setNote(FunctionModel *model, int currentPoint, int fmin, int fmax, bool useNotes, int mode)
{
    m_model = model;

    if (m_model == nullptr)
        return;

    if (m_model->lineSize() == 0)
        return;

    m_fmin = fmin;
    m_fmax = fmax;
    m_currentPoint = currentPoint;

    if (m_currentPoint < 0)
        m_currentPoint = 0;
    if (m_currentPoint >= LINE_POINTS)
        m_currentPoint = LINE_POINTS - 1;

    double min = m_model->minValue();
    double max = m_model->maxValue();

    if (max > 1 * m_model->maxY())
        max = 1 * m_model->maxY();
    if (min < 1 * m_model->minY())
        min = 1 * m_model->minY();

    double ratio = (double) m_currentPoint / LINE_POINTS;

    double a;
    double b;
    double l;
    double freq;
    bool n = true;
    if (max != min) {
        m_fmin = 110;
        m_fmax = 830;
        a =  (m_fmax-m_fmin)/(max - min);
        b = m_fmax - a * max;
        if (mode == 0)
            l = m_model->y(m_currentPoint);
        else if (mode == 1)
            l = m_model->derivative(m_currentPoint);
        else if (mode == 2)
            l = m_model->derivative2(m_currentPoint);
        if (l >= 0)
            n = true;
        else
            n = false;
        freq = a * l + b;

        if (freq > m_fmax)
            freq = m_fmax;
        if (freq < m_fmin)
            freq = m_fmin;

        if (m_model->isValid(m_currentPoint))
            m_audioPoints->setFreq(freq, useNotes, n, ratio);
        else
            m_audioPoints->setFreq(0, useNotes, n, ratio);
    } else {
        m_audioPoints->setFreq((m_fmax - m_fmin) / 2, useNotes, n, ratio);
    }
}

void AudioNotes::stopNotes()
{
    m_timer.stop();
    m_audioPoints->stopAudio();
}

void AudioNotes::timerExpired()
{
    m_timeElapsed += m_timer.interval();
    if (m_timeElapsed > m_duration) {
        stopNotes();
        return;
    }

    double cx = (double) m_timeElapsed / m_duration;
    int i = round(cx * LINE_POINTS);
    if (i >= LINE_POINTS)
        i = LINE_POINTS - 1;
    if (i < 0)
        i = 0;

    double min = m_model->minValue();
    if (min < 1 * m_model->minY())
        min = 1 * m_model->minY();
    double max = m_model->maxValue();
    if (max > 1 * m_model->maxX())
        max = 1 * m_model->maxY();
    double a;
    double b;
    double l;
    double freq;
    bool n = true;
    if (max != min) {
        m_fmin = 220;
        m_fmax = 493;
        a =  (m_fmax-m_fmin)/(max - min);
        b = m_fmax - a * max;
        if (m_mode == 0)
            l = m_model->y(i);
        else if (m_mode == 1)
            l = m_model->derivative(i);
        else if (m_mode == 2)
            l = m_model->derivative2(i);

        if (l >= 0)
            n = true;
        else
            n = false;
        freq = a * l + b;
        if (freq < m_fmin)
            freq = m_fmin;
        if (freq > m_fmax)
            freq = m_fmax;

        if (m_model->isValid(i))
            m_audioPoints->setFreq(freq, true, n, cx);
        else
            m_audioPoints->setFreq(0, true, n, cx);
    } else {
        m_audioPoints->setFreq((m_fmax - m_fmin) / 2, true, n, cx);
    }
}
