/*-----------------------------------------------------------------------------------
    COPYRIGHT 2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

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
-----------------------------------------------------------------------------------*/


#include <math.h>
#include <malloc.h>

#include "commonConstants.h"
#include "gis.h"

namespace gis
{
    Crit3DEllipsoid::Crit3DEllipsoid()
    {
        // WGS84
        this->equatorialRadius = 6378137.0 ;
        this->eccentricitySquared = 6.69438000426083E-03;
    }

    Crit3DGisSettings::Crit3DGisSettings()
    {
        this->isNorthernEmisphere = true;
        this->utmZone = 32;
        this->timeZone = 1;
        this->isUTC = true;
    }

    Crit3DPixel::Crit3DPixel()
    {
        this->x = 0;
        this->y = 0;
    }

    Crit3DPixel::Crit3DPixel(int x0, int y0)
    {
        this->x = x0;
        this->y = y0;
    }

    Crit3DUtmPoint::Crit3DUtmPoint()
    {
        this->x = NODATA;
        this->y = NODATA;
    }

    Crit3DUtmPoint::Crit3DUtmPoint(double myX, double myY)
    {
        this->x = myX;
        this->y = myY;
    }

    bool Crit3DUtmPoint::isInsideGrid(const Crit3DGridHeader& myGridHeader) const
    {
        return (x >= myGridHeader.llCorner->x && x <= myGridHeader.llCorner->x + (myGridHeader.nrCols*myGridHeader.cellSize)
                && y >= myGridHeader.llCorner->y && y <= myGridHeader.llCorner->y + (myGridHeader.nrRows*myGridHeader.cellSize));
    }

    Crit3DPoint::Crit3DPoint()
    {
        this->utm.x = NODATA;
        this->utm.y = NODATA;
        this->z = NODATA;
    }

    Crit3DPoint::Crit3DPoint(double myX, double myY, double myZ)
    {
        utm.x = myX;
        utm.y = myY;
        z = myZ;
    }

    Crit3DGridHeader::Crit3DGridHeader()
    {
        nrRows = 0;
        nrCols = 0;
        cellSize = NODATA;
        flag = NODATA;
        llCorner = new Crit3DUtmPoint();
    }

    bool operator == (const Crit3DGridHeader& myHeader1, const Crit3DGridHeader& myHeader2)
    {
        return ((myHeader1.cellSize == myHeader2.cellSize) &&
                (myHeader1.flag == myHeader2.flag) &&
                (fabs(myHeader1.llCorner->x - myHeader2.llCorner->x)<0.1) &&
                (fabs(myHeader1.llCorner->y - myHeader2.llCorner->y)<0.1) &&
                (myHeader1.nrCols == myHeader2.nrCols) &&
                (myHeader1.nrRows == myHeader2.nrRows));
    }

    Crit3DRasterGrid::Crit3DRasterGrid()
    {
        isLoaded = false;
        timeString = "";
        header = new Crit3DGridHeader();
        colorScale = new Crit3DColorScale();
        minimum = NODATA;
        maximum = NODATA;
        value = NULL;
    }

    bool Crit3DRasterGrid::initializeGrid(const Crit3DRasterGrid& myInitGrid)
    {
        this->freeGrid();

        *header = *(myInitGrid.header);
        *colorScale = *(myInitGrid.colorScale);

        minimum = (float)header->flag;
        maximum = (float)header->flag;

        long myRow, myCol;

        value = (float **) calloc(header->nrRows, sizeof(float *));
        for (myRow = 0; myRow < header->nrRows; myRow++)
            value[myRow] = (float *) calloc(header->nrCols, sizeof(float));

        for (myRow = 0; myRow < header->nrRows; myRow++)
            for (myCol = 0; myCol < header->nrCols; myCol++)
                value[myRow][myCol] = header->flag;

        isLoaded = true;

        return (true);
    }

    bool Crit3DRasterGrid::initializeGrid(const Crit3DRasterGrid& myInitGrid, float myInitValue)
    {
        this->freeGrid();

        *header = *(myInitGrid.header);
        *colorScale = *(myInitGrid.colorScale);

        minimum = myInitValue;
        maximum = myInitValue;

        value = (float **) calloc(header->nrRows, sizeof(float *));
        for (long myRow = 0; myRow < header->nrRows; myRow++)
            value[myRow] = (float *) calloc(header->nrCols, sizeof(float));

        for (long myRow = 0; myRow < header->nrRows; myRow++)
            for (long myCol = 0; myCol < header->nrCols; myCol++)
                value[myRow][myCol] = myInitValue;

        isLoaded = true;

        return (true);

    }

