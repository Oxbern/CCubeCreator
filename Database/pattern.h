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

        void clear();

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
        void setOption(int index, QJsonObject const & json);

        bool optionExists(Options::Option * option) const;
        int getNumberOfOptions() const;
        Options::Option const * getOption(int index) const;

        //Description and thumbnail
        void setDescription(QString const & description);
        void setImage(QImage const & image);
        QString getDescription() const;
        QImage getImage() const;

        //JSON
        virtual QJsonObject toJson(bool asRoot = false) const override;
        virtual bool setFromJson(QJsonObject const & json) override;

        virtual DbNodeType getType() const override;


    private :

        QList<QVector3D> _points;
        QList<Options::Option*> _options;
        QString _description;
        QImage _thumbnail;

};

#endif // PATTERN_H
