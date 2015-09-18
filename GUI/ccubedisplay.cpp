#include "ccubedisplay.h"

#include "mainwindow.h"

CCubeDisplay::CCubeDisplay(QWidget *parent)
    : QGLWidget(parent)
{
    resetView();

    viewAngle = 25.0f;
    aspectRatio = 1.0*width()/height();
    nearClippingPlane = 0.01f;
    farClippingPlane = 200.0f;

    _pickingDirection.setPerspective(viewAngle, nearClippingPlane);
    _pickingDirection.setViewport(width(), height());
    _hoveringIntersection = false;

    displayAxes = true;
    displayPositions = false;
    planeIndex = 0;

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    fps = new QTimer(this);
    connect(fps, SIGNAL(timeout()), this, SLOT(update()));
    fps->start(20);
}

void CCubeDisplay::setUndoStack(QUndoStack * undoStack)
{
    _undoStack = undoStack;
}

void CCubeDisplay::setMainWindow(MainWindow * mainWindow)
{
    _parent = mainWindow;
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
    glViewport( 0, 0, (GLint)width, (GLint)height );
    aspectRatio = 1.0*(float)width/(float)height;
    _pickingDirection.setViewport(width, height);

    /* create viewing cone with near and far clipping planes */
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        setPerspective(viewAngle, aspectRatio, nearClippingPlane, farClippingPlane);

    glMatrixMode( GL_MODELVIEW );
}

void CCubeDisplay::paintGL()
{
    //const float convexSphereDiameter = (N+1) * SPACE_BETWEEN_LEDS;

    //delete color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        lookAt(camX, camY, camZ, //Position of the eye
               0.0, 0.0, 0.0,    //Where to look
               camUpX, camUpY, camUpZ); //Up vector

        if (isEnabled())
        {
            if (displayAxes)
                drawAxes();

            if (displayPositions)
                drawPositions();
        }

        //Draw LEDs
        glShadeModel(GL_SMOOTH);
        glPushMatrix();

        const float LEDsOrigin = LEDS_TRANSLATION;
        glTranslatef(LEDsOrigin, LEDsOrigin, LEDsOrigin);

        _pickingDirection.clearIntersectionStack();

        for (float x=0 ; x<N ; x ++)
        {
            for (float y=0 ; y<N ; y ++)
            {
                for (float z=0 ; z<N ; z ++)
                {
                    glPushMatrix();
                    glTranslatef(x*SPACE_BETWEEN_LEDS, y*SPACE_BETWEEN_LEDS, z*SPACE_BETWEEN_LEDS);

                    //Color
                    if (isEnabled())
                    {
                        //LED on the active plane
                        if (y == planeIndex)
                        {
                            _pickingDirection.appendToIntersectionStack(
                                        getBoundingCubeBLandTR(LEDsOrigin + x*SPACE_BETWEEN_LEDS, LEDsOrigin + y*SPACE_BETWEEN_LEDS, LEDsOrigin + z*SPACE_BETWEEN_LEDS)[0],
                                        getBoundingCubeBLandTR(LEDsOrigin + x*SPACE_BETWEEN_LEDS, LEDsOrigin + y*SPACE_BETWEEN_LEDS, LEDsOrigin + z*SPACE_BETWEEN_LEDS)[1]);

                            if (_points.contains(QVector3D(x, y, z)))
                                glColor4f(ACTIVE_LED_ON_ACTIVE_PLANE_COLOR);
                            else
                                glColor4f(ACTIVE_PLANE_COLOR);
                        }
                        //Other LEDs
                        else
                        {
                            if (_points.contains(QVector3D(x, y, z)))
                                glColor4f(ACTIVE_LED_COLOR);
                            else
                                glColor4f(ENABLED_WIDGET_COLOR);
                        }
                    }
                    else
                        glColor4f(DISABLED_WIDGET_COLOR);

                    drawSphere();

                    glPopMatrix();
                }
            }
        }
        glPopMatrix();

        if (_pickingDirection.interesctionExistsInIntersectionStack())
        {
            if (!(QApplication::keyboardModifiers() & Qt::ControlModifier) && !(QApplication::mouseButtons() & Qt::RightButton))
                drawLEDBoudingCube(_pickingDirection.getIntersectionFromIntersectionStack());

            if (!_hoveringIntersection)
            {
                _hoveringIntersection = true;
                intersectionHoverIn();
            }
        }
        else
        {
            if (_hoveringIntersection)
            {
                _hoveringIntersection = false;
                intersectionHoverOut();
            }
        }

    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        setPerspective(viewAngle, aspectRatio, nearClippingPlane, farClippingPlane);
}