    bool Crit3DRasterGrid::setConstantValue(float myInitValue)
    {
        if (! isLoaded) return false;

        minimum = myInitValue;
        maximum = myInitValue;

        for (long myRow = 0; myRow < header->nrRows; myRow++)
            for (long myCol = 0; myCol < header->nrCols; myCol++)
                value[myRow][myCol] = myInitValue;

        if (! gis::updateMinMaxRasterGrid(this))
            return (false);

        return (true);
    }

    bool Crit3DRasterGrid::setConstantValueWithBase(float myInitValue, const Crit3DRasterGrid& myInitGrid)
    {
        if (! isLoaded) return false;
        if (! (*(header) == *(myInitGrid.header))) return false;

        minimum = myInitValue;
        maximum = myInitValue;

        for (long myRow = 0; myRow < header->nrRows; myRow++)
            for (long myCol = 0; myCol < header->nrCols; myCol++)
                if (myInitGrid.value[myRow][myCol] != myInitGrid.header->flag)
                    value[myRow][myCol] = myInitValue;

        if (! gis::updateMinMaxRasterGrid(this))
            return (false);

        return (true);
    }

    Crit3DPoint Crit3DRasterGrid::mapCenter()
    {
        Crit3DPoint myPoint;
        myPoint.utm.x = (header->llCorner->x + (header->nrCols * header->cellSize)/2.);
        myPoint.utm.y = (header->llCorner->y + (header->nrRows * header->cellSize)/2.);
        long myRow, myCol;
        if (getRowColFromXY(*this, myPoint.utm.x, myPoint.utm.y, &myRow, &myCol))
            myPoint.z = this->value[myRow][myCol];
        else
            myPoint.z = this->header->flag;

        return myPoint;
    }


    void Crit3DRasterGrid::freeGrid()
    {
        for (long myRow = 0; myRow < header->nrRows; myRow++)
            if (value[myRow] != NULL) ::free(value[myRow]);
        if (header->nrRows != 0)::free(value);

        timeString = "";

        minimum = NODATA;
        maximum = NODATA;
        header->nrRows = 0;
        header->nrCols = 0;
        isLoaded = false;
    }

    void Crit3DRasterGrid::emptyGrid()
    {
        for (long myRow = 0; myRow < header->nrRows; myRow++)
            for (long myCol = 0; myCol < header->nrCols; myCol++)
                value[myRow][myCol] = header->flag;
    }

    Crit3DRasterGrid::~Crit3DRasterGrid()
    {
        freeGrid();
    }

    // return X,Y of cell center
    Crit3DUtmPoint* Crit3DRasterGrid::utmPoint(long myRow, long myCol)
    {
        double x, y;
        Crit3DUtmPoint *myPoint;


        x = header->llCorner->x + header->cellSize * (myCol + 0.5);
        y = header->llCorner->y + header->cellSize * (header->nrRows - myRow - 0.5);
        myPoint = new Crit3DUtmPoint(x, y);
        return (myPoint);
    }


    bool updateMinMaxRasterGrid(Crit3DRasterGrid* myGrid)
    {
        float myValue, minimum, maximum;
        bool isFirstValue;

        isFirstValue = true;
        minimum = NODATA;
        maximum = NODATA;
        for (int myRow = 0; myRow < myGrid->header->nrRows; myRow++)
            for (int myCol = 0; myCol < myGrid->header->nrCols; myCol++)
            {
                myValue = myGrid->value[myRow][myCol];
                if (myValue != myGrid->header->flag)
                {
                    if (isFirstValue)
                    {
                        minimum = myValue;
                        maximum = myValue;
                        isFirstValue = false;
                    }
                    else
                    {
                        if (myValue < minimum) minimum = myValue;
                        if (myValue > maximum) maximum = myValue;
                    }
                }
            }

        //no values
        if (isFirstValue) return(false);

        myGrid->maximum = maximum;
        myGrid->minimum = minimum;
        myGrid->colorScale->maximum = myGrid->maximum;
        myGrid->colorScale->minimum = myGrid->minimum;
        return(true);
    }

    double computeDistance(Crit3DUtmPoint* p0, Crit3DUtmPoint *p1)
    {
            double dx, dy;

            dx = p1->x - p0->x;
            dy = p1->y - p0->y;

            return sqrt((dx * dx)+(dy * dy));
    }

    float computeDistance(float x1, float y1, float x2, float y2)
    {
            float dx, dy;

            dx = x2 - x1;
            dy = y2 - y1;

            return sqrt((dx * dx)+(dy * dy));
    }

