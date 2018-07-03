#include "climate.h"
#include "crit3dDate.h"
#include "utilities.h"
#include "quality.h"
#include "statistics.h"

#define THOMTHRESHOLD 24 // mettere nei settings quando ci saranno Environment.ThomThreshold
#define MINPERCENTAGE 80 // mettere nei settings quando ci saranno Environment.minPercentage

Crit3DDate firstDateDailyVar; //temporaneo

bool elaborationPointsCycle(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, meteoVariable variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate)
{


    bool pointOrGrid = 0; // meteoPoint

//    Dim row As Long, col As Long, i As Long
//    Dim result As Single
//    Dim currentParameter1 As Single


    QDate startDate(firstYear, firstDate.month(), firstDate.day());
    QDate endDate(lastYear, lastDate.month(), lastDate.day());

    if (nYears > 0)
    {
        endDate.setDate(lastYear + nYears, endDate.month(), endDate.day());
    }
    else if (nYears < 0)
    {
        startDate.setDate(firstYear + nYears, startDate.month(), startDate.day());
    }

    if (param1IsClimate)
    {
//        Dim myPeriodType As Byte
//        Dim myClimateIndex As Integer

//        myPeriodType = Climate.parserElaborationOnlyPeriodType(parameter1ClimateElab)
//        myClimateIndex = Climate.getClimateIndexFromDate(currentDay, myPeriodType)
     }


//                For i = 0 To UBound(meteoPoint)

//                    If meteoPoint(i).Point.active And meteoPoint(i).Point.selected Then

//                        If param1IsClimate Then
//                            If Climate.ClimateReadPoint(PragaClimate.Point(i).TableName, _
//                                                        param1ClimateField, _
//                                                        myPeriodType, _
//                                                        myClimateIndex, _
//                                                        PragaClimate.Point(i)) Then

//                                currentParameter1 = passaggioDati.GetClimateData(myPeriodType, PragaClimate.Point(i), myClimateIndex)
//                            Else
//                                currentParameter1 = Definitions.NO_DATA
//                            End If
//                        Else
//                            currentParameter1 = param1
//                        End If

//                        If elab1 = Definitions.ELABORATION_PHENO Then currentPheno.setPhenoPoint i

//                        If ElaborationOnPoint(meteoPoint(i).Point, pointOrGrid, variable, _
//                            elab1, currentParameter1, elab2, Parameter2, _
//                            startDate, endDate, nYears, firstYear, lastYear, _
//                            nYearsMin, _
//                            isAnomaly, True) Then ElaborationPointsCycle = True

//                    End If
//                Next i
    return true;

}




bool elaborationPointsCycleGrid(std::string *myError, Crit3DMeteoGridDbHandler* meteoGridDbHandler, meteoVariable variable, int firstYear, int lastYear, QDate firstDate, QDate lastDate, int nYears,
    QString elab1, bool param1IsClimate, QString param1ClimateField, float param1, QString elab2, float param2, bool isAnomaly,
    int nYearsMin, int firstYearClimate, int lastYearClimate)
{

    bool pointOrGrid = 1; // grid
    float currentParameter1;

    std::string id;


    QDate startDate(firstYear, firstDate.month(), firstDate.day());
    QDate endDate(lastYear, lastDate.month(), lastDate.day());

    if (nYears > 0)
    {
        endDate.setDate(lastYear + nYears, endDate.month(), endDate.day());
    }
    else if (nYears < 0)
    {
        startDate.setDate(firstYear + nYears, startDate.month(), startDate.day());
    }


    if (param1IsClimate)
    {
//        Dim myPeriodType As Byte
//        Dim myClimateIndex As Integer

//        myPeriodType = Climate.parserElaborationOnlyPeriodType(parameter1ClimateElab)
//        myClimateIndex = Climate.getClimateIndexFromDate(currentDay, myPeriodType)
     }


     for (int row = 0; row < meteoGridDbHandler->gridStructure().header().nrRows; row++)
     {
         for (int col = 0; col < meteoGridDbHandler->gridStructure().header().nrCols; col++)
         {

            if (meteoGridDbHandler->meteoGrid()->getMeteoPointActiveId(row, col, &id))
            {

                if (param1IsClimate)
                {
//                    if (Climate.ClimateReadPoint(PragaClimate.Point(row, col).TableName, _
//                        param1ClimateField, myPeriodType, myClimateIndex,PragaClimate.Point(row, col)) )
//                    {

//                      currentParameter1 = passaggioDati.GetClimateData(myPeriodType, PragaClimate.Point(row, col), myClimateIndex)
//                    }
//                    else
//                    {
//                        currentParameter1 = NODATA;
//                    }
                }
                else
                {
                    currentParameter1 = param1;
                }

//                            if  ( !elaborationOnPoint(&(meteoGridDbHandler->meteoGrid()->meteoPoint(row,col)), pointOrGrid, variable,
//                                elab1, currentParameter1, elab2, param2, startDate, endDate, nYears, firstYear, lastYear,
//                                nYearsMin, isAnomaly, true))
//                            {
//                                // LC se qualche problema su una cella, deve interrompere e ritornare false oppure no?
                                  // FT credo di no, ma lascio decidere a Gabri
                                  // GA deve tornare false solo se nemmeno una cella ha dato valido (v. VB)
//                                return false;
//                            }

            }

        }
    }

    return true;

}




