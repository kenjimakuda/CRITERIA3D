#include "dbMeteoPoints.h"
#include "commonConstants.h"
#include "meteo.h"
#include "meteoPoint.h"
#include "interpolationSettings.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QString>
#include <QStringBuilder>
#include <QUuid>


Crit3DMeteoPointsDbHandler::Crit3DMeteoPointsDbHandler(QString provider_, QString host_, QString dbname_, int port_,
                                                       QString user_, QString pass_)
{
    error = "";

    if(_db.isOpen())
    {
        qDebug() << _db.connectionName() << "is already open";
        _db.close();
    }

    _db = QSqlDatabase::addDatabase(provider_, QUuid::createUuid().toString());
    _db.setDatabaseName(dbname_);

    if (provider_ != "QSQLITE")
    {
        _db.setHostName(host_);
        _db.setPort(port_);
        _db.setUserName(user_);
        _db.setPassword(pass_);
    }

    if (!_db.open())
       error = _db.lastError().text();    

}

Crit3DMeteoPointsDbHandler::Crit3DMeteoPointsDbHandler(QString dbname_)
{
    error = "";

    if(_db.isOpen())
    {
        qDebug() << _db.connectionName() << "is already open";
        _db.close();
    }

    _db = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    _db.setDatabaseName(dbname_);

    if (!_db.open())
       error = _db.lastError().text();

}

Crit3DMeteoPointsDbHandler::~Crit3DMeteoPointsDbHandler()
{
    if ((_db.isValid()) && (_db.isOpen()))
    {
        _db.close();
        _db.removeDatabase(_db.connectionName());
    }
}


QString Crit3DMeteoPointsDbHandler::getDbName()
{
    return _db.databaseName();
}

int Crit3DMeteoPointsDbHandler::getIdfromMeteoVar(meteoVariable meteoVar)
{

    std::map<int, meteoVariable>::const_iterator it;
    int key = NODATA;

    for (it = MapIdMeteoVar.begin(); it != MapIdMeteoVar.end(); ++it)
    {
        if (it->second == meteoVar)
        {
            key = it->first;
            break;
        }
    }
    return key;
}


QString Crit3DMeteoPointsDbHandler::getDatasetURL(QString dataset)
{
    QSqlQuery qry(_db);
    QString url = nullptr;

    qry.prepare( "SELECT URL FROM datasets WHERE dataset = :dataset");
    qry.bindValue(":dataset", dataset);

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        if (qry.next())
            url = qry.value(0).toString();

        else
            qDebug( "Error: dataset not found" );
    }

    return url;
}


QStringList Crit3DMeteoPointsDbHandler::getDatasetsActive()
{
    QStringList activeList;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT dataset FROM datasets WHERE active = 1" );

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        while (qry.next())
        {
            QString active = qry.value(0).toString();
            activeList << active;

        }

    }

    return activeList;

}

void Crit3DMeteoPointsDbHandler::setDatasetsActive(QString active)
{
    QString statement = QString("UPDATE datasets SET active = 0");
    _db.exec(statement);

    statement = QString("UPDATE datasets SET active = 1 WHERE dataset IN ( %1 )").arg(active);
    _db.exec(statement);
}


QStringList Crit3DMeteoPointsDbHandler::getDatasetsList()
{
    QStringList datasetList;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT * FROM datasets" );

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        while (qry.next())
        {
            QString dataset = qry.value(0).toString();
            datasetList << dataset;
        }
    }

    return datasetList;
}


