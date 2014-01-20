#ifndef LOGREADER_H
#define LOGREADER_H

#include <QString>
#include <QDir>
#include <QHash>

class LogReader
{
public:
    enum LogType {CARS_POS = 1, IMAGES_POS = 2};

    LogReader(const QString& pathToLogs);
    QList<QString> getAvailableDates();
    QHash<LogType, QString> getLogNamesForDate(const QString& date);
private:
    QHash<QString, QHash<LogType, QString> > _logsByDate;
};

#endif // LOGREADER_H
