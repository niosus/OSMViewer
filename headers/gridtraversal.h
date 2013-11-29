#ifndef GRIDTRAVERSAL_H
#define GRIDTRAVERSAL_H

#include <vector>
#include <QPoint>

typedef std::vector<QPoint> GridLine;

class GridTraversal {
 public: 
  GridTraversal() {}
  static void  gridLine( const QPoint& start, const QPoint& end, GridLine& line );

 protected:
  static void gridLineCore( const QPoint& start, const QPoint& end, GridLine& line );
};

#endif
