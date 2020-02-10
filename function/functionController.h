#ifndef FUNCTIONCONTROLLER_H
#define FUNCTIONCONTROLLER_H

#include <QObject>
#include "functionModel.h"
#include "functionDisplayView.h"
#include "functionPointView.h"
#include "audio.h"
#include "audionotes.h"
#include "parameters.h"
#include "dragHandler.h"
#include "functionZoomer.h"
#include "pinchHandler.h"
#include "pointsinterest.h"

class FunctionController : public QObject
{
    Q_OBJECT

public:
    explicit FunctionController(QObject *parent = nullptr);
    ~FunctionController();
    Q_INVOKABLE void displayFunction(QString expression,
                                     QString minX,
                                     QString maxX,
                                     QString minY,
                                     QString maxY);

    Q_INVOKABLE void updateView();

    void setDisplayView(FunctionDisplayView *view);
    void setPointView(FunctionPointView *pointView);

    Q_INVOKABLE void zoom(double delta);
    Q_INVOKABLE void startDrag(int x, int y);
    Q_INVOKABLE void drag(int diffX, int diffY, int width, int height);
    Q_INVOKABLE void startPinch();
    Q_INVOKABLE void pinch(double scale);
    Q_INVOKABLE void nextPoint();
    Q_INVOKABLE void previousPoint();
    Q_INVOKABLE void mousePoint(int point);
    Q_INVOKABLE void nextPointInterest();
    Q_INVOKABLE void previousPointInterest();

    Q_INVOKABLE void audio();
    Q_INVOKABLE void stopAudio();

    Q_INVOKABLE bool validExpression();

    Q_INVOKABLE double minX();
    Q_INVOKABLE double maxX();
    Q_INVOKABLE double minY();
    Q_INVOKABLE double maxY();

    void setParameters(Parameters *parameters);

signals:
    void updateFinished();
    void error();
    void newInputValues(double minX, double maxX, double minY, double maxY);
    void movingPointFinished();

private slots:
    void updateDisplayView();
    void clearDisplayView();

private:
    void startAudio();
    void startNotes();
    void setPoint(int point);

    FunctionModel *m_model;
    FunctionDisplayView *m_view;
    FunctionPointView *m_pointView;
    Audio *m_audio;
    AudioNotes *m_audioNotes;
    Parameters *m_parameters;

    FunctionZoomer *m_zoomer;
    DragHandler *m_dragHandler;
    PinchHandler *m_pinchHandler;
    int m_currentPoint;
    PointsInterest *m_pointsInterest;
};

#endif // FUNCTIONCONTROLLER_H