void CCubeDisplay::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        this->setCursor(Qt::ClosedHandCursor);
    }

    lastPos = event->pos();

    event->accept();
}

void CCubeDisplay::mouseReleaseEvent(QMouseEvent * event)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        this->setCursor(Qt::OpenHandCursor);
    }
    else
    {
        if (_hoveringIntersection)
        {
            this->setCursor(Qt::PointingHandCursor);

            QVector3D clickedLed = _pickingDirection.getIntersectionFromIntersectionStack();
            clickedLed -= QVector3D(LEDS_TRANSLATION, LEDS_TRANSLATION, LEDS_TRANSLATION);
            clickedLed /= (float)SPACE_BETWEEN_LEDS;
            clickedLed = openGLToCCube(clickedLed);
            _undoStack->push(new Commands::PatternToggleLED(_parent, _parent->_currentPattern, clickedLed));
        }
        else
            this->setCursor(Qt::ArrowCursor);
    }

    event->accept();
}

void CCubeDisplay::mouseMoveEvent(QMouseEvent *event)
{
    const int dx = event->x() - lastPos.x();
    const int dy = event->y() - lastPos.y();

    _pickingDirection.setMousePosition(event->x(), event->y());

    if ((event->buttons() & Qt::LeftButton && QApplication::keyboardModifiers() & Qt::ControlModifier)
        || event->buttons() & Qt::RightButton)
    {
        this->setCursor(Qt::ClosedHandCursor);
        camT -= ((float)(dx))/2.f;
        camP += ((float)(dy))/2.f;
        computeCameraCoordinates();
    }

    lastPos = event->pos();
    event->accept();
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

void CCubeDisplay::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Control)
    {
        this->setCursor(Qt::OpenHandCursor);
    }
    else
        QGLWidget::keyPressEvent(event);
}

void CCubeDisplay::keyReleaseEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Control)
    {
        if (QApplication::mouseButtons() & Qt::RightButton)
            this->setCursor(Qt::ClosedHandCursor);
        else
        {
            if (_hoveringIntersection)
                this->setCursor(Qt::PointingHandCursor);
            else
                this->setCursor(Qt::ArrowCursor);
        }
    }
    else
        QGLWidget::keyPressEvent(event);
}

void CCubeDisplay::intersectionHoverIn()
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
        return;

    if (QApplication::mouseButtons() & Qt::RightButton)
        return;

    this->setCursor(Qt::PointingHandCursor);
}

void CCubeDisplay::intersectionHoverOut()
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
        this->setCursor(Qt::OpenHandCursor);
    else
        this->setCursor(Qt::ArrowCursor);
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

    _pickingDirection.setCamera(QVector3D(camX, camY, camZ), QVector3D(camUpX, camUpY, camUpZ));
}

void CCubeDisplay::toggleAxesDisplay()
{
    displayAxes = !displayAxes;
}

void CCubeDisplay::togglePositionsDisplay()
{
    displayPositions = !displayPositions;
}

void CCubeDisplay::resetView()
{
    camR = DEFAULT_ARCBALL_RADIUS;
    camT = -180;
    camP = -90;
    computeCameraCoordinates();
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
    return QVector3D(v.z(), v.x(), v.y());
}

bool CCubeDisplay::isInsideCube(QVector3D const & v) const
{
    return v.x() >= 0 && v.x() < N
            && v.y() >= 0 && v.y() < N
            && v.z() >= 0 && v.z() < N;
}


