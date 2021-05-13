#pragma once

#ifndef ORIENTATION_INCLUDED
#define ORIENTATION_INCLUDED

#undef DVAPI
#undef DVVAR
#ifdef TOONZLIB_EXPORTS
#define DVAPI DV_EXPORT_API
#define DVVAR DV_EXPORT_VAR
#else
#define DVAPI DV_IMPORT_API
#define DVVAR DV_IMPORT_VAR
#endif

#include "tcommon.h"
#include "cellposition.h"
#include "toonz/cellpositionratio.h"
#include <QPoint>
#include <QLine>
#include <QRect>
#include <QPainterPath>
#include <map>
#include <vector>

// Defines timeline direction: top to bottom;  left to right.
// old (vertical timeline) = new (universal)    = old (kept)
//                x        =   layer axis       =   column
//                y        =   frame axis       =    row

// A pair of numbers
class DVAPI NumberRange {
  int _from, _to;  // _from <= _to

  NumberRange() : _from(0), _to(0) {}

public:
  NumberRange(int from, int to)
      : _from(std::min(from, to)), _to(std::max(from, to)) {}

  int from() const { return _from; }
  int to() const { return _to; }

  int length() const { return _to - _from; }
  int middle() const { return (_to + _from) / 2; }
  int weight(double toWeight) const;
  double ratio(int at) const;

  NumberRange adjusted(int addFrom, int addTo) const;
};

class ColumnFan;
class QPixmap;
class QPainterPath;