bool elaborationOnPoint(std::string *myError, Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler,
    Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, meteoVariable variable, QString elab1, float param1,
    QString elab2, float param2, QDate startDate, QDate endDate, int nYears, int firstYear, int lastYear,
    int nYearsMin, bool isAnomaly, bool loadData)
{


//    Dim perc_data As Single
    bool dataLoaded;
    float result;

    if (!isAnomaly || meteoPoint->anomaly != NODATA)
    {

            dataLoaded = false;

            if (loadData)
            {
//                dataLoaded = Elaboration.preElaboration(pointOrGrid, variable, elab1, myPoint, _
//                   startDate, endDate, perc_data)
            }
            else
            {
                // ricordarsi di usare imbuto se loadData è falso
                dataLoaded = true;
            }

            if (dataLoaded)
            {
                // check
                Crit3DDate startD(startDate.day(), startDate.month(), firstYear);
                Crit3DDate endD(endDate.day(), endDate.month(), firstYear);

                if (nYears < 0)
                {
                    startD.year = firstYear + nYears;
                }
                else if (nYears > 0)
                {
                    endD.year = firstYear + nYears;
                }

                if (difference (startD, endD) < 0)
                {
                    *myError = "Wrong dates!";
                    return false;
                }
                if (elab1 == "")
                {
                    *myError = "Missing elaboration";
                    return false;
                }
//                result = computeStatistic(myVar, firstYear, lastYear, startD, endD, nYears, elab1, param1, elab2, param2, .z)

                if (isAnomaly)
                {
//                    ElaborationOnPoint = AnomalyOnPoint(myPoint, result)
                }
                else
                {
                      meteoPoint->elaboration = result;
                      if (meteoPoint->elaboration != NODATA)
                      {
                          return true;
                      }
                      else
                      {
                          return false;
                      }
                }
            }
            else if (isAnomaly)
            {
                meteoPoint->anomaly = NODATA;
                meteoPoint->anomalyPercentage = NODATA;
            }

    }

    return false;

}


