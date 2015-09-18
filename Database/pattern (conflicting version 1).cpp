#include "pattern.h"

Pattern::Pattern(QString const & name)
 : DbNode(name, false)
{
    _points.clear();
    _options.clear();
    _description.clear();
    _thumbnail = QImage();
}

Pattern::~Pattern()
{
    if (!_options.empty())
        qDeleteAll(_options);
}

void Pattern::clear()
{
    _points.clear();
    _description.clear();
    _thumbnail = QImage();
    if (!_options.empty())
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
    DEBUG_MSG("Added option to " << this);
    _options.append(option);
    DEBUG_MSG("Option 0 is " << _options[0]);
    DEBUG_MSG("Text is " << _options[0]->toText());
}

void Pattern::removeOption(int index)
{
    if (index >= 0 && index < _options.size())
    {
        delete _options[index];
        _options.removeAt(index);
    }
}

void Pattern::setOption(int index, QJsonObject const & json)
{
    if (index >= 0 && index < _options.size())
    {
        _options[index]->setFromJson(json);
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

Options::Option const * Pattern::getOption(int index) const
{
    return _options.at(index);
}

void Pattern::setDescription(QString const & description)
{
    _description = description;
}

void Pattern::setImage(QImage const & image)
{
    _thumbnail = image;
}

QString Pattern::getDescription() const
{
    return _description;
}

QImage Pattern::getImage() const
{
    return _thumbnail;
}

QJsonObject Pattern::toJson(bool asRoot) const
{
    QJsonObject pattern = nodeToJson(asRoot); //Start with basic stuff

    //Add description and thumbnail
    pattern["description"] = _description;
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    _thumbnail.save(&buffer, "PNG");
    pattern["image"] = QString::fromLatin1(byteArray.toBase64().data());

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

bool Pattern::setFromJson(QJsonObject const & json)
{
    //Check type
    if (getObjectType(json) != DbNodeType::Pattern)
    {
        ERROR_MSG("Cannot load something else than a pattern in a pattern object");
        return false;
    }

    //Get name
    if (json["name"] == QJsonValue::Undefined)
    {
        ERROR_MSG("Cannot find pattern name inside file");
        return false;
    }

    this->_name = json["name"].toString();

    //Get description
    if (json["description"] == QJsonValue::Undefined)
    {
        ERROR_MSG("Cannot find pattern description inside file");
        return false;
    }

    this->_description = json["description"].toString();

    //Get thumbnail
    if (json["image"] == QJsonValue::Undefined)
    {
        ERROR_MSG("Cannot find pattern image inside file");
        return false;
    }

    const QString base64 = json["image"].toString();
    this->_thumbnail.loadFromData(QByteArray(base64.toStdString().c_str()), "PNG");

    //Get points
    if (json["points"] == QJsonValue::Undefined)
    {
        ERROR_MSG("Cannot find points array of pattern " << this->_name << " inside file");
        return false;
    }

    QJsonArray points = json["points"].toArray();
    setAllOff();

    for (int i=0 ; i<points.size() ; i++)
    {
        if (points[i].toArray()[0] == QJsonValue::Undefined //x
         || points[i].toArray()[1] == QJsonValue::Undefined //y
         || points[i].toArray()[2] == QJsonValue::Undefined) //z
        {
            ERROR_MSG("Point " << i << " of pattern " << this->_name << " is missing a value");
            //Do not exit the function
        }
        else
        {
            QVector3D point(points[i].toArray()[0].toInt(), points[i].toArray()[1].toInt(), points[i].toArray()[2].toInt());
            setOn(point);
        }
    }

    //Get options
    if (json["options"] == QJsonValue::Undefined)
    {
        ERROR_MSG("Cannot find options array of pattern " << this->_name << " inside file");
        return false;
    }

    QJsonArray options = json["options"].toArray();
    if (!_options.empty())
        qDeleteAll(_options);

    for (int i=0 ; i<options.size() ; i++)
    {
        //Create option
        const Options::Type optionType = Options::getOptionType(options[i].toObject());
        Options::Option * opt = nullptr;
        if (optionType == Options::Type::Blink)
        {
            opt = static_cast<Options::Blink*>(opt);
            opt = new Options::Blink();
        }
        else if (optionType == Options::Type::Duplicate)
        {
            opt = static_cast<Options::Duplicate*>(opt);
            opt = new Options::Duplicate();
        }
        else
        {
            ERROR_MSG("Uknown type of option " << i << " of pattern " << this->_name);
            return false;
        }

        //Set option
        if (!opt->setFromJson(options[i].toObject()))
        {
            ERROR_MSG("Cannot parse option " << i << " of pattern " << this->_name);
            return false;
        }

        //Add option
        addOption(opt);
    }

    return true;
}

DbNodeType Pattern::getType() const
{
    return DbNodeType::Pattern;
}
