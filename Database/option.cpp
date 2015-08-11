#include "option.h"

using namespace Options;

/*************/
/* Interface */
/*************/

Option::Option()
{

}

/*********/
/* Blink */
/*********/

Blink::Blink()
 : Option()
{
    _position = QVector3D(0, 0, 0);
    _period = -1; //unsigned int max
}

void Blink::setPosition(QVector3D const & position)
{
    _position = position;
}

void Blink::setPeriod(unsigned period)
{
    _period = period;
}

QVector3D Blink::getPosition() const
{
    return _position;
}

unsigned Blink::getPeriod() const
{
    return _period;
}

/*************/
/* Duplicate */
/*************/

Duplicate::Duplicate()
 : Option()
{
    _vec1 = QVector3D(0, 0, 0);
    _vec2 = QVector3D(0, 0, 0);
    _vec3 = QVector3D(0, 0, 0);
}

void Duplicate::setVector1(QVector3D const & vector)
{
    _vec1 = vector;
}

void Duplicate::setVector2(QVector3D const & vector)
{
    _vec2 = vector;
}

void Duplicate::setVector3(QVector3D const & vector)
{
    _vec3 = vector;
}

QVector3D Duplicate::getVector1() const
{
    return _vec1;
}

QVector3D Duplicate::getVector2() const
{
    return _vec2;
}

QVector3D Duplicate::getVector3() const
{
    return _vec3;
}