std::vector<float> loadDailyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler,
   Crit3DMeteoGridDbHandler meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid,
   meteoVariable variable, QDate first, QDate last, bool saveValue)
{
    std::vector<float> dailyValues;
    QDate firstDateDB;
    //grid
    if (pointOrGrid)
    {
        if (meteoGridDbHandler.gridStructure().isFixedFields())
        {
            dailyValues = meteoGridDbHandler.loadGridDailyVarFixedFields(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }
        else
        {
            dailyValues = meteoGridDbHandler.loadGridDailyVar(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }
        if (saveValue)
        {
            // LC la serie deve iniziare con la prima data utile oppure con la data richiesta e quindi NODATA se non c'è il valore corrispondente a tale dato=
            //int numberOfDays = first.daysTo(last) + 1;
            // FT non mi è chiaro
            // GA in VB usiamo una variabile (firstDateDailyVar) che contiene la prima data della serie giornaliera, quindi senza riempire di NODATA il pezzo vuoto
            int numberOfDays = firstDateDB.daysTo(last) + 1;
            int row, col;
            int initialize = 1;
            meteoGridDbHandler.meteoGrid()->findMeteoPointFromId(&row, &col, meteoPoint->id);
            for (int i = 0; i < dailyValues.size(); i++)
            {
                meteoGridDbHandler.meteoGrid()->fillMeteoPointDailyValue(row, col, numberOfDays, initialize, Crit3DDate(firstDateDB.day(), firstDateDB.month(), firstDateDB.year()), variable, dailyValues[i]) ;
                initialize = 0;
                firstDateDB.addDays(1);
            }
        }
    }
    // meteoPoint
    else
    {
        dailyValues = meteoPointsDbHandler->getDailyVar(myError, variable, getCrit3DDate(first), getCrit3DDate(last), &firstDateDB, meteoPoint );
        if (saveValue)
        {
            for (unsigned int i = 0; i < dailyValues.size(); i++)
            {
                meteoPoint->setMeteoPointValueD(Crit3DDate(firstDateDB.day(), firstDateDB.month(), firstDateDB.year()), variable, dailyValues[i]);
                firstDateDB.addDays(1);
            }
        }
    }
    return dailyValues;

}

std::vector<float> loadHourlyVarSeries(std::string *myError, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler,
           Crit3DMeteoGridDbHandler meteoGridDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid,
           meteoVariable variable, QDateTime first, QDateTime last, bool saveValue)
{
    std::vector<float> hourlyValues;
    QDateTime firstDateDB;
    //grid
    if (pointOrGrid)
    {
        if (meteoGridDbHandler.gridStructure().isFixedFields())
        {
            hourlyValues = meteoGridDbHandler.loadGridHourlyVarFixedFields(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }
        else
        {
            hourlyValues = meteoGridDbHandler.loadGridHourlyVar(myError, QString::fromStdString(meteoPoint->id), variable, first, last, &firstDateDB);
        }
        if (saveValue)
        {
            // LC la serie deve iniziare con la prima data utile oppure con la data richeista e quindi NODATA se non c'è il valore corrispondente a tale dato=
            //int numberOfDays = first.date().daysTo(last.date());
            // FT non mi è chiaro
            // GA vedi commento per serie giornaliera
            int numberOfDays = firstDateDB.date().daysTo(last.date());
            int row, col;
            int initialize = 1;
            meteoGridDbHandler.meteoGrid()->findMeteoPointFromId(&row, &col, meteoPoint->id);
            for (unsigned int i = 0; i < hourlyValues.size(); i++)
            {
                meteoGridDbHandler.meteoGrid()->fillMeteoPointHourlyValue(row, col, numberOfDays, initialize,  Crit3DDate(firstDateDB.date().day(), firstDateDB.date().month(), firstDateDB.date().year()), firstDateDB.time().hour(), firstDateDB.time().minute(), variable, hourlyValues[i]) ;
                initialize = 0;
                firstDateDB.addSecs(3600);
            }
        }
    }
    // meteoPoint
    else
    {
        hourlyValues = meteoPointsDbHandler->getHourlyVar(myError, variable, getCrit3DDate(first.date()), getCrit3DDate(last.date()), &firstDateDB, meteoPoint );
        if (saveValue)
        {
            for (unsigned int i = 0; i < hourlyValues.size(); i++)
            {
                meteoPoint->setMeteoPointValueH(Crit3DDate(firstDateDB.date().day(), firstDateDB.date().month(), firstDateDB.date().year()), firstDateDB.time().hour(), firstDateDB.time().minute(), variable, hourlyValues[i]);
                firstDateDB.addSecs(3600);
            }
        }
    }
    return hourlyValues;

}


// compute daily Thom using Tmax e RHmin
float thomDayTime(float tempMax, float relHumMinAir)
{

    Crit3DQuality qualityCheck;
    quality::type qualityT = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, tempMax);
    quality::type qualityRelHumMinAir = qualityCheck.syntacticQualityControlSingleVal(dailyAirRelHumidityMin, relHumMinAir);


    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    if ( qualityT == quality::accepted && qualityRelHumMinAir == quality::accepted )
    {
            return computeThomIndex(tempMax, relHumMinAir);
    }
    else
        return NODATA;

}

// compute daily Thom using Tmin e RHmax
float thomNightTime(float tempMin, float relHumMaxAir)
{

    Crit3DQuality qualityCheck;
    quality::type qualityT = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, tempMin);
    quality::type qualityRelHumMaxAir = qualityCheck.syntacticQualityControlSingleVal(dailyAirRelHumidityMax, relHumMaxAir);

    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    if ( qualityT == quality::accepted && qualityRelHumMaxAir == quality::accepted )
    {
            return computeThomIndex(tempMin, relHumMaxAir);
    }
    else
        return NODATA;

}

// compuote hourly thom
float thomH(float tempAvg, float relHumAvgAir)
{

    Crit3DQuality qualityCheck;
    quality::type qualityT = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureAvg, tempAvg);
    quality::type qualityRelHumAvgAir = qualityCheck.syntacticQualityControlSingleVal(dailyAirRelHumidityAvg, relHumAvgAir);

    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    if ( qualityT == quality::accepted && qualityRelHumAvgAir == quality::accepted )
    {
            return computeThomIndex(tempAvg, relHumAvgAir);
    }
    else
        return NODATA;
}