QDateTime Crit3DMeteoPointsDbHandler::getLastDate(frequencyType frequency)
{
    QSqlQuery qry(_db);
    QStringList tables;
    QDateTime lastDay(QDate(1800, 1, 1), QTime(0, 0, 0));

    QString dayHour;
    if (frequency == daily)
        dayHour = "D";
    else if (frequency == hourly)
        dayHour = "H";

    qry.prepare( "SELECT name FROM sqlite_master WHERE type='table' AND name like :dayHour ESCAPE '^'");
    qry.bindValue(":dayHour",  "%^_" + dayHour  + "%");

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        while (qry.next())
        {
            QString table = qry.value(0).toString();
            tables << table;
        }
    }

    QDateTime date;
    QString dateStr, statement;
    foreach (QString table, tables)
    {
        //statement = QString( "SELECT date_time FROM `%1` ORDER BY datetime(date_time) DESC Limit 1").arg(table);
        statement = QString( "SELECT MAX(date_time) FROM `%1` AS dateTime").arg(table);
        if( !qry.exec(statement) )
        {
            //qDebug() << qry.lastError();
        }
        else
        {
            if (qry.next())
            {
                dateStr = qry.value(0).toString();
                if (!dateStr.isEmpty())
                {
                    if (frequency == daily)
                    {
                        date = QDateTime::fromString(dateStr,"yyyy-MM-dd");
                    }
                    else if (frequency == hourly)
                    {
                        date = QDateTime::fromString(dateStr,"yyyy-MM-dd HH:mm:ss");
                        if (date.time().hour() == 0)
                        {
                            date = date.addDays(-1);
                        }
                    }

                    if (date > lastDay)
                    {
                        lastDay = date;
                    }
                }
            }
        }
    }

    return lastDay;
}


QDateTime Crit3DMeteoPointsDbHandler::getFirstDay(frequencyType frequency)
{

    QSqlQuery qry(_db);
    QStringList tables;
    QDateTime firstDay(QDate::currentDate().addDays(1), QTime(0, 0, 0));

    QString dayHour;
    if (frequency == daily)
        dayHour = "D";
    else if (frequency == hourly)
        dayHour = "H";

    qry.prepare( "SELECT name FROM sqlite_master WHERE type='table' AND name like :dayHour ESCAPE '^'");
    qry.bindValue(":dayHour",  "%^" + dayHour  + "%");

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        while (qry.next())
        {
            QString table = qry.value(0).toString();
            tables << table;
        }
    }

    QString statement;
    foreach (QString table, tables)
    {
        //statement = QString( "SELECT date_time FROM `%1` ORDER BY datetime(date_time) ASC Limit 1").arg(table);
        statement = QString( "SELECT MIN(date_time) FROM `%1` AS dateTime").arg(table);
        if( !qry.exec(statement) )
        {
            qDebug() << qry.lastError();
        }
        else
        {
            if (qry.next())
            {
                QString dateStr = qry.value(0).toString();
                if (!dateStr.isEmpty())
                {
                    QDateTime date = QDateTime::fromString(dateStr,"yyyy-MM-dd HH:mm:ss");
                    if (date < firstDay)
                    {
                        firstDay = date;
                    }
                }
            }
        }
    }
    if (firstDay.date() == QDate::currentDate().addDays(1))
    {
        firstDay.date() = QDate(1800,1,1);
        firstDay.time() = QTime(0, 0, 0);
    }

    return firstDay;

}


bool Crit3DMeteoPointsDbHandler::loadDailyData(Crit3DDate dateStart, Crit3DDate dateEnd, Crit3DMeteoPoint *meteoPoint)
{
    QString dateStr;
    meteoVariable variable;
    QDate d;
    int idVar;
    float value;

    int numberOfDays = difference(dateStart, dateEnd) +1;
    QString startDate = QString::fromStdString(dateStart.toStdString());
    QString endDate = QString::fromStdString(dateEnd.toStdString());

    QSqlQuery myQuery(_db);

    meteoPoint->initializeObsDataD(numberOfDays, dateStart);

    QString tableName = QString::fromStdString(meteoPoint->id) + "_D";

    QString statement = QString( "SELECT * FROM `%1` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')")
                                .arg(tableName).arg(startDate).arg(endDate);

    if( !myQuery.exec(statement) )
    {
        //qDebug() << myQuery.lastError();
        return false;
    }
    else
    {
        while (myQuery.next())
        {
            dateStr = myQuery.value(0).toString();
            d = QDate::fromString(dateStr, "yyyy-MM-dd");

            idVar = myQuery.value(1).toInt();
            variable = MapIdMeteoVar.at(idVar);

            value = myQuery.value(2).toFloat();

            meteoPoint->setMeteoPointValueD(Crit3DDate(d.day(), d.month(), d.year()), variable, value);
        }
    }

    return true;
}


