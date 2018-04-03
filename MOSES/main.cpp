#include <iostream>
#include <QCoreApplication>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <cerrno>
#include <QDir>

#include "furtherMathFunctions.h"
#include "queryArcgis.h"
#include "project.h"
#include "modelcore.h"
#include "dbTools.h"
#include "dbToolsMOSES.h"
#include "commonConstants.h"


int main(int argc, char *argv[])
{
    QCoreApplication myApp(argc, argv);

    MOSESProject myProject;
    QString settingsFileName;
    int indexOfForecast;
    Crit3DDate dateOfForecast, firstDateAllSeason;
    QString dateOfForecastStr, IrrPreviousDateStr, firstDateAllSeasonStr, mySQL;
    QSqlQuery myQuery;
    float RAW, deficit, rootDepth, prec, maxTranspiration;
    float forecastIrrigation, previousIrrigation, previousAllIrrigation;
    float irriRatio;
    double percentile;

    if (argc > 1)
        settingsFileName = argv[1];
    else
    {
        //settingsFileName = "//PRAGA-SMR/MOSES/SWB/DA_MO/seasonalPredictions/seasonalIrriForecast_MO.ini";
        //settingsFileName = "//PRAGA-SMR/MOSES/SWB/DA_ITC4/ShortTermIrriForecast_C4.ini";

        myProject.logError("USAGE: MOSES_WB settings_filename.ini");
        return ERROR_SETTINGS_MISSING;
    }

    if (settingsFileName.left(1) == ".")
        settingsFileName = myApp.applicationDirPath() + settingsFileName;

    int myError = myProject.initializeProject(settingsFileName);
    if (myError != CRIT3D_OK)
    {
        myProject.logError();
        return myError;
    }

    //Unit map on ARCGIS
    if (myProject.isArcGisService)
    {
        queryArcgis *myQueryArcgis;
        QString myURL = myProject.serverArcgis + myProject.cropLayer;
        myQueryArcgis = new queryArcgis(myURL, &(myProject), NULL);
        myQueryArcgis->executeQuery();
        if (myQueryArcgis->isError)
        {
            myProject.logError("Error in Arcis server query: " + myQueryArcgis->error);
            return ERROR_READ_UNITS;
        }
    }

    //Unit map on LOCAL
    else if (! myProject.loadUnits())
    {
        myProject.logError();
        return ERROR_READ_UNITS;
    }

    myProject.logInfo("\nQuery result: " + QString::number(myProject.nrUnits) + " distinct units.\n");

    // Initialize irrigation output file
    if ((myProject.criteria.isSeasonalForecast) || (myProject.criteria.isShortTermForecast))
    {
        if (!QDir(myProject.irrigationPath).exists())
             QDir().mkdir(myProject.irrigationPath);

        // Add date to filename (only for ShortTermForecast)
        if (myProject.criteria.isShortTermForecast)
        {
            myProject.irrigationFileName = myProject.irrigationFileName.left(myProject.irrigationFileName.length()-4);
            myProject.irrigationFileName += "_" + QDate::currentDate().toString("yyyyMMdd") + ".csv";
        }

        myProject.outputFile.open(myProject.irrigationFileName.toStdString().c_str(), std::ios::out | std::ios::trunc);
        if ( myProject.outputFile.fail())
        {
            myProject.logError("open failure: " + QString(strerror(errno)) + '\n');
            return ERROR_DBOUTPUT;
        }
        else
        {
           myProject.logInfo("Output file: " + myProject.irrigationFileName);
        }

        if (myProject.criteria.isSeasonalForecast)
            myProject.outputFile << "ID_CASE,CROP,SOIL,METEO,p5,p25,p50,p75,p95\n";

        else if(myProject.criteria.isShortTermForecast)
            myProject.outputFile << "dateForecast,ID_CASE,CROP,SOIL,METEO,"
                                    "readilyAvailableWater,soilWaterDeficit,rootDepth,"
                                    "forecast7daysPrec,forecast7daysMaxTransp,forecast7daysIRR,"
                                    "previous14daysIRR,previousAllSeasonIRR\n";
    }

    bool isErrorMeteo = false;
    bool isErrorSoil = false;
    bool isErrorCrop = false;
    long nrUnitsComputed = 0;

    try
    {
        for (int i = 0; i < myProject.nrUnits; i++)
        {
            //CROP
            myProject.unit[i].idCrop = getMOSESIdCrop(&(myProject.criteria.dbParameters), myProject.unit[i].idCropMoses, &(myProject.projectError)).toUpper();
            if (myProject.unit[i].idCrop == "")
            {
                myProject.logInfo("Unit " + myProject.unit[i].idCase + " " + myProject.unit[i].idCropMoses + " ***** missing CROP *****");
                isErrorCrop = true;
            }
            else
            {
                //IRRI_RATIO
                irriRatio = getMOSESIrriRatio(&(myProject.criteria.dbParameters), myProject.unit[i].idCropMoses, &(myProject.projectError));
                if (irriRatio == NODATA)
                    myProject.logInfo("Unit " + myProject.unit[i].idCase + " " + myProject.unit[i].idCropMoses + " ***** missing IRRIGATION RATIO *****");
                else
                {
                    //SOIL
                    myProject.unit[i].idSoil = getIdSoilString(&(myProject.criteria.dbSoil), myProject.unit[i].idSoilNumber, &(myProject.projectError));
                    if (myProject.unit[i].idSoil == "")
                    {
                        myProject.logInfo("Unit " + myProject.unit[i].idCase + " Soil nr." + QString::number(myProject.unit[i].idSoilNumber) + " ***** missing SOIL *****");
                        isErrorSoil = true;
                    }
                    else
                    {
                        // LOG
                        myProject.logInfo("Unit " + myProject.unit[i].idCase +" "+ myProject.unit[i].idCrop +" "+ myProject.unit[i].idSoil +" "+ myProject.unit[i].idMeteo);

                        // SEASONAL
                        if (myProject.criteria.isSeasonalForecast)
                        {
                            if (irriRatio == 0)
                            {
                                myProject.outputFile << myProject.unit[i].idCase.toStdString() << "," << myProject.unit[i].idCrop.toStdString() << ",";
                                myProject.outputFile << myProject.unit[i].idSoil.toStdString() << "," << myProject.unit[i].idMeteo.toStdString();
                                myProject.outputFile << ",0,0,0,0,0\n";
                            }
                            else
                            {
                                if (! runModel(&(myProject.criteria), &(myProject.projectError), &(myProject.unit[i])))
                                {
                                    myProject.logError();
                                    // TODO Improve
                                    isErrorMeteo = true;
                                }
                                else
                                {
                                    myProject.outputFile << myProject.unit[i].idCase.toStdString() << "," << myProject.unit[i].idCrop.toStdString() << ",";
                                    myProject.outputFile << myProject.unit[i].idSoil.toStdString() << "," << myProject.unit[i].idMeteo.toStdString();
                                    // percentiles
                                    percentile = sorting::percentile(myProject.criteria.seasonalForecasts, &(myProject.criteria.nrSeasonalForecasts), 5, true);
                                    myProject.outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(myProject.criteria.seasonalForecasts, &(myProject.criteria.nrSeasonalForecasts), 25, false);
                                    myProject.outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(myProject.criteria.seasonalForecasts, &(myProject.criteria.nrSeasonalForecasts), 50, false);
                                    myProject.outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(myProject.criteria.seasonalForecasts, &(myProject.criteria.nrSeasonalForecasts), 75, false);
                                    myProject.outputFile << "," << percentile * irriRatio;
                                    percentile = sorting::percentile(myProject.criteria.seasonalForecasts, &(myProject.criteria.nrSeasonalForecasts), 95, false);
                                    myProject.outputFile << "," << percentile * irriRatio << "\n";
                                    myProject.outputFile.flush();

                                    nrUnitsComputed++;
                                }
                            }
                        }
                        else
                        {
                            if (! runModel(&(myProject.criteria), &(myProject.projectError), &(myProject.unit[i])))
                            {
                                myProject.logError();
                                isErrorMeteo = true;
                            }

                            // SHORT TERM FORECAST
                            else if(myProject.criteria.isShortTermForecast)
                            {
                                indexOfForecast = myProject.criteria.meteoPoint.nrObsDataDaysD - myProject.criteria.daysOfForecast - 1;
                                dateOfForecast = myProject.criteria.meteoPoint.obsDataD[indexOfForecast].date;
                                dateOfForecastStr = QString::fromStdString(dateOfForecast.toStdString());
                                IrrPreviousDateStr = QString::fromStdString(dateOfForecast.addDays(-13).toStdString());

                                // first date for annual irrigation
                                if (myProject.criteria.firstSeasonMonth <= dateOfForecast.month)
                                    firstDateAllSeason = Crit3DDate(1, myProject.criteria.firstSeasonMonth, dateOfForecast.year);
                                else
                                    firstDateAllSeason = Crit3DDate(1, myProject.criteria.firstSeasonMonth, dateOfForecast.year - 1);

                                firstDateAllSeasonStr = QString::fromStdString(firstDateAllSeason.toStdString());

                                prec = NODATA;
                                maxTranspiration = NODATA;
                                forecastIrrigation = NODATA;
                                previousIrrigation = NODATA;
                                previousAllIrrigation = NODATA;
                                RAW = NODATA;

                                mySQL = "SELECT SUM(PREC) AS prec,"
                                        " SUM(TRANSP_MAX) AS maxTransp, SUM(IRRIGATION) AS irr"
                                        " FROM '" + myProject.unit[i].idCase + "'"
                                        " WHERE DATE > '" + dateOfForecastStr + "'";

                                myQuery = myProject.criteria.dbOutput.exec(mySQL);

                                if (myQuery.lastError().type() != QSqlError::NoError)
                                    myProject.logError("SELECT SUM(PREC)\n" + myQuery.lastError().text());
                                else
                                {
                                    myQuery.last();
                                    prec = myQuery.value("prec").toFloat();
                                    maxTranspiration = myQuery.value("maxTransp").toFloat();
                                    forecastIrrigation = myQuery.value("irr").toFloat();
                                }

                                mySQL = "SELECT RAW, DEFICIT, ROOTDEPTH FROM '" + myProject.unit[i].idCase + "'"
                                        " WHERE DATE = '" + dateOfForecastStr + "'";
                                myQuery = myProject.criteria.dbOutput.exec(mySQL);

                                if (myQuery.lastError().type() != QSqlError::NoError)
                                    myProject.logError("SELECT RAW, DEFICIT, ROOTDEPTH\n" + myQuery.lastError().text());
                                else
                                {
                                    myQuery.last();
                                    RAW = myQuery.value("RAW").toFloat();
                                    deficit = myQuery.value("DEFICIT").toFloat();
                                    rootDepth = myQuery.value("ROOTDEPTH").toFloat();
                                }

                                mySQL = "SELECT SUM(IRRIGATION) AS prevIrr FROM '" + myProject.unit[i].idCase + "'"
                                        " WHERE DATE <= '" + dateOfForecastStr + "'"
                                        " AND DATE >= '" + IrrPreviousDateStr + "'";
                                myQuery = myProject.criteria.dbOutput.exec(mySQL);

                                if (myQuery.lastError().type() != QSqlError::NoError)
                                    myProject.logError("SELECT SUM(IRRIGATION) (previous 14 days) \n" + myQuery.lastError().text());
                                else
                                {
                                    myQuery.last();
                                    previousIrrigation = myQuery.value("prevIrr").toFloat();
                                }

                                mySQL = "SELECT SUM(IRRIGATION) AS prevAllIrr FROM '" + myProject.unit[i].idCase + "'"
                                        " WHERE DATE <= '" + dateOfForecastStr + "'"
                                        " AND DATE >= '" + firstDateAllSeasonStr + "'";
                                myQuery = myProject.criteria.dbOutput.exec(mySQL);

                                if (myQuery.lastError().type() != QSqlError::NoError)
                                    myProject.logError("SELECT SUM(IRRIGATION) (all season) \n" + myQuery.lastError().text());
                                else
                                {
                                    myQuery.last();
                                    previousAllIrrigation = myQuery.value("prevAllIrr").toFloat();
                                }

                                myProject.outputFile << dateOfForecast.toStdString();
                                myProject.outputFile << "," << myProject.unit[i].idCase.toStdString();
                                myProject.outputFile << "," << myProject.unit[i].idCrop.toStdString();
                                myProject.outputFile << "," << myProject.unit[i].idSoil.toStdString();
                                myProject.outputFile << "," << myProject.unit[i].idMeteo.toStdString();
                                myProject.outputFile << "," << QString::number(RAW,'f',1).toStdString();
                                myProject.outputFile << "," << QString::number(deficit,'f',1).toStdString();
                                myProject.outputFile << "," << QString::number(rootDepth,'f',2).toStdString();
                                myProject.outputFile << "," << QString::number(prec,'f',1).toStdString();
                                myProject.outputFile << "," << QString::number(maxTranspiration,'f',1).toStdString();
                                myProject.outputFile << "," << forecastIrrigation * irriRatio;
                                myProject.outputFile << "," << previousIrrigation * irriRatio;
                                myProject.outputFile << "," << previousAllIrrigation * irriRatio << "\n";
                                myProject.outputFile.flush();

                                nrUnitsComputed++;
                            }
                        }
                    }
                }
            }
        }

        myProject.outputFile.close();

    } catch (std::exception &e)
    {
        qFatal("Error %s ", e.what());

    } catch (...)
    {
        qFatal("Error <unknown>");
        return ERROR_UNKNOWN;
    }

    myProject.logInfo("END");

    // error check
    if (nrUnitsComputed == 0)
    {
        if (isErrorMeteo)
            return ERROR_METEO;
        else if (isErrorSoil)
            return ERROR_SOIL_MISSING;
        else if (isErrorCrop)
            return ERROR_CROP_MISSING;
        else
            return ERROR_UNKNOWN;
    }
    else if (nrUnitsComputed < myProject.nrUnits)
    {
        if (isErrorMeteo)
            return WARNING_METEO;
        else if (isErrorSoil)
            return WARNING_SOIL;
        else if (isErrorCrop)
            return WARNING_CROP;
    }

    else return CRIT3D_OK;
}