// compute # hours thom >  threshold per day
int thomDailyNHoursAbove(std::vector<float> hourlyValues, float* relHumAvgAir)
{

    int nData = 0;
    int thomDailyNHoursAbove = NODATA;
    for (int hour = 0; hour < 24; hour++)
    {
        float thom = thomH(hourlyValues.at(hour), relHumAvgAir[hour]);
        if (thom != NODATA)
        {
            nData = nData + 1;
            if (thomDailyNHoursAbove == NODATA)
                thomDailyNHoursAbove = 0;
            if (thom > THOMTHRESHOLD)
                thomDailyNHoursAbove = thomDailyNHoursAbove + 1;
        }
    }
    if ( (nData / 24 * 100) < MINPERCENTAGE)
        thomDailyNHoursAbove = NODATA;

    return thomDailyNHoursAbove;


}

// compute daily max thom value
float thomDailyMax(std::vector<float> hourlyValues, float* relHumAvgAir)
{
    int nData = 0;
    int thomDailyMax = NODATA;
    for (int hour = 0; hour < 24; hour++)
    {
        float thom = thomH(hourlyValues.at(hour), relHumAvgAir[hour]);
        if (thom != NODATA)
        {
            nData = nData + 1;
            if (thom > thomDailyMax)
                thomDailyMax = thom;
        }
    }
    if ( (nData / 24 * 100) < MINPERCENTAGE)
        thomDailyMax = NODATA;

    return thomDailyMax;
}

// compute daily avg thom value
float thomDailyMean(std::vector<float> hourlyValues, float* relHumAvgAir)
{

    int nData = 0;
    std::vector<float> thomValues;
    float thomDailyMean;

    for (int hour = 0; hour < 24; hour++)
    {
        float thom = thomH(hourlyValues.at(hour), relHumAvgAir[hour]);
        if (thom != NODATA)
        {
            thomValues.push_back(thom);
            nData = nData + 1;
        }
    }
    if ( (nData / 24 * 100) < MINPERCENTAGE)
        thomDailyMean = NODATA;
    else
        thomDailyMean = statistics::mean(thomValues, nData);


    return thomDailyMean;

}

float dailyLeafWetnessComputation(std::vector<float> hourlyValues)
{

    int nData = 0;
    float dailyLeafWetnessRes = 0;

    for (int hour = 0; hour < 24; hour++)
    {
        if (hourlyValues.at(hour) == 0 || hourlyValues.at(hour) == 1)
        {
                dailyLeafWetnessRes = dailyLeafWetnessRes + hourlyValues.at(hour);
                nData = nData + 1;
        }
    }
    if ( (nData / 24 * 100) < MINPERCENTAGE)
        dailyLeafWetnessRes = NODATA;

    return dailyLeafWetnessRes;

}