    bool getRowColFromXY(const Crit3DRasterGrid& myGrid, double myX, double myY, long* myRow, long* myCol)
    {
        *myRow = (myGrid.header->nrRows - 1) - (int)floor((myY - myGrid.header->llCorner->y) / myGrid.header->cellSize);
        *myCol = (int)floor((myX - myGrid.header->llCorner->x) / myGrid.header->cellSize);
        return true ;
    }

    bool isOutOfGridRowCol(long myRow, long myCol, const Crit3DRasterGrid& myGrid)
    {

        if ((myRow < 0) || (myRow >= myGrid.header->nrRows) || (myCol < 0) || (myCol >= myGrid.header->nrCols)) return true;
        else return false;
    }

    void getUtmXYFromRowColSinglePrecision(const Crit3DRasterGrid& myGrid,
        long myRow, long myCol, float* myX, float* myY)
    {
            *myX = (float)(myGrid.header->llCorner->x + myGrid.header->cellSize * (myCol + 0.5));
            *myY = (float)(myGrid.header->llCorner->y + myGrid.header->cellSize * (myGrid.header->nrRows - myRow - 0.5));
    }

    void getUtmXYFromRowCol(const Crit3DRasterGrid& myGrid,
        long myRow, long myCol, double* myX, double* myY)
    {
            *myX = myGrid.header->llCorner->x + myGrid.header->cellSize * (myCol + 0.5);
            *myY = myGrid.header->llCorner->y + myGrid.header->cellSize * (myGrid.header->nrRows - myRow - 0.5);
    }

    float getValueFromXY(const Crit3DRasterGrid& myGrid, double x, double y)
    {
        long myRow, myCol;

        if (gis::isOutOfGridXY(x, y, myGrid)) return myGrid.header->flag ;
        getRowColFromXY(myGrid, x, y, &myRow, &myCol);
        return myGrid.value[myRow][myCol];
    }

    float Crit3DRasterGrid::neighbourValue(long myRow, long myCol) const
    {
        for (int dx = -1; dx<=1; dx++)
            for (int dy = -1; dy<=1; dy++)
                if (myRow + dy >= 0 && myRow + dy < this->header->nrRows &&
                        myCol + dx >= 0 && myCol + dx < this->header->nrCols)
                    if (this->value[myRow + dy][myCol + dx] != this->header->flag)
                        return this->value[myRow + dy][myCol + dx];

        return this->header->flag;
    }

    float Crit3DRasterGrid::getValueFromRowCol(long myRow, long myCol) const
    {
        if (myRow < 0 || myRow > (this->header->nrRows - 1) || myCol < 0 || myCol > this->header->nrCols - 1)
            return this->header->flag;
        else
            return this->value[myRow][myCol];
    }

    bool isOutOfGridXY(double x, double y, const Crit3DRasterGrid& myGrid)
    {
        if ((x < myGrid.header->llCorner->x) || (y < myGrid.header->llCorner->y)
            || (x >= (myGrid.header->llCorner->x + (myGrid.header->nrCols * myGrid.header->cellSize)))
            || (y >= (myGrid.header->llCorner->y + (myGrid.header->nrRows * myGrid.header->cellSize))))
            return(true);
        else return(false);
    }

    void getLatLonFromUtm(const Crit3DGisSettings& gisSettings, double utmX, double utmY, double *myLat, double *myLon)
    {
        gis::utmToLatLon(gisSettings.utmZone, gisSettings.isNorthernEmisphere, utmX, utmY, myLat, myLon);
    }

