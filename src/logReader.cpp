#include "logReader.h"
#include <QDebug>

LogReader::LogReader(const QString &pathToLogs)
{
    QDir dir = QDir(pathToLogs);
    for (const QString& log: dir.entryList())
    {
        size_t dotPos = log.lastIndexOf(".");
        size_t log_pos = log.lastIndexOf("log_");
        QString date = log.mid(log_pos, dotPos - log_pos);
        qDebug() << "date" << date;
        if (log.contains("car"))
        {
            _logsByDate[date][CARS_POS] = pathToLogs + log;
        }
        else if (log.contains("image"))
        {
            _logsByDate[date][IMAGES_POS] = pathToLogs + log;
        }
    }
}

QList<QString> LogReader::getAvailableDates()
{
    return _logsByDate.keys();
}

QHash<LogReader::LogType, QString> LogReader::getLogNamesForDate(const QString& date)
{
    if (_logsByDate[date].size() == 2)
        return _logsByDate[date];
    QHash<LogType, QString> emptyHash;
    return emptyHash;
}