bool Crit3DMeteoPointsDbHandler::loadHourlyData(Crit3DDate dateStart, Crit3DDate dateEnd, Crit3DMeteoPoint *meteoPoint)
{
    QString dateStr;
    meteoVariable variable;
    QDateTime d;
    int idVar;
    float value;

    int numberOfDays = difference(dateStart, dateEnd)+1;
    int myHourlyFraction = 1;
    QString startDate = QString::fromStdString(dateStart.toStdString());
    QString endDate = QString::fromStdString(dateEnd.toStdString());

    QSqlQuery qry(_db);

    meteoPoint->initializeObsDataH(myHourlyFraction, numberOfDays, dateStart);

    QString tableName = QString::fromStdString(meteoPoint->id) + "_H";

    QString statement = QString( "SELECT * FROM `%1` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')")
                                 .arg(tableName).arg(startDate).arg(endDate);
    if( !qry.exec(statement) )
    {
        qDebug() << qry.lastError();
        return false;
    }
    else
    {
        while (qry.next())
        {
            dateStr = qry.value(0).toString();
            d = QDateTime::fromString(dateStr,"yyyy-MM-dd HH:mm:ss");

            idVar = qry.value(1).toInt();
            variable = MapIdMeteoVar.at(idVar);

            value = qry.value(2).toFloat();

            meteoPoint->setMeteoPointValueH(Crit3DDate(d.date().day(), d.date().month(), d.date().year()),
                                                   d.time().hour(), d.time().minute(), variable, value);
        }
    }
    return true;
}


std::vector<float> Crit3DMeteoPointsDbHandler::loadDailyVar(QString *myError, meteoVariable variable, Crit3DDate dateStart, Crit3DDate dateEnd, QDate* firstDateDB, Crit3DMeteoPoint *meteoPoint)
{
    QString dateStr;
    QDate d, previousDate;
    float value;
    std::vector<float> dailyVarList;
    bool firstRow = true;

    int idVar = getIdfromMeteoVar(variable);
    QString startDate = QString::fromStdString(dateStart.toStdString());
    QString endDate = QString::fromStdString(dateEnd.toStdString());

    QSqlQuery myQuery(_db);

    QString tableName = QString::fromStdString(meteoPoint->id) + "_D";

    QString statement = QString( "SELECT * FROM `%1` WHERE `%2` = %3 AND date_time >= DATE('%4') AND date_time < DATE('%5', '+1 day')")
                                .arg(tableName).arg(FIELD_METEO_VARIABLE).arg(idVar).arg(startDate).arg(endDate);

    if( !myQuery.exec(statement) )
    {
        *myError = myQuery.lastError().text();
        return dailyVarList;
    }
    else
    {
        while (myQuery.next())
        {
            if (firstRow)
            {
                dateStr = myQuery.value(0).toString();
                *firstDateDB = QDate::fromString(dateStr, "yyyy-MM-dd");
                previousDate = *firstDateDB;

                value = myQuery.value(2).toFloat();

                dailyVarList.push_back(value);

                firstRow = false;
            }
            else
            {
                dateStr = myQuery.value(0).toString();
                d = QDate::fromString(dateStr, "yyyy-MM-dd");

                int missingDate = previousDate.daysTo(d);
                for (int i =1; i<missingDate; i++)
                {
                    dailyVarList.push_back(NODATA);
                }

                value = myQuery.value(2).toFloat();

                dailyVarList.push_back(value);
                previousDate = d;

            }

        }
    }

    return dailyVarList;
}