void CCubeDisplay::drawAxes()
{
    glPushMatrix();

    const float maxDistanceToLEDs = (N - 1) * SPACE_BETWEEN_LEDS;
    glTranslatef(-maxDistanceToLEDs/2.f, -maxDistanceToLEDs/2.f, -maxDistanceToLEDs/2.f);

    glBegin(GL_LINES);

    //X
    glColor4f(X_AXIS_COLOR);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, maxDistanceToLEDs);

    //Y
    glColor4f(Y_AXIS_COLOR);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(maxDistanceToLEDs, 0.0, 0.0);

    //Z
    glColor4f(Z_AXIS_COLOR);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, maxDistanceToLEDs, 0.0);

    glEnd();

    //Legends
    drawText(0.0, 0.0, maxDistanceToLEDs + SPACE_BETWEEN_LEDS, QString("X"), X_AXIS_COLOR);
    drawText(maxDistanceToLEDs + SPACE_BETWEEN_LEDS, 0.0, 0.0, QString("Y"), Y_AXIS_COLOR);
    drawText(0.0, maxDistanceToLEDs + SPACE_BETWEEN_LEDS, 0.0, QString("Z"), Z_AXIS_COLOR);

    glPopMatrix();
}

void CCubeDisplay::drawPositions()
{
    #define POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR 0.7

    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Front"), POSITION_TEXT_COLOR);

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Right"), POSITION_TEXT_COLOR);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Back"), POSITION_TEXT_COLOR);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, -1, 0);
    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Left"), POSITION_TEXT_COLOR);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, -1, 0, 0);
    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Top"), POSITION_TEXT_COLOR);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    drawText(-4, 0, POSITION_TEXT_DISTANCE_TO_LEDS_FACTOR*N*SPACE_BETWEEN_LEDS, QString("Bottom"), POSITION_TEXT_COLOR);
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

void CCubeDisplay::drawLEDBoudingCube(QVector3D const & center)
{
    const float a = SPACE_BETWEEN_LEDS/2.f;
    const float x = center.x();
    const float y = center.y();
    const float z = center.z();

    glColor4f(TEMPORARY_INTERSECTION_CUBE_COLOR);

    glBegin(GL_QUADS);
    glVertex3f(x - a, y - a, z - a);
    glVertex3f(x - a, y + a, z - a);
    glVertex3f(x + a, y + a, z - a);
    glVertex3f(x + a, y - a, z - a);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(x - a, y - a, z - a);
    glVertex3f(x - a, y - a, z + a);
    glVertex3f(x + a, y - a, z + a);
    glVertex3f(x + a, y - a, z - a);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(x - a, y - a, z - a);
    glVertex3f(x - a, y - a, z + a);
    glVertex3f(x - a, y + a, z + a);
    glVertex3f(x - a, y + a, z - a);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(x - a, y - a, z + a);
    glVertex3f(x - a, y + a, z - a);
    glVertex3f(x + a, y + a, z + a);
    glVertex3f(x + a, y - a, z + a);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(x - a, y + a, z - a);
    glVertex3f(x - a, y + a, z + a);
    glVertex3f(x + a, y + a, z + a);
    glVertex3f(x + a, y + a, z - a);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(x + a, y - a, z - a);
    glVertex3f(x + a, y - a, z + a);
    glVertex3f(x + a, y + a, z + a);
    glVertex3f(x + a, y + a, z - a);
    glEnd();
}

QList<QVector3D> CCubeDisplay::getBoundingCubeBLandTR(float x, float y, float z) const
{
    const float a = SPACE_BETWEEN_LEDS/2.f;
    return QList<QVector3D> { QVector3D(x - a, y - a, z - a), QVector3D(x + a, y + a, z + a) };
}

void CCubeDisplay::displayPoints(QList<QVector3D> const & points)
{
    _points.clear();
    for (QVector3D v : points)
    {
        if (isInsideCube(v))
        {
            _points.append(ccubeToOpenGL(v));
        }
    }
}