float computeDailyBIC(float prec, float etp)
{

    Crit3DQuality qualityCheck;

    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    quality::type qualityPrec = qualityCheck.syntacticQualityControlSingleVal(dailyPrecipitation, prec);
    quality::type qualityETP = qualityCheck.syntacticQualityControlSingleVal(dailyReferenceEvapotranspiration, etp);
    if (qualityPrec == quality::accepted && qualityETP == quality::accepted)
    {
            return (prec - etp);
    }
    else
        return NODATA;

}

float dailyThermalRange(float Tmin, float Tmax)
{

    Crit3DQuality qualityCheck;

    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    quality::type qualityTmin = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, Tmin);
    quality::type qualityTmax = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, Tmax);
    if (qualityTmin  == quality::accepted && qualityTmax == quality::accepted)
        return (Tmax - Tmin);
    else
        return NODATA;

}

float dailyAverageT(float Tmin, float Tmax)
{

        Crit3DQuality qualityCheck;

        // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
        quality::type qualityTmin = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, Tmin);
        quality::type qualityTmax = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, Tmax);
        if (qualityTmin  == quality::accepted && qualityTmax == quality::accepted)
            return ( (Tmin + Tmax) / 2) ;
        else
            return NODATA;

}

float dailyEtpHargreaves(float Tmin, float Tmax, Crit3DDate date, double latitude)
{

    Crit3DQuality qualityCheck;

    // TODO nella versione vb ammessi anche i qualitySuspectData, questo tipo per ora non è stato implementato
    quality::type qualityTmin = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMin, Tmin);
    quality::type qualityTmax = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureMax, Tmax);
    int dayOfYear = getDoyFromDate(date);
    if (qualityTmin  == quality::accepted && qualityTmax == quality::accepted)
        return ET0_Hargreaves(TRANSMISSIVITY_SAMANI_COEFF_DEFAULT, latitude, dayOfYear, Tmax, Tmin);
    else
        return NODATA;

}

float dewPoint(float relHumAir, float tempAir)
{

    if (relHumAir == NODATA || relHumAir == 0 || tempAir == NODATA)
        return NODATA;

    relHumAir = minValue(100, relHumAir);

    float saturatedVaporPres = exp((16.78 * tempAir - 116.9) / (tempAir + 237.3));
    float actualVaporPres = relHumAir / 100 * saturatedVaporPres;
    return (log(actualVaporPres) * 237.3 + 116.9) / (16.78 - log(actualVaporPres));

}

void extractValidValuesWithThreshold(std::vector<float> myValues, std::vector<float>* myValidValues, float myThreshold)
{

    for (unsigned int i = 0; i < myValues.size(); i++)
    {
        if (myValues[i] != NODATA)
        {
            if (myValues[i] > myThreshold)
            {
                myValidValues->push_back(myValues[i]);
            }
        }
    }

}

void extractValidValuesCC(std::vector<float> myValues, std::vector<float>* myValidValues)
{

    for (unsigned int i = 0; i < myValues.size(); i++)
    {
        if (myValues[i] != NODATA)
        {
            myValidValues->push_back(myValues[i]);
        }
    }

}

bool elaborateDailyAggregatedVar(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float> dailyValues, std::vector<float> hourlyValues, std::vector<float>* aggregatedValues, float* percValue)
{

    frequencyType aggregationFrequency = getAggregationFrequency(myVar);

    if (aggregationFrequency == hourly)
    {
            return elaborateDailyAggregatedVarFromHourly(myVar, meteoPoint, hourlyValues, aggregatedValues);
    }
    else if (aggregationFrequency == daily)
    {
            return elaborateDailyAggregatedVarFromDaily(myVar, meteoPoint, dailyValues, aggregatedValues, percValue);
    }
    else
        return false;

}


frequencyType getAggregationFrequency(meteoVariable myVar)
{

    //if (elab == ELABORATION_THOM_DAILYHOURSABOVE || elab == ELABORATION_THOM_DAILYMEAN || elab == ELABORATION_THOM_DAILYMAX || elab == DAILY_LEAFWETNESS)
    if (myVar == dailyThomHoursAbove || myVar == dailyThomAvg || myVar == dailyThomMax || myVar == dailyLeafWetness)
    {
        return hourly;
    }
    else if (myVar != NODATA)
    {
        return daily;
    }
    else
    {
        return noFrequency;
    }

}