std::vector<float> Crit3DMeteoPointsDbHandler::loadHourlyVar(QString *myError, meteoVariable variable, Crit3DDate dateStart, Crit3DDate dateEnd, QDateTime* firstDateDB, Crit3DMeteoPoint *meteoPoint)
{
    QString dateStr;
    QDateTime d, previousDate;
    float value;
    std::vector<float> hourlyVarList;
    bool firstRow = true;

    int idVar = getIdfromMeteoVar(variable);
    QString startDate = QString::fromStdString(dateStart.toStdString());
    QString endDate = QString::fromStdString(dateEnd.toStdString());

    QSqlQuery qry(_db);

    QString tableName = QString::fromStdString(meteoPoint->id) + "_H";

    QString statement = QString( "SELECT * FROM `%1` WHERE `%2` = %3 AND date_time >= DATE('%4') AND date_time <= DATETIME('%5 00:00:00', '+1 day')")
                                 .arg(tableName).arg(FIELD_METEO_VARIABLE).arg(idVar).arg(startDate).arg(endDate);
    if( !qry.exec(statement) )
    {
        *myError = qry.lastError().text();
        return hourlyVarList;
    }
    else
    {
        while (qry.next())
        {
            if (firstRow)
            {
                dateStr = qry.value(0).toString();
                *firstDateDB = QDateTime::fromString(dateStr,"yyyy-MM-dd HH:mm:ss");
                previousDate = *firstDateDB;

                value = qry.value(2).toFloat();

                hourlyVarList.push_back(value);
                firstRow = false;
            }
            else
            {
                dateStr = qry.value(0).toString();
                d = QDateTime::fromString(dateStr,"yyyy-MM-dd HH:mm:ss");

                int missingDate = previousDate.daysTo(d);
                for (int i =1; i<missingDate; i++)
                {
                    hourlyVarList.push_back(NODATA);
                }
                value = qry.value(2).toFloat();

                hourlyVarList.push_back(value);
                previousDate = d;
            }
        }
    }
    return hourlyVarList;
}

QSqlDatabase Crit3DMeteoPointsDbHandler::getDb() const
{
    return _db;
}

void Crit3DMeteoPointsDbHandler::setDb(const QSqlDatabase &db)
{
    _db = db;
}

bool Crit3DMeteoPointsDbHandler::readPointProxyValues(Crit3DMeteoPoint* myPoint, Crit3DInterpolationSettings* interpolationSettings)
{
    if (myPoint == nullptr) return false;

    QSqlQuery qry(_db);
    QString proxyField;
    QString proxyTable;
    QString statement;
    int nrProxy;
    Crit3DProxy* myProxy;

    nrProxy = interpolationSettings->getProxyNr();
    myPoint->proxyValues.resize(nrProxy);

    for (int i=0; i < nrProxy; i++)
    {
        myPoint->proxyValues.at(i) = NODATA;

        // read only for active proxies
        if (interpolationSettings->getSelectedCombination().getValue(i))
        {
            myProxy = interpolationSettings->getProxy(i);
            proxyField = QString::fromStdString(myProxy->getProxyField());
            proxyTable = QString::fromStdString(myProxy->getProxyTable());
            if (proxyField != "" && proxyTable != "")
            {
                statement = QString("SELECT `%1` FROM `%2` WHERE id_point = '%3'").arg(proxyField).arg(proxyTable).arg(QString::fromStdString((*myPoint).id));
                if(qry.exec(statement))
                {
                    qry.last();
                    if (qry.value(proxyField) != "")
                        myPoint->proxyValues.at(i) = qry.value(proxyField).toFloat();
                }
            }

            if (myPoint->proxyValues.at(i) == NODATA)
            {
                gis::Crit3DRasterGrid* proxyGrid = myProxy->getGrid();
                if (proxyGrid == nullptr || ! proxyGrid->isLoaded)
                    return false;
                else
                {
                    float myValue = gis::getValueFromXY(*proxyGrid, myPoint->point.utm.x, myPoint->point.utm.y);
                    if (myValue != proxyGrid->header->flag)
                        myPoint->proxyValues.at(i) = myValue;
                }
            }
        }
    }

    return true;
}