    void latLonToUtm(double lat, double lon, double *utmEasting, double *utmNorthing, int *zoneNumber)
    {
        /* -------------------------------------------------------------------------------
           Converts lat/long to UTM coords.  Equations from USGS Bulletin 1532.
           East Longitudes are positive, West longitudes are negative.
           North latitudes are positive, South latitudes are negative.
           Lat and Lon are in decimal degrees.
         -------------------------------------------------------------------------------
           Source:
           Defense Mapping Agency. 1987b. DMA Technical Report:
           Supplement to Department of Defense World Geodetic System.
           1984 Technical Report. Part I and II. Washington, DC: Defense Mapping Agency
         -------------------------------------------------------------------------------
        */
        static double ellipsoidK0 = 0.9996;
        double eccSquared, lonOrigin, eccPrimeSquared, ae, a, n;
        double t, c,m,lonTemp, latRad,lonRad,lonOriginRad;

        Crit3DEllipsoid referenceEllipsoid;

        ae = referenceEllipsoid.equatorialRadius;
        eccSquared = referenceEllipsoid.eccentricitySquared;

        // Make sure the longitude is between -180.00 .. 179.9:
        lonTemp = (lon + 180) - (floor)((lon + 180) / 360) * 360 - 180;

        latRad = lat * DEG_TO_RAD ;
        lonRad = lonTemp * DEG_TO_RAD ;
        *zoneNumber = (int)ceil((lonTemp + 180.) / 6);

        // Special zones for Norway:
        if ((lat >= 56.0) && (lat < 64.0) && (lonTemp >= 3.0) && (lonTemp < 12.0)) (*zoneNumber) = 32 ;
        // Special zones for Svalbard:
        if ((lat >= 72.0)&&(lat < 84.0))
        {
            if ((lonTemp >= 0) && (lonTemp < 9.0)) (*zoneNumber) = 31;
            else if ((lonTemp >= 9.0)&& (lonTemp < 21.0)) (*zoneNumber) = 33;
            else if ((lonTemp >= 21.0)&& ( lonTemp < 33.0)) (*zoneNumber) = 35;
            else if ((lonTemp >= 33.0)&& (lonTemp < 42.0)) (*zoneNumber) = 3;
        }

        // puts origin in middle of zone
        lonOrigin = ((*zoneNumber) - 1) * 6 - 180 + 3;
        lonOriginRad = lonOrigin * DEG_TO_RAD;

        eccPrimeSquared = eccSquared / (1.0 - eccSquared);

        n = ae / sqrt(1.0 - eccSquared * sin(latRad) * sin(latRad));
        t = tan(latRad) * tan(latRad);
        c = eccPrimeSquared * cos(latRad) * cos(latRad);
        a = cos(latRad) * (lonRad - lonOriginRad);

        m = ae * ((1 - eccSquared / 4 - 3 * eccSquared * eccSquared / 64
          - 5 * eccSquared * eccSquared * eccSquared / 256) * latRad
          - (3 * eccSquared / 8 + 3 * eccSquared * eccSquared / 32
          + 45 * eccSquared * eccSquared * eccSquared / 1024) * sin(2 * latRad)
          + (15 * eccSquared * eccSquared / 256
          + 45 * eccSquared * eccSquared * eccSquared / 1024) * sin(4 * latRad)
          - (35 * eccSquared * eccSquared * eccSquared / 3072) * sin(6 * latRad));

        *utmEasting = (ellipsoidK0 * n * (a + (1 - t + c) * a * a * a / 6
                   + (5 - 18 * t + t * t + 72 * c
                   - 58 * eccPrimeSquared) * a * a * a * a * a / 120)
                   + 500000.0);

        *utmNorthing = (ellipsoidK0 * (m + n * tan(latRad) * (a * a / 2
                    + (5 - t + 9 * c + 4 * c * c) * a * a * a * a / 24
                    + (61 - 58 * t + t * t + 600 * c
                    - 330 * eccPrimeSquared) * a * a * a * a * a * a / 720)));

        // offset for southern hemisphere:
        if (lat < 0) (*utmNorthing) += 10000000.0;
    }