bool elaborateDailyAggregatedVarFromDaily(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float> dailyValues, std::vector<float>* aggregatedValues, float* percValue)
{

    float res;
    int nrValidi = 0;
    Crit3DDate date = meteoPoint.obsDataD[0].date;
    Crit3DQuality qualityCheck;

    for (unsigned int index = 0; index < dailyValues.size(); index++)
    {
        switch(myVar)
        {
        case dailyThomDaytime:
                res = thomDayTime(dailyValues.at(index), meteoPoint.obsDataD[index].rhMin);
            break;
        case dailyThomNighttime:
                res = thomNightTime(dailyValues.at(index), meteoPoint.obsDataD[index].rhMax);
                break;
        case dailyBIC:
                res = computeDailyBIC(dailyValues.at(index), meteoPoint.obsDataD[index].et0);
                break;
        case dailyAirTemperatureRange:
                res = dailyThermalRange(dailyValues.at(index), meteoPoint.obsDataD[index].tMax);
                break;
        case dailyAirTemperatureAvg:
                {
                    quality::type qualityTavg = qualityCheck.syntacticQualityControlSingleVal(dailyAirTemperatureAvg, meteoPoint.obsDataD[index].tAvg);
                    if (qualityTavg == quality::accepted)
                    {
                        res = meteoPoint.obsDataD[index].tAvg;
                    }
                    else
                    {
                        res = dailyAverageT(dailyValues.at(index), meteoPoint.obsDataD[index].tMax);
                    }
                    break;
                }
        case dailyReferenceEvapotranspiration:
        {
            quality::type qualityEtp = qualityCheck.syntacticQualityControlSingleVal(dailyReferenceEvapotranspiration, meteoPoint.obsDataD[index].et0);
            if (qualityEtp == quality::accepted)
            {
                res = meteoPoint.obsDataD[index].et0;
            }
            else
            {
                res = dailyEtpHargreaves(dailyValues.at(index), meteoPoint.obsDataD[index].tMax, date, meteoPoint.latitude);
            }
            break;
        }
        case dailyAirDewTemperatureAvg:
                res = dewPoint(dailyValues.at(index), meteoPoint.obsDataD[index].tAvg); // RHavg, Tavg
                break;
        case dailyAirDewTemperatureMin:
                res = dewPoint(dailyValues.at(index), meteoPoint.obsDataD[index].tMin); // RHmax, Tmin
                break;
        case dailyAirDewTemperatureMax:
                res = dewPoint(dailyValues.at(index), meteoPoint.obsDataD[index].tMax); // RHmin, Tmax
                break;
        default:
                res = NODATA;
                break;
        }

        if (res != NODATA)
        {
            nrValidi = nrValidi + 1;
            firstDateDailyVar = date;
        }
        aggregatedValues->push_back(res);
        date = date.addDays(1);

    }

    *percValue = nrValidi/aggregatedValues->size();
    if (nrValidi > 0)
        return true;
    else
        return false;

}

bool elaborateDailyAggregatedVarFromHourly(meteoVariable myVar, Crit3DMeteoPoint meteoPoint, std::vector<float> hourlyValues, std::vector<float>* aggregatedValues)
{

    float res;
    int nrValidi = 0;
    Crit3DDate date = meteoPoint.obsDataH[0].date;

    for (unsigned int index = 0; index < hourlyValues.size(); index++)
    {
        std::vector<float> oneDayHourlyValues (hourlyValues.begin()+index, hourlyValues.begin()+index+23);
        switch(myVar)
        {
            case dailyThomHoursAbove:
                res = thomDailyNHoursAbove(oneDayHourlyValues, meteoPoint.obsDataH[index].rhAir);
                break;
            case dailyThomMax:
                res = thomDailyMax(oneDayHourlyValues, meteoPoint.obsDataH[index].rhAir);
                break;
            case dailyThomAvg:
                res = thomDailyMean(oneDayHourlyValues, meteoPoint.obsDataH[index].rhAir);
                break;
            case dailyLeafWetness:
                res = dailyLeafWetnessComputation(oneDayHourlyValues);
                break;
            default:
                res = NODATA;
                break;
        }

        if (res != NODATA)
        {
            nrValidi = nrValidi + 1;
            firstDateDailyVar = date; // LC perchè in vb nella elaborateDailyAggregatedVarFromHourly viene posto firstDateDailyVar = currentHourlySeries(1).date
                                      // senza alcun controllo sul NODATA come invece avviene nella elaborateDailyAggregatedVarFromDaily
                                      // FT non mi è chiaro
                                      // GA si' il codice per l'orario è più vecchio e non c'era controllo. possiamo aggiungerlo.
        }
        aggregatedValues->push_back(res);
        date = date.addDays(1);
    }
    if (nrValidi > 0)
        return true;
    else
        return false;

}



