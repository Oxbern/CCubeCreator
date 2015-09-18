#include "pickingray.h"

PickingRay::PickingRay()
{
    _camera = QVector3D(0,0,-50);
    _up = QVector3D(0, 1, 0);
    _fovAngle = 25;
    _zNear = 0.01;
    _x = 0;
    _y = 0;
    _result = QVector3D(0,0,0);
}

void PickingRay::setCamera(QVector3D const & cameraPosition, QVector3D const & upVector)
{
    _camera = cameraPosition;
    _up = upVector;
    compute();
}

void PickingRay::setPerspective(float viewAngleInDegrees, float nearClippingPlaneDistance)
{
    _fovAngle = viewAngleInDegrees;
    _zNear = 200;//nearClippingPlaneDistance;
    compute();
}

void PickingRay::setViewport(float width, float height)
{
    _width = width;
    _height = height;
    compute();
}

void PickingRay::setMousePosition(float x, float y)
{
    _x = x;
    _y = y;
    compute();
}

QVector3D PickingRay::get() const
{
    return _result;
}

float PickingRay::intersectionDistance(QVector3D const & closestPoint, QVector3D const & farthestPoint) const
{
    const float t1 = (closestPoint.x()  - _camera.x())/_result.x();
    const float t2 = (farthestPoint.x() - _camera.x())/_result.x();
    const float t3 = (closestPoint.y()  - _camera.y())/_result.y();
    const float t4 = (farthestPoint.y() - _camera.y())/_result.y();
    const float t5 = (closestPoint.z()  - _camera.z())/_result.z();
    const float t6 = (farthestPoint.z() - _camera.z())/_result.z();

    const float tmin = qMax(qMax(qMin(t1, t2), qMin(t3, t4)), qMin(t5, t6));
    const float tmax = qMin(qMin(qMax(t1, t2), qMax(t3, t4)), qMax(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
    if (tmax < 0)
    {
        return -1;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
        return -1;
    }

    return tmin;
}

bool PickingRay::intersects(QVector3D const & closestPoint, QVector3D const & farthestPoint) const
{
    return intersectionDistance(closestPoint, farthestPoint) >= 0;
}

void PickingRay::compute()
{
    //The view vector is the direction from the camera to the world's origin.
    QVector3D viewVector = QVector3D(0.0,0.0,0.0) - _camera; //Only works if we look at the origin.
    viewVector.normalize();

    //X and Y max coordinates on the near clipping plane
    const float yMax = _zNear * tan(_fovAngle * M_PI / 360.0);
    const float xMax = yMax * _width/_height;

    //This vector is the direction from the center of the near clipping plane to its right.
    QVector3D xVectorClippingPlane = QVector3D::crossProduct(viewVector, _up);
    xVectorClippingPlane.normalize();
    xVectorClippingPlane *= xMax;

    //This vector is the direction from the center of the near clipping plane to its top.
    QVector3D yVectorClippingPlane = QVector3D::crossProduct(xVectorClippingPlane, viewVector);
    yVectorClippingPlane.normalize();
    yVectorClippingPlane *= yMax;

    //Normalize mouse coordinates and reverse y axis.
    const float normalizedMouseX = (_x - _width/2.f) / (_width / 2.f);
    const float normalizedMouseY = - (_y - _height/2.f) / (_height / 2.f);

    //The picking ray goes to the center of the near clipping plane + mapped x y coordinates.
    _result = viewVector * _zNear + normalizedMouseX*xVectorClippingPlane + normalizedMouseY*yVectorClippingPlane;
    _result.normalize();
}

void PickingRay::clearIntersectionStack()
{
    _intersectionStack.clear();
    _intersectionDistances.clear();
    _intersection = QVector3D(-1,-1,-1);
}

void PickingRay::appendToIntersectionStack(QVector3D const & closestPoint, QVector3D const & farthestPoint)
{
    _intersectionStack.append(QList<QVector3D> { closestPoint, farthestPoint } );

    _intersectionDistances.append(intersectionDistance(closestPoint, farthestPoint));

    float min = std::numeric_limits<float>::max();
    float finalIndex = -1;
    for (int i=0 ; i<_intersectionDistances.size() ; i++)
    {
        if (_intersectionDistances[i] > -1 && _intersectionDistances[i] < min)
        {
            min = _intersectionDistances[i];
            finalIndex = i;
        }
    }

    if (finalIndex == -1)
        _intersection = QVector3D(-1,-1,-1);
    else
        _intersection = (_intersectionStack[finalIndex][0] + _intersectionStack[finalIndex][1])/2.f;
}

bool PickingRay::interesctionExistsInIntersectionStack() const
{
    return _intersection != QVector3D(-1,-1,-1);
}

QVector3D PickingRay::getIntersectionFromIntersectionStack() const
{
    return _intersection;
}

