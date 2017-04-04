#include "dbArkimet.h"
#include "commonConstants.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QString>
#include <QStringBuilder>

//#include <QtNetwork>

DbArkimet::DbArkimet(QString dbName) : DbMeteoPoints(dbName)
{

}


DbArkimet::~DbArkimet()
{

}


QList<VariablesList> DbArkimet::getHourlyVarFields(QList<int> id)
{

    QList<VariablesList> variableList;

    QString idlist = QString("(%1").arg(id[0]);

    for (int i = 1; i < id.size(); i++)
    {
        idlist = idlist % QString(",%1").arg(id[i]);
    }
    idlist = idlist % QString(")");

    QString statement = QString("SELECT * FROM variable_properties WHERE id_arkimet IN %1").arg(idlist);

    QSqlQuery qry(statement, _db);

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        qDebug( "Selected!" );

        while (qry.next())
        {
            variableList.append(VariablesList(qry.value(0).toInt(), qry.value(1).toInt(), qry.value(2).toString(), qry.value(5).toInt() ));
        }


    }
    return variableList;


}

QString DbArkimet::getVarName(int id)
{

    QString varName = NULL;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT variable FROM variable_properties WHERE id_arkimet = :id" );
    qry.bindValue(":id", id);

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "Selected!" );

        if (qry.next())
            varName = qry.value(0).toString();

        else
            qDebug( "Error: dataset not found" );

    }

    return varName;
}


int DbArkimet::getId(QString VarName)
{

    int id = 0;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT id_arkimet FROM variable_properties WHERE variable = :VarName" );
    qry.bindValue(":VarName", VarName);

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "Selected!" );

        if (qry.next())
            id = qry.value(0).toInt();

        else
            qDebug( "Error: dataset not found" );

    }

    return id;
}

QList<int> DbArkimet::getDailyVar()
{
    QList<int> dailyVarList;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT id_arkimet FROM variable_properties WHERE aggregation_time = 86400" );

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "Selected!" );

        while (qry.next())
        {
            int id = qry.value(0).toInt();
            dailyVarList << id;

        }

    }

    return dailyVarList;

}

QList<int> DbArkimet::getHourlyVar()
{
    QList<int> hourlyVarList;
    QSqlQuery qry(_db);

    qry.prepare( "SELECT id_arkimet FROM variable_properties WHERE aggregation_time < 86400" );

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "Selected!" );

        while (qry.next())
        {
            int id = qry.value(0).toInt();
            hourlyVarList << id;

        }

    }

    return hourlyVarList;

}

bool DbArkimet::fillPointProperties(Crit3DMeteoPoint *pointProp)
{


    bool success = false;
    QSqlQuery qry(_db);

    qry.prepare( "INSERT INTO point_properties (id_point, name, dataset, latitude, longitude, latInt, lonInt, utm_x, utm_y, altitude, state, region, province, municipality)"
                                      " VALUES (:id_point, :name, :dataset, :latitude, :longitude, :latInt, :lonInt, :utm_x, :utm_y, :altitude, :state, :region, :province, :municipality)" );


    qry.bindValue(":id_point", QString::fromStdString(pointProp->id));
    qry.bindValue(":name", QString::fromStdString(pointProp->name));
    qry.bindValue(":dataset", QString::fromStdString(pointProp->dataset));
    qry.bindValue(":latitude", pointProp->latitude);
    qry.bindValue(":longitude", pointProp->longitude);
    qry.bindValue(":latInt", pointProp->latInt);
    qry.bindValue(":lonInt", pointProp->lonInt);
    qry.bindValue(":utm_x", pointProp->point.utm.x);
    qry.bindValue(":utm_y", pointProp->point.utm.y);
    qry.bindValue(":altitude", pointProp->point.z);
    qry.bindValue(":state", QString::fromStdString(pointProp->state));
    qry.bindValue(":region", QString::fromStdString(pointProp->region));
    qry.bindValue(":province", QString::fromStdString(pointProp->province));
    qry.bindValue(":municipality", QString::fromStdString(pointProp->municipality));


    if( !qry.exec() )
    {
        success = false;
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "successfully inserted" );
        success = true;
    }

    return success;

}

