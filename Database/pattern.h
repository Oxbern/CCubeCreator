#ifndef PATTERN_H
#define PATTERN_H

#include "general.h"

#include "dbnode.h"
#include "option.h"

class Pattern : public DbNode
{
    public:

        explicit Pattern(QString const & name = QString(""));
        virtual ~Pattern() override;

        //LEDs
        void setOn(QVector3D const & point);
        void setOff(QVector3D const & point);
        void toggle(QVector3D const & point);
        void setAllOff();

        bool isOn(QVector3D const & point) const;
        QList<QVector3D> getAllPoints() const;

        //Options
        void addOption(Options::Option * option);
        void removeOption(int index);

        bool optionExists(Options::Option * option) const;
        int getNumberOfOptions() const;
        QList<Options::Option*> getOptions() const;

        //JSON
        virtual QJsonObject toJson(bool asRoot = false) const override;
        virtual bool setFromJson(QJsonObject const & json) override;


    private :

        QList<QVector3D> _points;
        QList<Options::Option*> _options;

};

#endif // PATTERN_H
