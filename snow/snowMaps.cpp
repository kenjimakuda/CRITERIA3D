/*!
    CRITERIA3D

    \copyright 2010-2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    You should have received a copy of the GNU General Public License
    along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by A.R.P.A. Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/

#include <cmath>
#include "commonConstants.h"
#include "snowMaps.h"
#include "snowPoint.h"


Crit3DSnowMaps::Crit3DSnowMaps()
{
    this->initializeMaps();
    _isLoaded = false;
}

Crit3DSnowMaps::Crit3DSnowMaps(const gis::Crit3DRasterGrid& dtmGrid)
{
    this->initializeMaps();

    _snowFallMap->initializeGrid(dtmGrid);
    _snowMeltMap->initializeGrid(dtmGrid);
    _snowWaterEquivalentMap->initializeGrid(dtmGrid);
    _iceContentMap->initializeGrid(dtmGrid);
    _lWContentMap->initializeGrid(dtmGrid);
    _internalEnergyMap->initializeGrid(dtmGrid);
    _surfaceInternalEnergyMap->initializeGrid(dtmGrid);
    _snowSurfaceTempMap->initializeGrid(dtmGrid);
    _ageOfSnowMap->initializeGrid(dtmGrid);

    _initSoilPackTemp = 3.4f;
    _initSnowSurfaceTemp = 5.0f;
}


/*!
 * \brief Initialize all maps
 */
void Crit3DSnowMaps::initializeMaps()
{
    _snowFallMap = new gis::Crit3DRasterGrid;
    _snowMeltMap = new gis::Crit3DRasterGrid;
    _snowWaterEquivalentMap = new gis::Crit3DRasterGrid;
    _iceContentMap = new gis::Crit3DRasterGrid;
    _lWContentMap = new gis::Crit3DRasterGrid;
    _internalEnergyMap = new gis::Crit3DRasterGrid;
    _surfaceInternalEnergyMap = new gis::Crit3DRasterGrid;
    _snowSurfaceTempMap = new gis::Crit3DRasterGrid;
    _ageOfSnowMap = new gis::Crit3DRasterGrid;
}


void Crit3DSnowMaps::updateMap(Crit3DSnowPoint* snowPoint, int row, int col)
{
    _snowFallMap->value[row][col] = snowPoint->getSnowFall();
    _snowMeltMap->value[row][col] = snowPoint->getSnowMelt();
    _snowWaterEquivalentMap->value[row][col] = snowPoint->getSnowWaterEquivalent();
    _iceContentMap->value[row][col] = snowPoint->getIceContent();
    _lWContentMap->value[row][col] = snowPoint->getLWContent();
    _internalEnergyMap->value[row][col] = snowPoint->getInternalEnergy();
    _surfaceInternalEnergyMap->value[row][col] = snowPoint->getSurfaceInternalEnergy();
    _snowSurfaceTempMap->value[row][col] = snowPoint->getSnowSurfaceTemp();
    _ageOfSnowMap->value[row][col] = snowPoint->getAgeOfSnow();
}


void Crit3DSnowMaps::resetSnowModel(gis::Crit3DRasterGrid* sweGrid, Crit3DSnowPoint* snowPoint)
{
    float initSWE;              /*!<   [mm] */
    int surfaceBulkDensity;     /*!<   [kg/m^3] */

    for (long row = 0; row < sweGrid->header->nrRows; row++)
    {
        for (long col = 0; col < sweGrid->header->nrCols; col++)
        {
            initSWE = sweGrid->value[row][col];
            if (int(initSWE) != int(sweGrid->header->flag))
            {
                // TODO usare dato reale bulk density se disponibile
                surfaceBulkDensity = DEFAULT_BULK_DENSITY;

                _snowWaterEquivalentMap->value[row][col] = initSWE;

                /*! from [mm] to [m] */
                initSWE = initSWE / 1000;

                _snowMeltMap->value[row][col] = 0;
                _iceContentMap->value[row][col] = 0;
                _lWContentMap->value[row][col] = 0;
                _ageOfSnowMap->value[row][col] = 0;

                _snowSurfaceTempMap->value[row][col] = _initSnowSurfaceTemp;

                float snowSkinThickness = snowPoint->getSnowSkinThickness();

                _surfaceInternalEnergyMap->value[row][col] = Crit3DSnowMaps::computeSurfaceInternalEnergy(_initSnowSurfaceTemp, surfaceBulkDensity, initSWE, snowSkinThickness);

                _internalEnergyMap->value[row][col] = Crit3DSnowMaps::computeInternalEnergyMap(_initSnowSurfaceTemp, surfaceBulkDensity, initSWE);
            }
        }
    }
}


gis::Crit3DRasterGrid* Crit3DSnowMaps::getSnowFallMap()
{
    return _snowFallMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getSnowMeltMap()
{
    return _snowMeltMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getSnowWaterEquivalentMap()
{
    return _snowWaterEquivalentMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getIceContentMap()
{
    return _iceContentMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getLWContentMap()
{
    return _lWContentMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getInternalEnergyMap()
{
    return _internalEnergyMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getSurfaceInternalEnergyMap()
{
    return _surfaceInternalEnergyMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getSnowSurfaceTempMap()
{
    return _snowSurfaceTempMap;
}

gis::Crit3DRasterGrid* Crit3DSnowMaps::getAgeOfSnowMap()
{
    return _ageOfSnowMap;
}

// è la formula 3.27 a pag. 54 in cui ha diviso la surface come la somma dei contributi della parte "water" e di quella "soil"
float computeSurfaceInternalEnergy(float initSnowSurfaceTemp,int bulkDensity, float initSWE, float snowSkinThickness)
{
   return (initSnowSurfaceTemp * (HEAT_CAPACITY_SNOW / 1000 * minValue(initSWE, snowSkinThickness) + SOIL_SPECIFIC_HEAT * maxValue(0.0f, snowSkinThickness - initSWE) * bulkDensity));
}


// LC: InternalEnergyMap pag. 54 formula 3.29  initSoilPackTemp sarebbe da chiamare initSnowPackTemp ????
float computeInternalEnergyMap(float initSoilPackTemp,int bulkDensity, float initSWE)
{
    return ( initSoilPackTemp * (HEAT_CAPACITY_SNOW / 1000 * initSWE + bulkDensity * SNOW_DAMPING_DEPTH * SOIL_SPECIFIC_HEAT) );
}