void DbArkimet::initStationsDailyTables(Crit3DDate dataStartInput, Crit3DDate dataEndInput, QList<int> stations)
{
    QString startDate = QString::fromStdString(dataStartInput.toStdString());
    QString endDate = QString::fromStdString(dataEndInput.toStdString());

    foreach(int station, stations)
    {
        QString statement = QString("CREATE TABLE IF NOT EXISTS `%1_D` (date_time TEXT, id_variable INTEGER, value REAL, PRIMARY KEY(date_time,id_variable))").arg(station);
        //qDebug() << "initStationsDailyTables - Create " << statement;

        QSqlQuery qry(statement, _db);
        qry.exec();

        statement = QString("DELETE FROM `%1_D` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')").arg(station).arg(startDate).arg(endDate);
        //qDebug() << "initStationsDailyTables - Delete " << statement;

        qry = QSqlQuery(statement, _db);
        qry.exec();
    }

}

void DbArkimet::initStationsHourlyTables(Crit3DTime dataStartInput, Crit3DTime dataEndInput, QList<int> stations)
{
    QString startDate = QString::fromStdString(dataStartInput.toStdString());
    QString endDate = QString::fromStdString(dataEndInput.toStdString());

    foreach(int station, stations)
    {
        QString statement = QString("CREATE TABLE IF NOT EXISTS `%1_H` (date_time TEXT, id_variable INTEGER, value REAL, PRIMARY KEY(date_time,id_variable))").arg(station);
        //qDebug() << "initStationsHourlyTables - Create " << statement;

        QSqlQuery qry(statement, _db);
        qry.exec();

        statement = QString("DELETE FROM `%1_H` WHERE date_time >= DATE('%2') AND date_time < DATE('%3', '+1 day')").arg(station).arg(startDate).arg(endDate);
        //qDebug() << "initStationsHourlyTables - Delete " << statement;

        qry = QSqlQuery(statement, _db);
        qry.exec();
    }

}

void DbArkimet::createTmpTable()
{

    QSqlQuery qry(_db);
    qry.prepare("CREATE TABLE IF NOT EXISTS TmpHourlyData (date_time TEXT, id_point INTEGER, id_variable INTEGER, variable_name TEXT, value REAL, frequency INTEGER, PRIMARY KEY(date_time,id_point,variable_name))");
    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "createTmpTable - Create" );
    }

    qry.prepare("DELETE FROM TmpHourlyData");

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "createTmpTable - Delete all records" );
    }


}

void DbArkimet::deleteTmpTable()
{

    QSqlQuery qry(_db);

    qry.prepare( "DROP TABLE TmpHourlyData" );

    if( !qry.exec() )
    {
        qDebug() << qry.lastError();
    }
    else
    {
        qDebug( "Drop table" );

    }

}

void DbArkimet::insertDailyValue(QString station, QString date, int varType, double varValue, QString flag)
{
    if (flag.left(1) == "1" || flag.left(1) == "054") {
        // invalid data
        varValue = NODATA;
    }

    if (varValue !=NODATA)
    {
        QString statement = QString("INSERT INTO `%1_D` VALUES('%2', '%3', '%4')").arg(station).arg(date).arg(varType).arg(varValue);

        QSqlQuery qry = QSqlQuery(statement, _db);
        qry.exec();
    }

}


