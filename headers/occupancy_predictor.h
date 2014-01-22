#ifndef OCCUPANCY_PREDICTOR_H
#define OCCUPANCY_PREDICTOR_H

#include "occupancygrid.h"

class OccupancyCell {
public:
    int occupied;
    int free;
    OccupancyCell()
    {
        occupied = 0;
        free = 0;
    }
    OccupancyCell(int occ, int fr)
    {
        occupied = occ;
        free = fr;
    }
};

typedef QHash<int, QHash<int, OccupancyCell> > PredictedOccupancyGrid;

class OccupancyPredictor
{
public:
    OccupancyPredictor() {}
    void predictFromGrids(const QHash<QString, OccupancyGrid>& grids);
    PredictedOccupancyGrid getPrediction();
    void writeImage(int xMin, int yMin, int xMax, int yMax, QString imageName) const;
private:
    void updatePrediction(const OccupancyGrid& grid);

    PredictedOccupancyGrid _prediction;
};

#endif // OCCUPANCY_PREDICTOR_H
