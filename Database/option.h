#ifndef OPTION_H
#define OPTION_H

#include "general.h"

namespace Options
{

enum class Type
{
    Blink,
    Duplicate,
    Unknown
};

Type getOptionType(QJsonObject const & json);

class Option
{
    public :

        Option();
        virtual ~Option();

        virtual QJsonObject toJson() const = 0;
        virtual bool setFromJson(QJsonObject const & json) = 0;


    protected :

};

class Blink : public Option
{
    public :

        explicit Blink(QVector3D const & position = QVector3D(0,0,0), unsigned period_ms = -1);
        virtual ~Blink() override;

        void setPosition(QVector3D const & position);
        void setPeriod(unsigned period);

        QVector3D getPosition() const;
        unsigned getPeriod() const;

        virtual QJsonObject toJson() const override;
        virtual bool setFromJson(QJsonObject const & json);


    private :

        QVector3D _position;
        unsigned _period; //in ms

};

class Duplicate : public Option
{
    public :

        explicit Duplicate(QVector3D const & vector1 = QVector3D(0,0,0), QVector3D const & vector2 = QVector3D(0,0,0), QVector3D const & vector3 = QVector3D(0,0,0));
        virtual ~Duplicate() override;

        void setVector1(QVector3D const & vector);
        void setVector2(QVector3D const & vector);
        void setVector3(QVector3D const & vector);

        QVector3D getVector1() const;
        QVector3D getVector2() const;
        QVector3D getVector3() const;

        virtual QJsonObject toJson() const override;
        virtual bool setFromJson(QJsonObject const & json);


    private :

        QVector3D _vec1, _vec2, _vec3;

};

} //namespace

#endif // OPTION_H