void DbArkimet::saveHourlyData()
{

    QStringList stations;

    QString statement = QString("SELECT DISTINCT id_point FROM TmpHourlyData");
    QSqlQuery qry = QSqlQuery(statement, _db);
    qry.exec();

    while (qry.next())
    {
        stations.append(qry.value(0).toString());
    }

    statement = QString("INSERT INTO `%1_H` ");
    statement = statement % "SELECT date_time, id_variable, value FROM TmpHourlyData WHERE id_point = %1 AND frequency = 3600";

    foreach (QString station, stations) {
        qry = QSqlQuery(statement.arg(station), _db);
        qry.exec();
    }

    statement = QString("DELETE FROM TmpHourlyData WHERE frequency = 3600");
    qry = QSqlQuery(statement, _db);
    qry.exec();

    statement = QString("INSERT INTO `%1_H` ");
    statement = statement % "SELECT date_time, id_variable, value FROM TmpHourlyData WHERE ";
    statement = statement % "id_point = %1 AND variable_name IN ('HOURLY_WIND_INT', 'HOURLY_WIND_DIR') AND strftime('%M', date_time) = '00'";

    foreach (QString station, stations) {
        qry = QSqlQuery(statement.arg(station), _db);
        qry.exec();
    }

    statement = QString("INSERT INTO `%1_H` ");
    statement = statement % "SELECT DATETIME(date_time, '+30 minutes'), id_variable, value FROM TmpHourlyData WHERE ";
    statement = statement % "id_point = %1 AND variable_name = 'HOURLY_RAD' AND strftime('%M', date_time) = '30'";

    foreach (QString station, stations) {
        qry = QSqlQuery(statement.arg(station), _db);
        qry.exec();
    }

    statement = QString("DELETE FROM TmpHourlyData WHERE variable_name IN ('HOURLY_RAD', 'HOURLY_WIND_INT', 'HOURLY_WIND_DIR')");
    qry = QSqlQuery(statement, _db);
    qry.exec();

    statement = QString("INSERT INTO `%1_H` ");
    statement = statement % "SELECT aggregate_date, id_variable, aggregate_value FROM (";
    statement = statement % "SELECT aggregate_date, id_variable, SUM(value) AS aggregate_value, SUM(frequency) AS aggregate_frequency FROM (";
    statement = statement % "SELECT datetime(date_time, '+' || (SELECT CASE WHEN strftime('%M', date_time) = '00' THEN 0 ELSE 60 - strftime('%M', date_time) END) || ' minutes') AS aggregate_date, ";
    statement = statement % "id_variable, value, frequency FROM TmpHourlyData WHERE id_point = %1 AND variable_name like '%PREC%'";
    statement = statement % ") group by aggregate_date, id_variable) WHERE aggregate_frequency = 3600";
    statement = statement % " UNION ALL ";
    statement = statement % "SELECT aggregate_date, id_variable, aggregate_value FROM (";
    statement = statement % "SELECT aggregate_date, id_variable, AVG(value) AS aggregate_value FROM (";
    statement = statement % "SELECT datetime(date_time, '+' || (SELECT CASE WHEN strftime('%M', date_time) = '00' THEN 0 ELSE 60 - strftime('%M', date_time) END) || ' minutes') AS aggregate_date, ";
    statement = statement % "id_variable, value FROM TmpHourlyData WHERE id_point = %1 AND variable_name like '%AVG%'";
    statement = statement % ") group by aggregate_date, id_variable)";

    QString delStationStatement = QString("DELETE FROM TmpHourlyData WHERE id_point = :id_point");

    foreach (QString station, stations) {


        qry = QSqlQuery(statement.arg(station), _db);
        qry.exec();

        qry = QSqlQuery(delStationStatement, _db);
        qry.bindValue(":id_point", station);
        qry.exec();

    }

}

void DbArkimet::insertOrUpdate(QString date, QString id_point, int id_variable, QString variable_name, double value, int frequency, QString flag)
{
    if (flag.left(1) == "1" || flag.left(1) == "054") {
        // invalid data
        value = NODATA;
    }

    if (value !=NODATA)
    {
        QString statement = QString("REPLACE INTO TmpHourlyData SELECT '%1', %2, %3, '%4', %5, %6 WHERE %6 > (SELECT COALESCE((");
        statement = statement % "SELECT frequency FROM TmpHourlyData WHERE date_time = '%1' AND id_point = %2 AND variable_name = '%4'), 0))";
        statement = statement.arg(date).arg(id_point).arg(id_variable).arg(variable_name).arg(value).arg(frequency);

        QSqlQuery qry = QSqlQuery(statement, _db);
        qry.exec();
    }

}

int DbArkimet::arkIdmap(int arkId)
{

    switch(arkId)
    {
        case 158:
            return 51;
        case 159:
            return 52;
        case 139:
            return 53;
        case 164:
            return 54;
        case 166:
            return 55;
        case 165:
            return 56;
        case 78:
            return 101;
        case 160:
            return 102;
        case 140:
            return 103;
        case 409:
            return 104;
        case 69:
            return 105;
        case 431:
            return 106;
        case 232:
            return 151;
        case 233:
            return 152;
        case 231:
            return 153;
        case 250:
            return 154;
        case 241:
            return 155;
        case 242:
            return 156;
        case 240:
            return 157;
        case 706:
            return 158;
        case 227:
            return 159;
        case 230:
            return 160;
        default:
            qDebug() << "value of arkId unknown";
    }
    return NODATA;
}