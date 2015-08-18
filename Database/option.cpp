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

Blink::Blink(QVector3D const & position, unsigned period_ms)
 : Option(),
   _position(position), _period(period_ms)
{
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

QJsonObject Blink::toJson() const
{
    QJsonObject result;

    result["option"] = "blink";

    QJsonArray vector;
    vector.append(_position.x());
    vector.append(_position.y());
    vector.append(_position.z());
    result["point"] = vector;

    result["period"] = (int)_period;

    return result;
}

/*************/
/* Duplicate */
/*************/

Duplicate::Duplicate(QVector3D const & vector1, QVector3D const & vector2, QVector3D const & vector3)
 : Option(),
   _vec1(vector1), _vec2(vector2), _vec3(vector3)
{
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

QJsonObject Duplicate::toJson() const
{
    QJsonObject result;

    result["option"] = "duplicate";

    QJsonArray vector1;
    vector1.append(_vec1.x());
    vector1.append(_vec1.y());
    vector1.append(_vec1.z());
    result["i"] = vector1;

    QJsonArray vector2;
    vector2.append(_vec2.x());
    vector2.append(_vec2.y());
    vector2.append(_vec2.z());
    result["j"] = vector2;

    QJsonArray vector3;
    vector3.append(_vec3.x());
    vector3.append(_vec3.y());
    vector3.append(_vec3.z());
    result["k"] = vector3;

    return result;
}
