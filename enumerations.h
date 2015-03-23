#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

#include <QObject>
#include <QHash>
#include <QJsonObject>
#include <QJsonArray>

class Enumerations
{
public:
    Enumerations();

    enum item_type {
        EIT_UNKNOWN = 0,
        EIT_ISSUE,
        EIT_TIME_ENTRY,
        EIT_DOCUMENT,
    };

    struct enumeration {
        int             id;
        QString         name;
        enum item_type  item_type;
        QString         field_name;
    };

    struct enumeration get(int enumeration_id);
    QHash<QString, struct enumeration> get(enum item_type item_type);
    void set(QJsonArray json_array);

private:

    QHash <int, struct enumeration> id2enumeration;
    QHash <enum item_type, QHash<QString, struct enumeration>> itemType2enumeration;
    //QHash <QString, struct enumeration> name2enumeration;

    void add(QJsonObject json_enumeration);
    void clear();

};

#endif // ENUMERATIONS_H