//! lists predefined rectangle sizes and positions (relative to top left corner
//! of a cell)
enum class PredefinedRect {
  CELL,                     //! size of a cell
  CAMERA_CELL,              //! size of a cell of the camera column
  DRAG_HANDLE_CORNER,       //! area for dragging a cell
  KEY_ICON,                 //! position of key icon
  CAMERA_KEY_ICON,          //! position of key icon in the camera column
  CELL_NAME,                //! cell name box
  CELL_NAME_WITH_KEYFRAME,  //! cell name box when keyframe is displayed
  END_EXTENDER,             //! bottom / right extender
  BEGIN_EXTENDER,           //! top / left extender
  KEYFRAME_AREA,            //! part of cell dedicated to key frames
  DRAG_AREA,                //! draggable side bar
  SOUND_TRACK,              //! area dedicated to waveform display
  PREVIEW_TRACK,            //! sound preview area
  BEGIN_SOUND_EDIT,         //! top sound resize
  END_SOUND_EDIT,           //! bottom sound resize
  NOTE_AREA,                //! size of top left note controls
  NOTE_ICON,                //! size of note icons that appear in cells area
  FRAME_LABEL,              //! area for writing frame number
  FRAME_HEADER,
  LAYER_HEADER,
  FOLDED_LAYER_HEADER,  //! size of layer header when it is folded
  CAMERA_LAYER_HEADER,
  PLAY_RANGE,       //! area for play range marker within frame header
  ONION,            //! onion handle placement
  ONION_DOT,        //! moveable dot placement
  ONION_DOT_FIXED,  //! fixed dot placement
  ONION_AREA,       //! area where mouse events will alter onion
  ONION_FIXED_DOT_AREA,
  ONION_DOT_AREA,
  PINNED_CENTER_KEY,   //! displays a small blue number
  RENAME_COLUMN,       //! where column rename control appears after clicking
  EYE_AREA,            //! clickable area larger than the eye, containing it
  EYE,                 //! the eye itself
  PREVIEW_LAYER_AREA,  //! clickable area larger than preview icon, containing
                       //! it
  PREVIEW_LAYER,
  LOCK_AREA,          //! clickable area larger than lock icon, containing it
  LOCK,               //! the lock icon itself
  CAMERA_LOCK_AREA,   //! lock area for the camera column
  CAMERA_LOCK,        //! the lock icon for camera column
  DRAG_LAYER,         //! draggable area in layer header
  LAYER_NAME,         //! where to display column name. clicking will rename
  CAMERA_LAYER_NAME,  //! where to display the camera column name
  LAYER_NUMBER,       //! where to display column number.
  SOUND_ICON,
  VOLUME_TRACK,        //! area where track is displayed
  VOLUME_AREA,         //! active area for volume control
  LOOP_ICON,           //! area for repeat animation icon
  CAMERA_LOOP_ICON,    //! area for repeat icon in the camera column
  LAYER_HEADER_PANEL,  //! panel displaying headers for the layer rows in
                       //! timeline mode
  THUMBNAIL_AREA,      //! area for header thumbnails and other icons
  THUMBNAIL,           //! the actual thumbnail, if there is one
  CAMERA_ICON_AREA,    //! area for the camera column icon
  CAMERA_ICON,         //! the actual camera column icon
  PEGBAR_NAME,         //! where to display pegbar name
  PARENT_HANDLE_NAME,  //! where to display parent handle number
  FILTER_COLOR,        //! where to show layer's filter color
  CONFIG_AREA,  //! clickable area larger than the config icon, containing it
  CONFIG,       //! the config icon itself
  CAMERA_CONFIG_AREA,        //! config area for the camera column
  CAMERA_CONFIG,             //! the config icon for camera column
  FRAME_MARKER_AREA,         //! Cell's frame indicator
  CAMERA_FRAME_MARKER_AREA,  //! Cell's frame indicator for camera column
  FRAME_INDICATOR,           //! Row # indicator
  ZOOM_SLIDER_AREA,
  ZOOM_SLIDER,
  ZOOM_IN_AREA,
  ZOOM_IN,
  ZOOM_OUT_AREA,
  ZOOM_OUT,
  LAYER_FOOTER_PANEL,
  PREVIEW_FRAME_AREA,
  SHIFTTRACE_DOT,
  SHIFTTRACE_DOT_AREA,
  PANEL_EYE,
  PANEL_PREVIEW_LAYER,
  PANEL_LOCK,
  PANEL_LAYER_NAME
};
enum class PredefinedLine {
  LOCKED,              //! dotted vertical line when cell is locked
  SEE_MARKER_THROUGH,  //! horizontal marker visible through drag handle
  CONTINUE_LEVEL,      //! level with the same name represented by vertical line
  CONTINUE_LEVEL_WITH_NAME,  //! adjusted when level name is on each marker
  EXTENDER_LINE              //! see grid through extender handle
};
enum class PredefinedDimension {
  LAYER,                 //! width of a layer column / height of layer row
  FRAME,                 //! height of frame row / width of frame column
  INDEX,                 //! index of this orientation in the array of all
  SOUND_AMPLITUDE,       //! amplitude of sound track, in pixels
  FRAME_LABEL_ALIGN,     //! alignment flag for frame number
  ONION_TURN,            //! onion handle turn in degrees
  QBOXLAYOUT_DIRECTION,  //! direction of QBoxLayout
  CENTER_ALIGN,          //! horizontal / vertical align
  CAMERA_LAYER,          //! width of a camera column / height of camera row
  SCALE_THRESHOLD        //! scale threshold to simplify the view
};
enum class PredefinedPath {
  DRAG_HANDLE_CORNER,   //! triangle corner at drag sidebar
  BEGIN_EASE_TRIANGLE,  //! triangle marking beginning of ease range
  END_EASE_TRIANGLE,    //! triangle marking end of ease range
  BEGIN_PLAY_RANGE,     //! play range markers
  END_PLAY_RANGE,
  VOLUME_SLIDER_TRACK,  //! slider track
  VOLUME_SLIDER_HEAD,   //! slider head
  TIME_INDICATOR_HEAD,  //! current time indicator head
  FRAME_MARKER_DIAMOND
};
enum class PredefinedPoint {
  KEY_HIDDEN,  //! move extender handle that much if key icons are disabled
  EXTENDER_XY_RADIUS,        //! x and y radius for rounded rectangle
  VOLUME_DIVISIONS_TOP_LEFT  //! where to draw volume slider
};
enum class PredefinedRange {
  HEADER_FRAME,  //! size of of column header height(v) / row header width(h)
  HEADER_LAYER,  //! size of row header width(v) / column header height(h)
};
enum class PredefinedFlag {
  DRAG_LAYER_BORDER,
  DRAG_LAYER_VISIBLE,
  LAYER_NAME_BORDER,
  LAYER_NAME_VISIBLE,
  LAYER_NUMBER_BORDER,
  LAYER_NUMBER_VISIBLE,
  EYE_AREA_BORDER,
  EYE_AREA_VISIBLE,
  LOCK_AREA_BORDER,
  LOCK_AREA_VISIBLE,
  PREVIEW_LAYER_AREA_BORDER,
  PREVIEW_LAYER_AREA_VISIBLE,
  CONFIG_AREA_BORDER,
  CAMERA_CONFIG_AREA_BORDER,
  CONFIG_AREA_VISIBLE,
  CAMERA_CONFIG_AREA_VISIBLE,
  PEGBAR_NAME_BORDER,
  PEGBAR_NAME_VISIBLE,
  PARENT_HANDLE_NAME_BORDER,
  PARENT_HANDLE_NAME_VISIBILE,
  THUMBNAIL_AREA_BORDER,
  THUMBNAIL_AREA_VISIBLE,
  CAMERA_ICON_VISIBLE,
  VOLUME_AREA_VERTICAL
};

