#include "pointInterest.h"

PointsInterest::PointsInterest(TextToSpeech &textToSpeech):
    m_textToSpeech(textToSpeech)
{
    m_funcDescription = nullptr;
    m_pointInterest = 0;
    m_audioNotes = nullptr;
    m_forward = true;
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerExpired()));
}

PointsInterest::~PointsInterest()
{
    if (m_funcDescription != nullptr)
        delete m_funcDescription;
}

void PointsInterest::nextPoint(AudioNotes *audioNotes,
                               CurrentPoint *currentPoint,
                               FunctionPointView *pointView)
{
    if (m_model->lineSize() == 0)
        return;

    m_forward = true;
    start(audioNotes, currentPoint, pointView);
}

void PointsInterest::previousPoint(AudioNotes *audioNotes,
                                   CurrentPoint *currentPoint,
                                   FunctionPointView *pointView)
{
    if (m_model->lineSize() == 0)
        return;

    m_forward = false;
    start(audioNotes, currentPoint, pointView);
}

void PointsInterest::nextPointFast(CurrentPoint *currentPoint, FunctionPointView *pointView)
{
    if (m_model->lineSize() == 0)
        return;

    if (m_funcDescription == nullptr)
        m_funcDescription = new FunctionDescription;


    if (m_isUpdated == false) {
        m_points.clear();
        m_points = m_funcDescription->points(m_model);
        m_isUpdated = true;
        qDebug() << "updated";
    }

    m_currentPoint = currentPoint;
    qDebug() << "currentPoint " << m_currentPoint->X();
    m_pointView = pointView;

    m_forward = true;
    m_pointInterest = getNextPointInterest();
    qDebug() << "pointInterest " << m_pointInterest << m_points[m_pointInterest].x;
    m_currentPoint->setPoint(m_points[m_pointInterest].x);
}

void PointsInterest::previousPointFast(CurrentPoint *currentPoint, FunctionPointView *pointView)
{

    if (m_model->lineSize() == 0)
        return;
    if (m_funcDescription == nullptr)
        m_funcDescription = new FunctionDescription;


    if (m_isUpdated == false) {
        m_points.clear();
        m_points = m_funcDescription->points(m_model);
        m_isUpdated = true;
    }

    m_currentPoint = currentPoint;
    m_pointView = pointView;

    m_forward = false;
    m_pointInterest = getNextPointInterest();
    m_currentPoint->setPoint(m_points[m_pointInterest].x);
}

void PointsInterest::start(AudioNotes *audioNotes, CurrentPoint *currentPoint, FunctionPointView *pointView)
{
    if (m_funcDescription == nullptr)
        m_funcDescription = new FunctionDescription;


    if (m_isUpdated == false) {
        m_points.clear();
        m_points = m_funcDescription->points(m_model);
        m_isUpdated = true;
    }

    m_audioNotes = audioNotes;
    m_currentPoint = currentPoint;
    m_pointView = pointView;

    m_pointInterest = getNextPointInterest();

    m_timer.setInterval(2);
    m_timer.start();
}

void PointsInterest::setMode(int mode)
{
    m_mode = mode;
}

int PointsInterest::getNextPointInterest()
{
    int point = 0;
    if (m_forward == true) {
        for (int i = 0; i < m_points.size(); i++) {
            if (m_points.at(i).x < m_currentPoint->point()) {
                continue;
            } else if ((m_points.at(i).x == m_currentPoint->point())) {
                point = i + 1;
                break;
            }
            else {
                point = i;
                break;
            }
        }
    } else {
        for (int i = m_points.size() - 1; i >= 0; i--) {
            if (m_points.at(i).x > m_currentPoint->point()) {
                continue;
            } else if (m_points.at(i).x == m_currentPoint->point()) {
                point = i - 1;
                break;
            }
            else {
                point = i;
                break;
            }
        }
    }

    if (point >= m_points.size())
        point = m_points.size() - 1;

    if (point < 0)
        point = 0;

    return point;
}

void PointsInterest::stop()
{
    m_timer.stop();
    if (m_audioNotes != nullptr)
        m_audioNotes->stopNotes();
    m_isUpdated = false;
}

void PointsInterest::timerExpired()
{
    if (m_pointView == nullptr)
        return;
    if (m_audioNotes == nullptr)
        return;
    if (m_model == nullptr)
        return;

    Parameters *parameters = &Parameters::getInstance();

    if (m_forward) {
        m_currentPoint->incPoint();
        if (m_currentPoint->point() >= m_points[m_pointInterest].x) {
            m_timer.stop();
            QString label = currentPointLabel();
            m_textToSpeech.speak(label);
            emit finished();
        } else {
            m_audioNotes->setNote(m_model, m_currentPoint->point(), parameters->minFreq(), parameters->maxFreq(), parameters->useNotes(), m_mode);
        }
    } else {
        m_currentPoint->decPoint();
        if (m_currentPoint->point() <= m_points[m_pointInterest].x) {
            m_timer.stop();
            QString label = currentPointLabel();
            m_textToSpeech.speak(label);
            emit finished();
        } else {
            m_audioNotes->setNote(m_model, m_currentPoint->point(), parameters->minFreq(), parameters->maxFreq(), parameters->useNotes(), m_mode);
        }
    }
}

void PointsInterest::setModel(FunctionModel *model)
{
    m_model = model;
    m_isUpdated = false;
}

double PointsInterest::currentPointX()
{
    return m_model->x(m_points[m_pointInterest].x);
}

double PointsInterest::currentPointY()
{
    return m_points[m_pointInterest].y;
}

QString PointsInterest::currentPointLabel()
{
    return m_points[m_pointInterest].label;
}
