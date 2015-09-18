#ifndef PICKINGRAY_H
#define PICKINGRAY_H

#include "general.h"

//This class represents a vector going from the camera to the mouse pointer projected to 3D scene on the near clipping plane.
//It assumes we're always looking at the world's origin.

class PickingRay
{
    public:

        PickingRay();

        void setCamera(QVector3D const & cameraPosition, QVector3D const & upVector);
        void setPerspective(float viewAngleInDegrees, float nearClippingPlaneDistance);
        void setViewport(float width, float height);
        void setMousePosition(float x, float y);

        QVector3D get() const;
        float intersectionDistance(QVector3D const & closestPoint, QVector3D const & farthestPoint) const;
        bool intersects(QVector3D const & closestPoint, QVector3D const & farthestPoint) const;

        void clearIntersectionStack();
        void appendToIntersectionStack(QVector3D const & closestPoint, QVector3D const & farthestPoint);
        bool interesctionExistsInIntersectionStack() const;
        QVector3D getIntersectionFromIntersectionStack() const;

    private:

        void compute();

        QVector3D _camera;
        QVector3D _up;
        float _fovAngle;
        float _zNear;
        float _x, _y;
        float _width, _height;
        QVector3D _result;

        QList< QList<QVector3D> > _intersectionStack;
        QList<float> _intersectionDistances;
        QVector3D _intersection;
};

#endif // PICKINGRAY_H