    void latLonToUtmForceZone(int zoneNumber, double lat, double lon, double *utmEasting, double *utmNorthing)
    {
    //-------------------------------------------------------------------------------
    // equivalent to LatLonToUTM forcing UTM zone
    //-------------------------------------------------------------------------------
        double eccSquared, lonOrigin, eccPrimeSquared, ae, a, n , t, c,m,lonTemp, latRad,lonRad,lonOriginRad;
        static double ellipsoidK0 = 0.9996;

        Crit3DEllipsoid referenceEllipsoid;
        ae = referenceEllipsoid.equatorialRadius;
        eccSquared = referenceEllipsoid.eccentricitySquared;

        // make sure the longitude is between -180.00 .. 179.9:
        lonTemp = (lon + 180) - (floor)((lon + 180) / 360) * 360 - 180;

        latRad = lat * DEG_TO_RAD;
        lonRad = lonTemp * DEG_TO_RAD;

        // puts origin in middle of zone
        lonOrigin = (zoneNumber - 1) * 6 - 180 + 3;
        lonOriginRad = lonOrigin * DEG_TO_RAD;

        eccPrimeSquared = eccSquared / (1.0 - eccSquared);

        n = ae / sqrt(1.0 - eccSquared * sin(latRad) * sin(latRad));
        t = tan(latRad) * tan(latRad);
        c = eccPrimeSquared * cos(latRad) * cos(latRad);
        a = cos(latRad) * (lonRad - lonOriginRad);

        m = ae * ((1 - eccSquared / 4 - 3 * eccSquared * eccSquared / 64
          - 5 * eccSquared * eccSquared * eccSquared / 256) * latRad
          - (3 * eccSquared / 8 + 3 * eccSquared * eccSquared / 32
          + 45 * eccSquared * eccSquared * eccSquared / 1024) * sin(2 * latRad)
          + (15 * eccSquared * eccSquared / 256
          + 45 * eccSquared * eccSquared * eccSquared / 1024) * sin(4 * latRad)
          - (35 * eccSquared * eccSquared * eccSquared / 3072) * sin(6 * latRad));

        *utmEasting = (ellipsoidK0 * n * (a + (1 - t + c) * a * a * a / 6
                   + (5 - 18 * t + t * t + 72 * c
                   - 58 * eccPrimeSquared) * a * a * a * a * a / 120)
                   + 500000.0);

        *utmNorthing = (ellipsoidK0 * (m + n * tan(latRad) * (a * a / 2
                    + (5 - t + 9 * c + 4 * c * c) * a * a * a * a / 24
                    + (61 - 58 * t + t * t + 600 * c
                    - 330 * eccPrimeSquared) * a * a * a * a * a * a / 720)));

        // offset for southern hemisphere:
        if (lat < 0) (*utmNorthing) += 10000000.0;

    }


    void utmToLatLon(int zoneNumber, bool north, double utmEasting, double utmNorthing, double *lat, double *lon)
    {
        // -------------------------------------------------------------------------------
        //   Converts UTM coords to Lat/Lng.  Equations from USGS Bulletin 1532.
        //   East Longitudes are positive, West longitudes are negative.
        //   North latitudes are positive, South latitudes are negative.
        //   Lat and Lng are in decimal degrees.
        // -------------------------------------------------------------------------------

        double ae, e1, eccSquared, eccPrimeSquared, n1, t1, c1, r1, d, m, x, y;
        double longOrigin , mu , phi1Rad;
        static double ellipsoidK0 = 0.9996;

        Crit3DEllipsoid referenceEllipsoid;
        ae = referenceEllipsoid.equatorialRadius;
        eccSquared = referenceEllipsoid.eccentricitySquared;

        e1 = (1.0 - sqrt(1.0 - eccSquared)) / (1.0 + sqrt(1.0 - eccSquared));

        // offset for longitude
        x = utmEasting - 500000.0;
        y = utmNorthing;

        //offset used for southern hemisphere
        if (north == false)    y -= 10000000.0;

        eccPrimeSquared = (eccSquared) / (1.0 - eccSquared);

        m = y / ellipsoidK0;
        mu = m / (ae * (1.0 - eccSquared / 4.0 - 3.0 * eccSquared * eccSquared / 64.0
           - 5.0 * eccSquared * eccSquared * eccSquared / 256.0));

        phi1Rad = mu + (3.0 * e1 / 2.0 - 27.0 * e1 * e1 * e1 / 32.0) * sin(2.0 * mu)
                + (21.0 * e1 * e1 / 16.0 - 55.0 * e1 * e1 * e1 * e1 / 32.0) * sin(4.0 * mu)
                + (151.0 * e1 * e1 * e1 / 96.0) * sin(6.0 * mu);

        n1 = ae / sqrt(1.0 - eccSquared * sin(phi1Rad) * sin(phi1Rad));
        t1 = tan(phi1Rad) * tan(phi1Rad);
        c1 = eccPrimeSquared * cos(phi1Rad) * cos(phi1Rad);
        r1 = ae * (1.0 - eccSquared) / pow(1.0 - eccSquared
           * (sin(phi1Rad) * sin(phi1Rad)),1.5);
        d = x / (n1 * ellipsoidK0);

        *lat = phi1Rad - (n1 * tan(phi1Rad) / r1) * (d * d / 2.0
            - (5.0 + 3.0 * t1 + 10 * c1 - 4.0 * c1 * c1
            - 9.0 * eccPrimeSquared) * d * d * d * d / 24.0
            + (61.0 + 90.0 * t1 + 298 * c1 + 45.0 * t1 * t1
            - 252.0 * eccPrimeSquared - 3.0 * c1 * c1) * d * d * d * d * d * d / 720.0);

        *lat *= RAD_TO_DEG ;

        *lon = (d - (1.0 + 2.0 * t1 + c1) * d * d * d / 6.0
            + (5.0 - 2.0 * c1 + 28 * t1 - 3.0 * c1 * c1
            + 8.0 * eccPrimeSquared + 24.0 * t1 * t1)
            * d * d * d * d * d / 120.0) / cos(phi1Rad);

        // puts origin in middle of zone
        longOrigin = (float)(zoneNumber - 1) * 6 - 180 + 3;

        *lon *= RAD_TO_DEG ;
        *lon += longOrigin ;
    }


