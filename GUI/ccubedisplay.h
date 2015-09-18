#ifndef CCUBEDISPLAY_H
#define CCUBEDISPLAY_H

#include "general.h"
#include "pickingray.h"
#include "commands.h"

//Distances
#define SPACE_BETWEEN_LEDS 3.0
#define LED_RADIUS 0.5
#define DEFAULT_ARCBALL_RADIUS (3 * N * SPACE_BETWEEN_LEDS)
#define LEDS_TRANSLATION (-(N-1)/2.f*SPACE_BETWEEN_LEDS)

#define SCROLL_MIN_PIXEL_FOR_PLANE_CHANGE 10
#define SCROLL_PIXEL_ZOOM_FACTOR 3.0
#define SCROLL_MIN_STEPS_FOR_PLANE_CHANGE 2
#define SCROLL_STEPS_ZOOM_FACTOR 2.0

//Colors
#define OFF_LED_OPACITY         0.15f
#define ON_LED_OPACITY          0.5f
#define OTHER_LED_OPACITY       0.7f

#define DISABLED_WIDGET_COLOR               COLOR_DARK, OFF_LED_OPACITY
#define ENABLED_WIDGET_COLOR                COLOR_GRAY, OFF_LED_OPACITY
#define ACTIVE_PLANE_COLOR                  COLOR_ORANGE, OTHER_LED_OPACITY
#define ACTIVE_LED_COLOR                    COLOR_BLUE, ON_LED_OPACITY
#define ACTIVE_LED_ON_ACTIVE_PLANE_COLOR    COLOR_VIOLET, OTHER_LED_OPACITY

#define TEMPORARY_INTERSECTION_CUBE_COLOR   COLOR_VIOLET, 0.4f
#define ACTIVE_INTERSECTION_CUBE_COLOR      COLOR_VIOLET, 0.1f

#define X_AXIS_COLOR        COLOR_RED, 1.f
#define Y_AXIS_COLOR        COLOR_GREEN, 1.f
#define Z_AXIS_COLOR        COLOR_BLUE, 1.f
#define POSITION_TEXT_COLOR COLOR_WHITE, 1.f


class CCubeDisplay : public QGLWidget
{
    Q_OBJECT

    public:

        explicit CCubeDisplay(QWidget *parent = NULL);
        void setUndoStack(QUndoStack * undoStack);
        void setMainWindow(MainWindow * mainWindow);

        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent * event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void wheelEvent(QWheelEvent * event) override;
        void keyPressEvent(QKeyEvent * event) override;
        void keyReleaseEvent(QKeyEvent * event) override;
        void intersectionHoverIn();
        void intersectionHoverOut();

        void computeCameraCoordinates();


    private :

        QVector3D ccubeToOpenGL(QVector3D const& v) const;
        QVector3D openGLToCCube(QVector3D const& v) const;
        bool isInsideCube(QVector3D const & v) const;

        void drawAxes();
        void drawPositions();
        void drawSphere() const;
        void drawText(double x, double y, double z, QString const & text, float r, float g, float b, float a);

        void normalize(float * vector) const;
        void cross(float * v1, float * v2, float * res) const;
        void identity44(float * matrix) const;
        void lookAt(GLdouble eyex,    GLdouble eyey,     GLdouble eyez,
                    GLdouble centerx, GLdouble centery,  GLdouble centerz,
                    GLdouble upx,     GLdouble upy,      GLdouble upz);
        void setPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

        void drawLEDBoudingCube(QVector3D const & center);
        QList<QVector3D> getBoundingCubeBLandTR(float x, float y, float z) const;

        float camX, camY, camZ, camR, camT, camP; //Cartesian and spherical coordinates.
        float camUpX, camUpY, camUpZ; //Needed to specify the camera direction.
        float viewAngle, aspectRatio, nearClippingPlane, farClippingPlane; //Perspective values.
        PickingRay _pickingDirection;
        bool _hoveringIntersection;

        QTimer* fps;
        QPoint lastPos;
        QList<QVector3D> _points;

        bool displayAxes;
        bool displayPositions;
        int planeIndex;

        QUndoStack * _undoStack;
        MainWindow * _parent;


    public slots :

        void toggleAxesDisplay();
        void togglePositionsDisplay();
        void resetView();

        void displayPoints(QList<QVector3D> const & points = QList<QVector3D>());

};

#endif // CCUBEDISPLAY_H