bool preElaboration(Crit3DMeteoGridDbHandler* meteoGridDbHandler, Crit3DMeteoPointsDbHandler* meteoPointsDbHandler, Crit3DMeteoPoint* meteoPoint, bool pointOrGrid, meteoVariable variable, QString elab1,
    QDate startDate, QDate endDate, float* percValue)
{

    /*
    //// era in statistica
    todo: spostare in preElaboration (quindi eliminare myVar)
    If myElab <> Definitions.ELAB_GIORNI_CONSECUTIVI_SOPRA_SOGLIA And _
        myElab <> Definitions.ELAB_GIORNI_CONSECUTIVI_SOTTO_SOGLIA And _
        myElab <> Definitions.ELAB_TREND Then

        If myVar = Definitions.DAILY_PREC And myElab = Definitions.ELAB_PERCENTILE And Not isCumulated Then
            Elaboration.ExtractValidValuesWithThreshold myValues, myValidValues, Environment.RainfallThreshold
        Else
            Elaboration.ExtractValidValuesCC myValues, myValidValues
        End If
    End If
    //////////
    */
/*
// LC c'è confusione tra elaboration e variable fa lo switch su variable ma ci sono elaboration e variabili. Ho messo tutto in un unico enum visto che comunque non sono davvero suddivisi
// GA sono considerate variabili ai fini del calcolo tutte quelle grandezze giornaliere che possono essere elaborate con le stesse procedure delle variabili primarie
// LC inoltre ci sono elaborazioni che non trovo nell'interfaccia grafica, sono state rimosse? (es. ELABORATION_HUGLIN come si seleziona? tutte le "elaboerazioni specifiche")
// GA queste sono vere elaborazioni, non riconducibili a variabili giornaliere. quelle che dici le trovi selezionando Tmin o Tmax nelle variabili, perché sono calcolate a partire da quelle
// FT Si caricano con dei moduli appositi, ma qui lo sa meglio Gabri
    case variable


        case Definitions.DAILY_LEAFWETNESS
            If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_LEAFWETNESS, myPoint, startDate, endDate) > 0 Then
                passaggioDati.InizializzaMbutoOrario startDate, endDate
                passaggioDati.MbutoInversoOrario Definitions.HOURLY_LEAFWETNESS
                preElaboration = Elaboration.elaborateDailyAggregatedVar(variable, myPoint, percValue)
            End If
// LC? perchè viene fatta 2 volte l'aggregazione giornaliera della hourly leafwetness
// FT Ah, questo proprio non lo so :-)
            If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_LEAFWETNESS, myPoint, startDate, endDate) > 0 Then
                passaggioDati.MbutoHourly Definitions.HOURLY_LEAFWETNESS, currentHourlySeries, myPoint.z
                preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_LEAFWETNESS, myPoint, percValue)
            End If

        case Definitions.ELABORATION_THOM_DAYTIME
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmin, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmin
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.ELABORATION_THOM_DAYTIME, myPoint, percValue)
                End If
            End If

        case Definitions.ELABORATION_THOM_NIGHTTIME
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmax, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmax
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.ELABORATION_THOM_NIGHTTIME, myPoint, percValue)
                End If
            End If

        case Definitions.ELABORATION_THOM_DAILYMEAN, Definitions.ELABORATION_THOM_DAILYMAX, Definitions.ELABORATION_THOM_DAILYHOURSABOVE
            If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_TAVG, myPoint, startDate, endDate) > 0 Then
                passaggioDati.InizializzaMbutoOrario startDate, endDate
                passaggioDati.MbutoInversoOrario Definitions.HOURLY_TAVG
                If passaggioDati.LoadGenericHourlySeries(pointOrGrid, Definitions.HOURLY_RHAVG, myPoint, startDate, endDate) > 0 Then
                    passaggioDati.MbutoInversoOrario Definitions.HOURLY_RHAVG
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(variable, myPoint, percValue)
                End If
            End If

        case Definitions.DAILY_BIC
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_ETP, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            ElseIf Environment.AutomaticETP Then
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    preElaboration = False
                    passaggioDati.InizializzaMbuto startDate, endDate
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                        preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_ETP, myPoint, percValue)
                    End If
                End If
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_ETP
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_PREC, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_PREC
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_BIC, myPoint, percValue)
                End If
            End If

        case Definitions.DAILY_TEMPERATURE_RANGE
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TEMPERATURE_RANGE, myPoint, percValue)
                End If
            End If

        case Definitions.DAILY_TDMAX
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmin, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmin
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TDMAX, myPoint, percValue)
                End If
            End If

        case Definitions.DAILY_TDMIN
            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                    preElaboration = True
            End If
            If preElaboration Then
                preElaboration = False
                passaggioDati.InizializzaMbuto startDate, endDate
                passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_RHmax, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_RHmax
                    preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TDMIN, myPoint, percValue)
                End If
            End If

        case Definitions.DAILY_TAVG
            percValue = passaggioDati.loadDailyVarSeries(pointOrGrid, DAILY_TAVG, myPoint, startDate, endDate)
            If percValue > 0 Then
                    preElaboration = True

            ElseIf Environment.AutomaticTmed Then
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    preElaboration = False
                    passaggioDati.InizializzaMbuto startDate, endDate
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                        preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TAVG, myPoint, percValue)
                    End If
                End If
            End If

        case Definitions.DAILY_ETP
            percValue = passaggioDati.loadDailyVarSeries(pointOrGrid, DAILY_ETP, myPoint, startDate, endDate)
            If percValue > 0 The
                    preElaboration = True

            ElseIf Environment.AutomaticETP Then
                If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                        preElaboration = True
                End If
                If preElaboration Then
                    preElaboration = False
                    passaggioDati.InizializzaMbuto startDate, endDate
                    passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                        preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_ETP, myPoint, percValue)
                    End If
                End If
            End If

        case Else

            Select case elab1

                case Definitions.ELABORATION_HUGLIN
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        preElaboration = False
                        passaggioDati.InizializzaMbuto startDate, endDate
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                        If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                        End If
                        If preElaboration Then
                            preElaboration = False
                            passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TAVG, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                            ElseIf Environment.AutomaticTmed Then
                                preElaboration = Elaboration.elaborateDailyAggregatedVar(Definitions.DAILY_TAVG, myPoint, percValue)
                            End If
                        End If
                    End If

                case Definitions.ELABORATION_WINKLER, Definitions.ELABORATION_CORRECTED_SUM, Definitions.ELABORATION_FREGONI

                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        preElaboration = False
                        passaggioDati.InizializzaMbuto startDate, endDate
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                        If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                        End If
                        If preElaboration Then passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                    End If

                case Definitions.ELABORATION_PHENO
                    If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMIN, myPoint, startDate, endDate) > 0 Then
                            preElaboration = True
                    End If
                    If preElaboration Then
                        preElaboration = False
                        passaggioDati.InizializzaMbuto startDate, endDate
                        passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMIN
                        If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_TMAX, myPoint, startDate, endDate) > 0 Then
                                preElaboration = True
                        End If
                        If preElaboration Then
                            preElaboration = False
                            passaggioDati.MbutoInversoGiornaliero Definitions.DAILY_TMAX
                            If passaggioDati.loadDailyVarSeries(pointOrGrid, Definitions.DAILY_PREC, myPoint, startDate, endDate) > 0 Then
                                    preElaboration = True
                            End If
                        End If
                    End If

                case Else

                    percValue = passaggioDati.loadDailyVarSeries(pointOrGrid, variable, myPoint, startDate, endDate)

                    If percValue > 0 Then
                            preElaboration = True
                    End If

            End Select

    End Select
*/

    return true;
}


