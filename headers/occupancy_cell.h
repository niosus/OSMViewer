#ifndef OCCUPANCY_CELL_H
#define OCCUPANCY_CELL_H

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

#endif // OCCUPANCY_CELL_H
