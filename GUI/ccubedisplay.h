#ifndef CCUBEDISPLAY_H
#define CCUBEDISPLAY_H

#include "general.h"

#define SPACE_BETWEEN_LEDS 3.0
#define LED_RADIUS 0.5
#define DEFAULT_ARCBALL_RADIUS (3 * N * SPACE_BETWEEN_LEDS)

#define SCROLL_MIN_PIXEL_FOR_PLANE_CHANGE 10
#define SCROLL_PIXEL_ZOOM_FACTOR 3.0
#define SCROLL_MIN_STEPS_FOR_PLANE_CHANGE 2
#define SCROLL_STEPS_ZOOM_FACTOR 2.0

class CCubeDisplay : public QGLWidget
{
    Q_OBJECT

    public:

        explicit CCubeDisplay(QWidget *parent = NULL);

        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void wheelEvent(QWheelEvent * event) override;

        void computeCameraCoordinates();


    private :

        QVector3D ccubeToOpenGL(QVector3D const& v) const;
        QVector3D openGLToCCube(QVector3D const& v) const;

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

        float camX, camY, camZ, camR, camT, camP; //Cartesian and spherical coordinates.
        float camUpX, camUpY, camUpZ; //Needed to specify the camera direction.
        float rotY;
        QTimer* fps;
        QPoint lastPos;
        QList<QVector3D> points;

        bool displayAxes;
        bool displayPositions;
        int planeIndex;


    public slots :

        //void recevoir_nouveaux_points(QList<QVector3D> const& p);
        void toggleAxesDisplay();
        void togglePositionsDisplay();
        void resetView();
};

#endif // CCUBEDISPLAY_H