QList<Crit3DMeteoPoint> Crit3DMeteoPointsDbHandler::getPropertiesFromDb(const gis::Crit3DGisSettings& gisSettings, QString *errorString)
{
    QList<Crit3DMeteoPoint> meteoPointsList;
    Crit3DMeteoPoint meteoPoint;
    QSqlQuery qry(_db);
    bool isPositionOk;

    qry.prepare( "SELECT id_point, name, dataset, latitude, longitude, utm_x, utm_y, altitude, state, region, province, municipality, is_active, is_utc, orog_code from point_properties ORDER BY id_point" );

    if( !qry.exec() )
    {
        *errorString = qry.lastError().text();
    }
    else
    {
        while (qry.next())
        {
            //initialize
            meteoPoint = *(new Crit3DMeteoPoint());

            meteoPoint.id = qry.value("id_point").toString().toStdString();
            meteoPoint.name = qry.value("name").toString().toStdString();
            meteoPoint.dataset = qry.value("dataset").toString().toStdString();

            if (qry.value("latitude") != "")
                meteoPoint.latitude = qry.value("latitude").toDouble();
            if (qry.value("longitude") != "")
                meteoPoint.longitude = qry.value("longitude").toDouble();
            if (qry.value("utm_x") != "")
                meteoPoint.point.utm.x = qry.value("utm_x").toDouble();
            if (qry.value("utm_y") != "")
                meteoPoint.point.utm.y = qry.value("utm_y").toDouble();
            if (qry.value("altitude") != "")
                meteoPoint.point.z = qry.value("altitude").toDouble();

            // check position
            isPositionOk = false;
            if ((meteoPoint.latitude != NODATA || meteoPoint.longitude != NODATA)
                && (meteoPoint.point.utm.x != NODATA && meteoPoint.point.utm.y != NODATA))
            {
                isPositionOk = true;
            }
            else if ((meteoPoint.latitude == NODATA || meteoPoint.longitude == NODATA)
                && (meteoPoint.point.utm.x != NODATA && meteoPoint.point.utm.y != NODATA))
            {
                gis::getLatLonFromUtm(gisSettings, meteoPoint.point.utm.x, meteoPoint.point.utm.y,
                                        &(meteoPoint.latitude), &(meteoPoint.longitude));
                isPositionOk = true;
            }
            else if ((meteoPoint.latitude != NODATA || meteoPoint.longitude != NODATA)
                && (meteoPoint.point.utm.x == NODATA && meteoPoint.point.utm.y == NODATA))
            {
                gis::latLonToUtmForceZone(gisSettings.utmZone, meteoPoint.latitude, meteoPoint.longitude,
                                          &(meteoPoint.point.utm.x), &(meteoPoint.point.utm.y));
                isPositionOk = true;
            }

            if (isPositionOk)
            {
                meteoPoint.state = qry.value("state").toString().toStdString();
                meteoPoint.region = qry.value("region").toString().toStdString();
                meteoPoint.province = qry.value("province").toString().toStdString();
                meteoPoint.municipality = qry.value("municipality").toString().toStdString();
                meteoPoint.active = qry.value("is_active").toBool();
                meteoPoint.isUTC = qry.value("is_utc").toBool();
                meteoPoint.lapseRateCode = lapseRateCodeType((qry.value("orog_code").toInt()));
                meteoPointsList << meteoPoint;
            }
        }

    }

    return meteoPointsList;
}

bool Crit3DMeteoPointsDbHandler::writePointProperties(Crit3DMeteoPoint *myPoint)
{

    QSqlQuery qry(_db);

    qry.prepare( "INSERT INTO point_properties (id_point, name, dataset, latitude, longitude, latInt, lonInt, utm_x, utm_y, altitude, state, region, province, municipality)"
                                      " VALUES (:id_point, :name, :dataset, :latitude, :longitude, :latInt, :lonInt, :utm_x, :utm_y, :altitude, :state, :region, :province, :municipality)" );

    qry.bindValue(":id_point", QString::fromStdString(myPoint->id));
    qry.bindValue(":name", QString::fromStdString(myPoint->name));
    qry.bindValue(":dataset", QString::fromStdString(myPoint->dataset));
    qry.bindValue(":latitude", myPoint->latitude);
    qry.bindValue(":longitude", myPoint->longitude);
    qry.bindValue(":latInt", myPoint->latInt);
    qry.bindValue(":lonInt", myPoint->lonInt);
    qry.bindValue(":utm_x", myPoint->point.utm.x);
    qry.bindValue(":utm_y", myPoint->point.utm.y);
    qry.bindValue(":altitude", myPoint->point.z);
    qry.bindValue(":state", QString::fromStdString(myPoint->state));
    qry.bindValue(":region", QString::fromStdString(myPoint->region));
    qry.bindValue(":province", QString::fromStdString(myPoint->province));
    qry.bindValue(":municipality", QString::fromStdString(myPoint->municipality));

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
        return false;
    }
    else
        return true;

}
