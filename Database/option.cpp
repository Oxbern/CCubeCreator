#include "option.h"

using namespace Options;

Type Options::getOptionType(QJsonObject const & json)
{
    if (json["option"] == QJsonValue::Undefined)
    {
        ERROR_MSG("Cannot find option type: database/pattern file is probably corrupted");
        return Type::Unknown;
    }

    if (json["option"].toString() == QString("blink"))
    {
        return Type::Blink;
    }
    else if (json["option"].toString() == QString("duplicate"))
    {
        return Type::Duplicate;
    }
    else
        return Type::Unknown;
}

/*************/
/* Interface */
/*************/

Option::Option()
{

}

Option::~Option()
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

Blink::~Blink()
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

bool Blink::setFromJson(QJsonObject const & json)
{
    if (getOptionType(json) != Type::Blink)
    {
        ERROR_MSG("Option should be of type Blink");
        return false;
    }

    if (json["point"] == QJsonValue::Undefined
            || json["point"].toArray()[0] == QJsonValue::Undefined
            || json["point"].toArray()[1] == QJsonValue::Undefined
            || json["point"].toArray()[2] == QJsonValue::Undefined
            || json["period"] == QJsonValue::Undefined)
    {
        ERROR_MSG("An attribute is missing for the Blink option");
        return false;
    }

    _position.setX(json["point"].toArray()[0].toInt());
    _position.setY(json["point"].toArray()[1].toInt());
    _position.setZ(json["point"].toArray()[2].toInt());
    _period = json["period"].toInt();

    return true;
}

/*************/
/* Duplicate */
/*************/

Duplicate::Duplicate(QVector3D const & vector1, QVector3D const & vector2, QVector3D const & vector3)
 : Option(),
   _vec1(vector1), _vec2(vector2), _vec3(vector3)
{
}

Duplicate::~Duplicate()
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

bool Duplicate::setFromJson(QJsonObject const & json)
{
    if (getOptionType(json) != Type::Duplicate)
    {
        ERROR_MSG("Option should be of type Duplicate");
        return false;
    }

    if (   json["i"] == QJsonValue::Undefined
        || json["i"].toArray()[0] == QJsonValue::Undefined
        || json["i"].toArray()[1] == QJsonValue::Undefined
        || json["i"].toArray()[2] == QJsonValue::Undefined
        || json["j"] == QJsonValue::Undefined
        || json["j"].toArray()[0] == QJsonValue::Undefined
        || json["j"].toArray()[1] == QJsonValue::Undefined
        || json["j"].toArray()[2] == QJsonValue::Undefined
        || json["k"] == QJsonValue::Undefined
        || json["k"].toArray()[0] == QJsonValue::Undefined
        || json["k"].toArray()[1] == QJsonValue::Undefined
        || json["k"].toArray()[2] == QJsonValue::Undefined)
    {
        ERROR_MSG("An attribute is missing for the Duplicate option");
        return false;
    }

    _vec1.setX(json["i"].toArray()[0].toInt());
    _vec1.setY(json["i"].toArray()[1].toInt());
    _vec1.setZ(json["i"].toArray()[2].toInt());

    _vec2.setX(json["j"].toArray()[0].toInt());
    _vec2.setY(json["j"].toArray()[1].toInt());
    _vec2.setZ(json["j"].toArray()[2].toInt());

    _vec3.setX(json["k"].toArray()[0].toInt());
    _vec3.setY(json["k"].toArray()[1].toInt());
    _vec3.setZ(json["k"].toArray()[2].toInt());

    return true;
}
