#include "meteoGrid.h"
#include "commonConstants.h"
#include <iostream> //debug


Crit3DMeteoGridStructure::Crit3DMeteoGridStructure()
{

}

std::string Crit3DMeteoGridStructure::name() const
{
    return _name;
}

void Crit3DMeteoGridStructure::setName(const std::string &name)
{
    _name = name;
}

gis::Crit3DGridHeader Crit3DMeteoGridStructure::header() const
{
    return _header;
}

void Crit3DMeteoGridStructure::setHeader(const gis::Crit3DGridHeader &header)
{
    _header = header;
}

int Crit3DMeteoGridStructure::dataType() const
{
    return _dataType;
}

void Crit3DMeteoGridStructure::setDataType(int dataType)
{
    _dataType = dataType;
}

bool Crit3DMeteoGridStructure::isRegular() const
{
    return _isRegular;
}

void Crit3DMeteoGridStructure::setIsRegular(bool isRegular)
{
    _isRegular = isRegular;
}

bool Crit3DMeteoGridStructure::isTIN() const
{
    return _isTIN;
}

void Crit3DMeteoGridStructure::setIsTIN(bool isTIN)
{
    _isTIN = isTIN;
}

bool Crit3DMeteoGridStructure::isUTM() const
{
    return _isUTM;
}

void Crit3DMeteoGridStructure::setIsUTM(bool isUTM)
{
    _isUTM = isUTM;
}

bool Crit3DMeteoGridStructure::isLoaded() const
{
    return _isLoaded;
}

void Crit3DMeteoGridStructure::setIsLoaded(bool isLoaded)
{
    _isLoaded = isLoaded;
}

bool Crit3DMeteoGridStructure::isFixedFields() const
{
    return _isFixedFields;
}

void Crit3DMeteoGridStructure::setIsFixedFields(bool isFixedFields)
{
    _isFixedFields = isFixedFields;
}

bool Crit3DMeteoGridStructure::isHourlyDataAvailable() const
{
    return _isHourlyDataAvailable;
}

void Crit3DMeteoGridStructure::setIsHourlyDataAvailable(bool isHourlyDataAvailable)
{
    _isHourlyDataAvailable = isHourlyDataAvailable;
}

bool Crit3DMeteoGridStructure::isDailyDataAvailable() const
{
    return _isDailyDataAvailable;
}

void Crit3DMeteoGridStructure::setIsDailyDataAvailable(bool isDailyDataAvailable)
{
    _isDailyDataAvailable = isDailyDataAvailable;
}

Crit3DMeteoGrid::Crit3DMeteoGrid()
{

}

Crit3DMeteoGridStructure Crit3DMeteoGrid::gridStructure() const
{
    return _gridStructure;
}



bool Crit3DMeteoGrid::loadRasterGrid()
{

    dataMeteoGrid.header->cellSize = NODATA;
    dataMeteoGrid.header->llCorner->x = _gridStructure.header().llCorner->longitude;
    dataMeteoGrid.header->llCorner->y = _gridStructure.header().llCorner->latitude;

    dataMeteoGrid.header->nrCols = _gridStructure.header().nrCols;
    dataMeteoGrid.header->nrRows = _gridStructure.header().nrRows;
    dataMeteoGrid.header->flag = NODATA;

    dataMeteoGrid.initializeGrid(NODATA);

    return true;

}


void Crit3DMeteoGrid::setGridStructure(const Crit3DMeteoGridStructure &gridStructure)
{
    _gridStructure = gridStructure;
}

std::vector<std::vector<Crit3DMeteoPoint> > Crit3DMeteoGrid::meteoPoints() const
{
    return _meteoPoints;
}

void Crit3DMeteoGrid::setMeteoPoints(const std::vector<std::vector<Crit3DMeteoPoint> > &meteoPoints)
{
    _meteoPoints = meteoPoints;
}

bool Crit3DMeteoGrid::isAggregationDefined() const
{
    return _isAggregationDefined;
}

void Crit3DMeteoGrid::setIsAggregationDefined(bool isAggregationDefined)
{
    _isAggregationDefined = isAggregationDefined;
}

bool Crit3DMeteoGrid::isDateTypeSpecified() const
{
    return _isDateTypeSpecified;
}

void Crit3DMeteoGrid::setIsDateTypeSpecified(bool isDateTypeSpecified)
{
    _isDateTypeSpecified = isDateTypeSpecified;
}

Crit3DDate Crit3DMeteoGrid::firstDate() const
{
    return _firstDate;
}

void Crit3DMeteoGrid::setFirstDate(const Crit3DDate &firstDate)
{
    _firstDate = firstDate;
}

Crit3DDate Crit3DMeteoGrid::lastDate() const
{
    return _lastDate;
}

void Crit3DMeteoGrid::setLastDate(const Crit3DDate &lastDate)
{
    _lastDate = lastDate;
}

int Crit3DMeteoGrid::nrVarsArray() const
{
    return _nrVarsArray;
}

void Crit3DMeteoGrid::setNrVarsArray(int nrVarsArray)
{
    _nrVarsArray = nrVarsArray;
}







