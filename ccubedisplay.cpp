#include "ccubedisplay.h"

CCubeDisplay::CCubeDisplay(QWidget *parent)
    : QGLWidget(parent)
{
    camR = DEFAULT_ARCBALL_RADIUS;
    camT = -180;
    camP = -90;
    rotY = 0;
    computeCameraCoordinates();

    displayAxes = true;
    displayPositions = false;
    planeIndex = 0;

    fps = new QTimer(this);
    connect(fps, SIGNAL(timeout()), this, SLOT(updateGL()));
    fps->start(20);
}

void CCubeDisplay::initializeGL()
{
    QGLWidget::setFormat(QGLFormat(QGL::SampleBuffers));

    //activate the depth buffer
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

void CCubeDisplay::resizeGL(int width, int height)
{
    qDebug() << width << height;
    glViewport( 0, 0, (GLint)width, (GLint)height );

    /* create viewing cone with near and far clipping planes */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 30.0);

    glMatrixMode( GL_MODELVIEW );
}

void CCubeDisplay::paintGL()
{
    const float convexSphereDiameter = (N+1) * SPACE_BETWEEN_LEDS;

    //delete color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        lookAt(camX, camY, camZ, //Position of the eye
               0.0, 0.0, 0.0,    //Where to look
               camUpX, camUpY, camUpZ); //Up vector

        if (displayAxes)
            drawAxes();

        if (displayPositions)
            drawPositions();

        //On affiche les LEDS
        glShadeModel(GL_SMOOTH);
        //drawSphere();
        for (int x=0 ; x<N ; x++)
        {
            for (int y=0 ; y<N ; y++)
            {
                for (int z=0 ; z<N ; z++)
                {
                    glPushMatrix();
                    glTranslatef((x-(N-1)/2.f)*SPACE_BETWEEN_LEDS, (y-(N-1)/2.f)*SPACE_BETWEEN_LEDS, (z-(N-1)/2.f)*SPACE_BETWEEN_LEDS);

                    if (y == planeIndex)
                    {
                        glColor4f(1.0f, 0.5f, 0.5f, 0.5f);
                    }
                    else
                    {
                        glColor4f(0.5f, 0.5f, 1.0f, 0.15f);
                    }
                    drawSphere();

                    glPopMatrix();
                }
            }
        }

    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        //glFrustum(- convexSphereDiameter/2.f * width()/height(), convexSphereDiameter/2.f * width()/height(), -convexSphereDiameter/2.f, convexSphereDiameter/2.f, 5.0, 5.0 + convexSphereDiameter);
        setPerspective(25.0f, 1.0*width()/height(), 0.01f, 200.0f);
}

void CCubeDisplay::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void CCubeDisplay::mouseMoveEvent(QMouseEvent *event)
{
    const int dx = event->x() - lastPos.x();
    const int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton)
    {
        if (QApplication::keyboardModifiers() & Qt::ControlModifier)
        {
            camT -= dx/2;
            camP += dy/2;
            rotY += dy/2;
            computeCameraCoordinates();
        }
    }

    lastPos = event->pos();
}

void CCubeDisplay::wheelEvent(QWheelEvent * event)
{
    const QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull())
    {
        const int length = numPixels.manhattanLength();
        const int sign = numPixels.x() < 0 || numPixels.y() < 0 ? -1 : 1;

        if (QApplication::keyboardModifiers() & Qt::ControlModifier)
        {
            //Zoom
            camR *= sign * length * SCROLL_PIXEL_ZOOM_FACTOR;
            computeCameraCoordinates();
        }
        else
        {
            //Change plane
            planeIndex += sign;
        }
    }
    else if (!numDegrees.isNull())
    {
        numDegrees /= 15;
        const int sign = numDegrees.y() < 0 ? -1 : 1;

        if (QApplication::keyboardModifiers() & Qt::ControlModifier)
        {
            //Zoom
            camR -= sign * SCROLL_STEPS_ZOOM_FACTOR;
            camR = qBound(0.f, camR, 10.f * N * (float)SPACE_BETWEEN_LEDS);
            DEBUG_MSG(camR);
            computeCameraCoordinates();
        }
        else
        {
            //Change plane
            planeIndex += sign;
        }
    }

    planeIndex = qBound(-1, planeIndex, N); //Clamp planevalue

    event->accept();
}

void CCubeDisplay::computeCameraCoordinates()
{
    camY = camR*qCos(qDegreesToRadians(camP));
    camZ = camR*qSin(qDegreesToRadians(camP))*qCos(qDegreesToRadians(camT));
    camX = camR*qSin(qDegreesToRadians(camP))*qSin(qDegreesToRadians(camT));

    //qDebug() << camR << camT << camP << "|" << camX << camY << camZ;

    if (camX == 0 && camY >= 0 && camZ == 0)
    {
        camUpX = 1;
        camUpY = 0;
        camUpZ = 0;
    }
    else
    {
        camUpX = 0;
        camUpY = 1;
        camUpZ = 0;
    }
}

QVector3D CCubeDisplay::ccubeToOpenGL(QVector3D const& v) const
{
    /* On the CCube :
     * x to the front
     * y to the right
     * z to the top
     * (origin on the bottom left corner, behind)
     * Avec OpenGl :
     * z to the front
     * x to the right
     * y to the top
     *
     * CCube <-> OpenGL :
     *  - x <-> z
     *  - y <-> x
     *  - z <-> y
     */

    return QVector3D(v.y(), v.z(), v.x());
}

QVector3D CCubeDisplay::openGLToCCube(QVector3D const& v) const
{
    return QVector3D(v.y(), v.z(), v.x());
}

