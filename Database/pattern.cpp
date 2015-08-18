#include "pattern.h"

Pattern::Pattern(QString const & name)
 : DbNode(name, false)
{
    _points.clear();
    _options.clear();
}

Pattern::~Pattern()
{
    qDeleteAll(_options);
}

//LEDs
void Pattern::setOn(QVector3D const & point)
{
    if (!isOn(point))
    {
        _points.append(point);
    }
}

void Pattern::setOff(QVector3D const & point)
{
    _points.removeAll(point);
}

void Pattern::toggle(QVector3D const & point)
{
    if (isOn(point))
    {
        setOff(point);
    }
    else
    {
        setOn(point);
    }
}

void Pattern::setAllOff()
{
    _points.clear();
}

bool Pattern::isOn(QVector3D const & point) const
{
    return _points.contains(point);
}

QList<QVector3D> Pattern::getAllPoints() const
{
    return _points;
}

//Options
void Pattern::addOption(Options::Option * option)
{
    if (!optionExists(option))
        _options.append(option);
}

void Pattern::removeOption(int index)
{
    if (index < _options.size())
    {
        delete _options[index];
        _options.removeAt(index);
    }
}

bool Pattern::optionExists(Options::Option * option) const
{
    return _options.contains(option);
}

int Pattern::getNumberOfOptions() const
{
    return _options.size();
}

QList<Options::Option*> Pattern::getOptions() const
{
    return _options;
}

QJsonObject Pattern::toJson() const
{
    QJsonObject pattern = nodeToJson(); //Start with basic stuff

    //Add points
    QJsonArray jsPoints;
    for (QVector3D p : _points)
    {
        QJsonArray jsPoint;
        jsPoint.append(p.x());
        jsPoint.append(p.y());
        jsPoint.append(p.z());
        jsPoints.append(jsPoint);
    }
    pattern["points"] = jsPoints;

    //Add options
    QJsonArray jsOptions;
    for (Options::Option * opt : _options)
    {
        jsOptions.append(opt->toJson());
    }
    pattern["options"] = jsOptions;

    return pattern;
}
