#ifndef MERCATOR_H
#define MERCATOR_H

// From http://wiki.openstreetmap.org/wiki/Mercator#Elliptical_Mercator

double merc_x (double lon);
double merc_y (double lat);
double merc_lon (double x);
double merc_lat (double y);

#endif