void CCubeDisplay::drawAxes()
{
    glPushMatrix();

    const float maxDistanceToLEDs = (N - 1) * SPACE_BETWEEN_LEDS;
    glTranslatef(-maxDistanceToLEDs/2.f, -maxDistanceToLEDs/2.f, -maxDistanceToLEDs/2.f);

    glBegin(GL_LINES);

    //X
    glColor4f(COLOR_RED);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, maxDistanceToLEDs);

    //Y
    glColor4f(COLOR_GREEN);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(maxDistanceToLEDs, 0.0, 0.0);

    //Z
    glColor4f(COLOR_BLUE);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, maxDistanceToLEDs, 0.0);

    glEnd();

    //Legends
    drawText(0.0, 0.0, maxDistanceToLEDs + SPACE_BETWEEN_LEDS, QString("X"), COLOR_RED);
    drawText(maxDistanceToLEDs + SPACE_BETWEEN_LEDS, 0.0, 0.0, QString("Y"), COLOR_GREEN);
    drawText(0.0, maxDistanceToLEDs + SPACE_BETWEEN_LEDS, 0.0, QString("Z"), COLOR_BLUE);

    glPopMatrix();
}

void CCubeDisplay::drawPositions()
{
    #define POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR 0.7

    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Front"), COLOR_WHITE);

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Right"), COLOR_WHITE);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Back"), COLOR_WHITE);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, -1, 0);
    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Left"), COLOR_WHITE);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, -1, 0, 0);
    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Top"), COLOR_WHITE);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Bottom"), COLOR_WHITE);
    glPopMatrix();
}

void CCubeDisplay::drawSphere() const
{
    //Basic sphere divided into quads.

    #define SPHERE_PRECISION 10 //Number of latitudes and longitudes

    for(int i = 0; i <= SPHERE_PRECISION; i++)
    {
        const double lat0 = M_PI * (-0.5 + (double) (i - 1) / SPHERE_PRECISION);
        const double z0  = sin(lat0);
        const double zr0 =  cos(lat0);

        const double lat1 = M_PI * (-0.5 + (double) i / SPHERE_PRECISION);
        const double z1 = sin(lat1);
        const double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);

        for(int j = 0; j <= SPHERE_PRECISION; j++)
        {
            const double lng = 2 * M_PI * (double) (j - 1) / SPHERE_PRECISION;
            const double x = cos(lng);
            const double y = sin(lng);

            glNormal3f(x * zr0 * LED_RADIUS, y * zr0 * LED_RADIUS, z0 * LED_RADIUS);
            glVertex3f(x * zr0 * LED_RADIUS, y * zr0 * LED_RADIUS, z0 * LED_RADIUS);
            glNormal3f(x * zr1 * LED_RADIUS, y * zr1 * LED_RADIUS, z1 * LED_RADIUS);
            glVertex3f(x * zr1 * LED_RADIUS, y * zr1 * LED_RADIUS, z1 * LED_RADIUS);
        }

        glEnd();
    }
}

void CCubeDisplay::drawText(double x, double y, double z, QString const & text, float r, float g, float b, float a)
{
    /* The trick here is to get the vertices from a path computed by Qt.
     * It's pretty crappy but it kind of works.
     */

    QPainterPath path;
    //glDisable(GL_LIGHTING);

    path.addText(QPointF(0, 0), QFont("Arial", 2), text);

    QList<QPolygonF> poly = path.toFillPolygons();
    for (QList<QPolygonF>::iterator i = poly.begin(); i != poly.end(); i++)
    {
        glBegin(GL_LINE_LOOP);
        glColor4f(r, g, b, a);

        for (QPolygonF::iterator p = i->begin(); p != i->end(); p++)
            glVertex3f(p->x()+x, -p->y()+y, z);

        glEnd();
    }

    //glEnable(GL_LIGHTING);
}


void CCubeDisplay::normalize(float * vector) const
{
    const float length = sqrt(pow(vector[0], 2) + pow(vector[1], 2) + pow(vector[2], 2));
    vector[0] /= length;
    vector[1] /= length;
    vector[2] /= length;
}

void CCubeDisplay::cross(float * v1, float * v2, float * res) const
{
    res[0] = v1[1] * v2[2] - v1[2] * v2[1];
    res[1] = v1[2] * v2[0] - v1[0] * v2[2];
    res[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void CCubeDisplay::identity44(float * matrix) const
{
    for (unsigned i=0 ; i<16 ; i++)
    {
        matrix[i] = 0.f;
    }

    matrix[0] = 1.f;
    matrix[5] = 1.f;
    matrix[10] = 1.f;
    matrix[15] = 1.f;
}

void CCubeDisplay::lookAt(GLdouble eyex,    GLdouble eyey,     GLdouble eyez,
                             GLdouble centerx, GLdouble centery,  GLdouble centerz,
                             GLdouble upx,     GLdouble upy,      GLdouble upz)
{
    //Same as gluLookAt().

    float forward[3], side[3], up[3];
    GLfloat m[4][4];

    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    normalize(forward);

    /* Side = forward x up */
    cross(forward, up, side);
    normalize(side);

    /* Recompute up as: up = side x forward */
    cross(side, forward, up);

    identity44(&m[0][0]);
    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = up[0];
    m[1][1] = up[1];
    m[2][1] = up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

    glMultMatrixf(&m[0][0]);
    glTranslated(-eyex, -eyey, -eyez);
}

void CCubeDisplay::setPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    //This function is the same as gluPerspective().

    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan( fovy * M_PI / 360.0 );
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}
