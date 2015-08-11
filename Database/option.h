#ifndef OPTION_H
#define OPTION_H

#include "general.h"

namespace Options
{

class Option
{
    public :

        Option();


    protected :

};

class Blink : public Option
{
    public :

        Blink();

        void setPosition(QVector3D const & position);
        void setPeriod(unsigned period);

        QVector3D getPosition() const;
        unsigned getPeriod() const;


    private :

        QVector3D _position;
        unsigned _period; //in ms

};

class Duplicate : public Option
{
    public :

        Duplicate();

        void setVector1(QVector3D const & vector);
        void setVector2(QVector3D const & vector);
        void setVector3(QVector3D const & vector);

        QVector3D getVector1() const;
        QVector3D getVector2() const;
        QVector3D getVector3() const;


    private :

        QVector3D _vec1, _vec2, _vec3;

};

} //namespace

#endif // OPTION_H