    // UTM zone:   [1,60]
    // Time zone:  [-12,12]
    // lon:       [-180,180]
    bool isValidUtmTimeZone(int utmZone, int timeZone)
    {
        float lonUtmZone , lonTimeZone;
        lonUtmZone = (float)((utmZone - 1) * 6 - 180 + 3);
        lonTimeZone = (float)(timeZone * 15);
        if (fabs(lonTimeZone - lonUtmZone) <= 7.5) return true;
        else return false;
    }

    bool computeLatLonMaps(const gis::Crit3DRasterGrid& myGrid,
                           gis::Crit3DRasterGrid* latMap, gis::Crit3DRasterGrid* lonMap,
                           const gis::Crit3DGisSettings& gisSettings)
    {
        if (! myGrid.isLoaded) return false;

        double latDegrees, lonDegrees;
        double utmX, utmY;

        latMap->initializeGrid(myGrid);
        lonMap->initializeGrid(myGrid);

        for (long myRow = 0; myRow < myGrid.header->nrRows; myRow++)
            for (long myCol = 0; myCol < myGrid.header->nrCols; myCol++)
                if (myGrid.value[myRow][myCol] != myGrid.header->flag)
                {
                    getUtmXYFromRowCol(myGrid, myRow, myCol, &utmX, &utmY);
                    getLatLonFromUtm(gisSettings, utmX, utmY, &latDegrees, &lonDegrees);

                    latMap->value[myRow][myCol] = (float)latDegrees;
                    lonMap->value[myRow][myCol] = (float)lonDegrees;
                }

        gis::updateMinMaxRasterGrid(latMap);
        gis::updateMinMaxRasterGrid(lonMap);

        latMap->isLoaded = true;
        lonMap->isLoaded = true;

        return true;
    }

    bool computeSlopeAspectMaps(const gis::Crit3DRasterGrid& myDtm,
                                gis::Crit3DRasterGrid* slopeMap, gis::Crit3DRasterGrid* aspectMap)
    {
        if (! myDtm.isLoaded) return false;

        double reciprocalCellSize;
        double dz_dx, dz_dy;
        double mySlope, myAspect;
        double myZ;
        double zNorth, zSouth, zEast, zWest;

        slopeMap->initializeGrid(myDtm);
        aspectMap->initializeGrid(myDtm);

        reciprocalCellSize = 1. / myDtm.header->cellSize;

        for (long myRow = 0; myRow < myDtm.header->nrRows; myRow++)
            for (long myCol = 0; myCol < myDtm.header->nrCols; myCol++)
            {
                myZ = myDtm.value[myRow][myCol];
                if (myZ != myDtm.header->flag)
                {
                    zNorth = myDtm.getValueFromRowCol(myRow-1, myCol);
                    zSouth = myDtm.getValueFromRowCol(myRow+1, myCol);

                    if (zNorth != myDtm.header->flag && zSouth != myDtm.header->flag)
                        dz_dy = 0.5 * (zNorth - zSouth) * reciprocalCellSize;
                    else if (zNorth != myDtm.header->flag)
                        dz_dy = (zNorth - myZ) * reciprocalCellSize;
                    else if (zSouth != myDtm.header->flag)
                        dz_dy = (myZ - zSouth) * reciprocalCellSize;
                    else
                        dz_dy = 0.000001;

                    zWest = myDtm.getValueFromRowCol(myRow, myCol-1);
                    zEast = myDtm.getValueFromRowCol(myRow, myCol+1);

                    if (zWest != myDtm.header->flag && zEast != myDtm.header->flag)
                        dz_dx = 0.5 * (zWest - zEast) * reciprocalCellSize;
                    else if (zWest != myDtm.header->flag)
                        dz_dx = (zWest - myZ) * reciprocalCellSize;
                    else if (zEast != myDtm.header->flag)
                        dz_dx = (myZ - zEast) * reciprocalCellSize;
                    else
                        dz_dx = EPSILON;

                    // slope in degrees
                    mySlope = atan(sqrt(dz_dx * dz_dx + dz_dy * dz_dy)) * RAD_TO_DEG;
                    slopeMap->value[myRow][myCol] = (float)mySlope;

                    // avoid arctan to infinite
                    if (dz_dx == 0.) dz_dx = EPSILON;

                    // compute with zero to east
                    myAspect = 0.0;
                    if (dz_dx > 0)
                        myAspect = atan(dz_dy / dz_dx);
                    else if (dz_dx < 0)
                        myAspect = PI + atan(dz_dy / dz_dx);

                    // convert to zero from north and to degrees
                    myAspect += (PI / 2.);
                    myAspect *= RAD_TO_DEG;

                    aspectMap->value[myRow][myCol] = (float)myAspect;

                }
            }

        gis::updateMinMaxRasterGrid(slopeMap);
        gis::updateMinMaxRasterGrid(aspectMap);

        aspectMap->isLoaded = true;
        slopeMap->isLoaded = true;

        return true;
    }