// Knows everything about geometry of a particular orientation.
class DVAPI Orientation {
protected:
  std::map<PredefinedRect, QRect> _rects;
  std::map<PredefinedLine, QLine> _lines;
  std::map<PredefinedDimension, int> _dimensions;
  std::map<PredefinedPath, QPainterPath> _paths;
  std::map<PredefinedPoint, QPoint> _points;
  std::map<PredefinedRange, NumberRange> _ranges;
  std::map<PredefinedFlag, bool> _flags;

public:
  virtual CellPosition xyToPosition(const QPoint &xy,
                                    const ColumnFan *fan) const           = 0;
  virtual QPoint positionToXY(const CellPosition &position,
                              const ColumnFan *fan) const                 = 0;
  virtual CellPositionRatio xyToPositionRatio(const QPointF &xy) const    = 0;
  virtual QPointF positionRatioToXY(const CellPositionRatio &ratio) const = 0;

  virtual int colToLayerAxis(int layer, const ColumnFan *fan) const = 0;
  virtual int rowToFrameAxis(int frame) const                       = 0;

  virtual QPoint frameLayerToXY(int frameAxis, int layerAxis) const = 0;
  QRect frameLayerRect(const NumberRange &frameAxis,
                       const NumberRange &layerAxis) const;

  virtual NumberRange layerSide(const QRect &area) const = 0;
  virtual NumberRange frameSide(const QRect &area) const = 0;
  virtual int layerAxis(const QPoint &xy) const          = 0;
  virtual int frameAxis(const QPoint &xy) const          = 0;
  //! top right corner in vertical layout. bottom left in horizontal
  virtual QPoint topRightCorner(const QRect &area) const = 0;
  QRect foldedRectangle(int layerAxis, const NumberRange &frameAxis,
                        int i) const;
  QLine foldedRectangleLine(int layerAxis, const NumberRange &frameAxis,
                            int i) const;

  virtual CellPosition arrowShift(int direction) const = 0;

  //! line was vertical in vertical timeline. adjust accordingly
  QLine verticalLine(int layerAxis, const NumberRange &frameAxis) const;
  QLine horizontalLine(int frameAxis, const NumberRange &layerAxis) const;

  virtual bool isVerticalTimeline() const = 0;
  virtual bool flipVolume() const         = 0;

  virtual QString name() const            = 0;
  virtual QString caption() const         = 0;
  virtual const Orientation *next() const = 0;

  const QRect &rect(PredefinedRect which) const { return _rects.at(which); }
  const QLine &line(PredefinedLine which) const { return _lines.at(which); }
  int dimension(PredefinedDimension which) const {
    return _dimensions.at(which);
  }
  const QPainterPath &path(PredefinedPath which) const {
    return _paths.at(which);
  }
  const QPoint &point(PredefinedPoint which) const { return _points.at(which); }
  const NumberRange &range(PredefinedRange which) const {
    return _ranges.at(which);
  }
  const bool &flag(PredefinedFlag which) const { return _flags.at(which); }

  virtual int cellWidth() const      = 0;
  virtual int cellHeight() const     = 0;
  virtual int foldedCellSize() const = 0;

  virtual ~Orientation() {}

protected:
  void addRect(PredefinedRect which, const QRect &rect);
  void addLine(PredefinedLine which, const QLine &line);
  void addDimension(PredefinedDimension which, int dimension);
  void addPath(PredefinedPath which, const QPainterPath &path);
  void addPoint(PredefinedPoint which, const QPoint &point);
  void addRange(PredefinedRange which, const NumberRange &range);
  void addFlag(PredefinedFlag which, const bool &flag);
};

// Enumerates all orientations available in the system as global const objects.
class DVAPI Orientations {
  const Orientation *_topToBottom, *_leftToRight;
  std::vector<const Orientation *> _all;

  Orientations();

public:
  ~Orientations();

  static const Orientations &instance();

  static const int COUNT = 2;

  static const Orientation *topToBottom();
  static const Orientation *leftToRight();

  static const std::vector<const Orientation *> &all();

  static const Orientation *byName(const QString &name);
};

#endif
