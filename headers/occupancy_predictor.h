#ifndef OCCUPANCY_PREDICTOR_H
#define OCCUPANCY_PREDICTOR_H

#include "occupancy_grid.h"
#include "occupancy_cell.h"

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