    bool mapAlgebra(gis::Crit3DRasterGrid* myMap1, gis::Crit3DRasterGrid* myMap2,
                    gis::Crit3DRasterGrid* myMapOut, operationType myOperation)
    {
        if (myMapOut == NULL || myMap1 == NULL || myMap2 == NULL) return false;
        if (! (*(myMap1->header) == *(myMap2->header))) return false;
        if (! (*(myMapOut->header) == *(myMap1->header))) return false;

        for (long myRow=0; myRow<myMapOut->header->nrRows; myRow++)
            for (long myCol=0; myCol<myMapOut->header->nrCols; myCol++)
            {
                if (myMap1->value[myRow][myCol] != myMap1->header->flag && myMap2->value[myRow][myCol] != myMap2->header->flag)
                {
                    if (myOperation == operationMin)
                    {
                        myMapOut->value[myRow][myCol] = minValue(myMap1->value[myRow][myCol], myMap2->value[myRow][myCol]);
                    }
                    else if (myOperation == operationMax)
                    {
                        myMapOut->value[myRow][myCol] = maxValue(myMap1->value[myRow][myCol], myMap2->value[myRow][myCol]);
                    }
                    else if (myOperation == operationSum)
                        myMapOut->value[myRow][myCol] = (myMap1->value[myRow][myCol] + myMap2->value[myRow][myCol]);
                    else if (myOperation == operationSubtract)
                        myMapOut->value[myRow][myCol] = (myMap1->value[myRow][myCol] - myMap2->value[myRow][myCol]);
                    else if (myOperation == operationProduct)
                        myMapOut->value[myRow][myCol] = (myMap1->value[myRow][myCol] * myMap2->value[myRow][myCol]);
                    else if (myOperation == operationDivide)
                    {
                        if (myMap2->value[myRow][myCol] != 0)
                            myMapOut->value[myRow][myCol] = (myMap1->value[myRow][myCol] / myMap2->value[myRow][myCol]);
                        else
                            return false;
                    }
                }
            }

        return true;
    }

    bool mapAlgebra(gis::Crit3DRasterGrid* myMap1, float myValue,
                    gis::Crit3DRasterGrid* myMapOut, operationType myOperation)
    {
        if (myMapOut == NULL || myMap1 == NULL) return false;
        if (! (*(myMap1->header) == *(myMapOut->header))) return false;

        for (long myRow=0; myRow<myMapOut->header->nrRows; myRow++)
            for (long myCol=0; myCol<myMapOut->header->nrCols; myCol++)
            {
                if (myMap1->value[myRow][myCol] != myMap1->header->flag)
                {
                    if (myOperation == operationMin)
                        myMapOut->value[myRow][myCol] = minValue(myMap1->value[myRow][myCol], myValue);
                    else if (myOperation == operationMax)
                        myMapOut->value[myRow][myCol] = maxValue(myMap1->value[myRow][myCol], myValue);
                    else if (myOperation == operationSum)
                        myMapOut->value[myRow][myCol] = (myMap1->value[myRow][myCol] + myValue);
                    else if (myOperation == operationSubtract)
                        myMapOut->value[myRow][myCol] = (myMap1->value[myRow][myCol] - myValue);
                    else if (myOperation == operationProduct)
                        myMapOut->value[myRow][myCol] = (myMap1->value[myRow][myCol] * myValue);
                    else if (myOperation == operationDivide)
                    {
                        if (myValue != 0)
                            myMapOut->value[myRow][myCol] = (myMap1->value[myRow][myCol] / myValue);
                        else
                            return false;
                    }
                }
            }

        return true;
    }

    // return true if value(row, col) > all values of neighbours
    bool isStrictMaximum(const Crit3DRasterGrid& myGrid, long row, long col)
    {
        float z, adjZ;
        z = myGrid.getValueFromRowCol(row, col);
        if (z == myGrid.header->flag) return false;

        for (int r=-1; r<=1; r++)
            for (int c=-1; c<=1; c++)
                {
                if ((r != 0)&&(c != 0))
                    {
                    adjZ = myGrid.getValueFromRowCol(row+r, col+c);
                    if (adjZ != myGrid.header->flag)
                        if (z <= adjZ) return (false);
                    }
                }
        return true;
    }

    bool isUpper(const Crit3DRasterGrid& myGrid, float z, long row, long col)
    {
        if (z == myGrid.header->flag) return false;
        float adjZ = myGrid.getValueFromRowCol(row, col);
        if (adjZ == myGrid.header->flag) return false;
        return (z > adjZ);
    }

    // return true if value(row, col) <= value of neighbours
    bool isMinimum(const Crit3DRasterGrid& myGrid, long row, long col)
    {
        float z = myGrid.getValueFromRowCol(row, col);
        if (z == myGrid.header->flag) return false;
        for (int r=-1; r<=1; r++)
            for (int c=-1; c<=1; c++)
                if ((r != 0)&&(c != 0))
                    if (isUpper(myGrid, z, row +r, col +c)) return (false);
        return true;
    }

    // return true if (row, col) is a minimum, or adjacent to a minimum
    bool isMinimumOrNearMinimum(const Crit3DRasterGrid& myGrid, long row, long col)
    {
        float z = myGrid.getValueFromRowCol(row, col);
        if (z != myGrid.header->flag)
            for (int r=-1; r<=1; r++)
                for (int c=-1; c<=1; c++)
                    if (isMinimum(myGrid, row + r, col + c)) return true;

        return false;
    }


    // return true if is valid, and one neighbour (at least) is nodata
    bool isBoundary(const Crit3DRasterGrid& myGrid, long row, long col)
    {
        float z = myGrid.getValueFromRowCol(row, col);
        if (z != myGrid.header->flag)
            for (int r=-1; r<=1; r++)
                for (int c=-1; c<=1; c++)
                    if ((r != 0)&&(c != 0))
                        if (myGrid.getValueFromRowCol(row + r, col + c) == myGrid.header->flag)
                            return true;
        return false;
    }


    float prevailingValue(const std::vector<float> valueList)
    {
        std::vector <float> values;
        std::vector <unsigned int> counters;
        float prevailing;
        unsigned int i, j, maxCount;
        bool isFound;

        values.push_back(valueList[0]);
        counters.push_back(1);
        for (i = 1; i < valueList.size(); i++)
        {
            isFound = false;
            j = 0;
            while ((j < values.size()) && (!isFound))
            {
                if (valueList[i] == values[j])
                {
                    isFound = true;
                    counters[j]++;
                }
                j++;
            }

            if (!isFound)
            {
                values.push_back(valueList[i]);
                counters.push_back(1);
            }
        }

        maxCount = counters[0];
        prevailing = values[0];
         for (i = 1; i < values.size(); i++)
            if (counters[i] > maxCount)
            {
                maxCount = counters[i];
                prevailing = values[i];
            }

        return(prevailing);
    }


    bool prevailingMap(const Crit3DRasterGrid& inputMap,  Crit3DRasterGrid *outputMap)
    {
        int i, j;
        float value;
        double x, y;
        long inputRow, inputCol;
        int dim = 3;

        std::vector <float> valuesList;
        double step = outputMap->header->cellSize / (2*dim+1);

        for (long row = 0; row < outputMap->header->nrRows ; row++)
            for (long col = 0; col < outputMap->header->nrCols; col++)
            {
                //center
                getUtmXYFromRowCol(*outputMap, row, col, &x, &y);
                valuesList.resize(0);

                for (i = -dim; i <= dim; i++)
                    for (j = -dim; j <= dim; j++)
                        if (! gis::isOutOfGridXY(x+(i*step), y+(j*step), inputMap))
                        {
                            gis::getRowColFromXY(inputMap, x+(i*step), y+(j*step), &inputRow, &inputCol);
                            value = inputMap.value[inputRow][inputCol];
                            if (value != inputMap.header->flag)
                                valuesList.push_back(value);
                        }

                if (valuesList.size() == 0)
                    outputMap->value[row][col] = outputMap->header->flag;
                else
                    outputMap->value[row][col] = prevailingValue(valuesList);
            }

        return true;
    }

}
