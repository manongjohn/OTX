#include "orientation.h"
#include "toonz/columnfan.h"
#include "toonz/preferences.h"

#include <QPainterPath>
#include <QBoxLayout>
#include <math.h>
#include <stdexcept>

namespace {
const int KEY_ICON_WIDTH      = 11;
const int KEY_ICON_HEIGHT     = 13;
const int EASE_TRIANGLE_SIZE  = 4;
const int PLAY_MARKER_SIZE    = 10;
const int ONION_SIZE          = 19;
const int ONION_DOT_SIZE      = 8;
const int PINNED_SIZE         = 10;
const int FRAME_MARKER_SIZE   = 4;
const int FOLDED_CELL_SIZE    = 9;
const int SHIFTTRACE_DOT_SIZE = 12;

QRect iconRect(const QRect &areaRect, const int iconWidth, const int iconHeight,
               const int xOffset = 0) {
  return QRect(
      areaRect.left() + xOffset + ((areaRect.width() / 2) - (iconWidth / 2)),
      areaRect.top() + ((areaRect.height() / 2) - (iconHeight / 2)), iconWidth,
      iconHeight);
}

}  // namespace

class TopToBottomOrientation : public Orientation {
  const int CELL_WIDTH                 = 74;
  const int CELL_HEIGHT                = 20;
  const int CELL_DRAG_WIDTH            = 7;
  const int EXTENDER_WIDTH             = 20;
  const int EXTENDER_HEIGHT            = 8;
  const int SOUND_PREVIEW_WIDTH        = 7;
  const int LAYER_HEADER_HEIGHT        = 119;
  const int FOLDED_LAYER_HEADER_HEIGHT = LAYER_HEADER_HEIGHT;
  const int FOLDED_LAYER_HEADER_WIDTH  = 8;
  const int FRAME_HEADER_WIDTH         = CELL_WIDTH;
  const int PLAY_RANGE_X = FRAME_HEADER_WIDTH / 2 - PLAY_MARKER_SIZE;
  const int ONION_X = 0, ONION_Y = 0;
  const int ICON_WIDTH               = 18;
  const int ICON_HEIGHT              = 18;
  const int TRACKLEN                 = 60;
  const int SHIFTTRACE_DOT_OFFSET    = 3;
  const int CAMERA_CELL_WIDTH        = 22;
  const int LAYER_FOOTER_PANEL_WIDTH = 16;

public:
  TopToBottomOrientation();

  virtual CellPosition xyToPosition(const QPoint &xy,
                                    const ColumnFan *fan) const override;
  virtual QPoint positionToXY(const CellPosition &position,
                              const ColumnFan *fan) const override;
  virtual CellPositionRatio xyToPositionRatio(const QPointF &xy) const override;
  virtual QPointF positionRatioToXY(
      const CellPositionRatio &ratio) const override;

  virtual int colToLayerAxis(int layer, const ColumnFan *fan) const override;
  virtual int rowToFrameAxis(int frame) const override;

  virtual QPoint frameLayerToXY(int frameAxis, int layerAxis) const override;
  virtual int layerAxis(const QPoint &xy) const override;
  virtual int frameAxis(const QPoint &xy) const override;

  virtual NumberRange layerSide(const QRect &area) const override;
  virtual NumberRange frameSide(const QRect &area) const override;
  virtual QPoint topRightCorner(const QRect &area) const override;

  virtual CellPosition arrowShift(int direction) const override;

  virtual QString name() const override { return "TopToBottom"; }
  virtual QString caption() const override { return QObject::tr("Xsheet"); }
  virtual const Orientation *next() const override {
    return Orientations::leftToRight();
  }

  virtual bool isVerticalTimeline() const override { return true; }
  virtual bool flipVolume() const override { return false; }

  virtual int cellWidth() const override { return CELL_WIDTH; }
  virtual int cellHeight() const override { return CELL_HEIGHT; }
  virtual int foldedCellSize() const override { return FOLDED_CELL_SIZE; }
};

class LeftToRightOrientation : public Orientation {
  const int CELL_WIDTH           = 50;
  const int CELL_HEIGHT          = 24;
  const int CELL_DRAG_HEIGHT     = 7;
  const int EXTENDER_WIDTH       = 8;
  const int EXTENDER_HEIGHT      = 12;
  const int SOUND_PREVIEW_HEIGHT = 6;
  const int FRAME_HEADER_HEIGHT  = 50;
  const int ONION_X = 0, ONION_Y = 0;
  const int PLAY_RANGE_Y       = ONION_SIZE;
  const int ICON_WIDTH         = 20;
  const int ICON_HEIGHT        = 20;
  const int ICON_OFFSET        = ICON_WIDTH;
  const int ICONS_WIDTH        = ICON_OFFSET * 4;  // 88
  const int THUMBNAIL_WIDTH    = 43;
  const int LAYER_NUMBER_WIDTH = 20;
  const int LAYER_NAME_WIDTH   = 150;
  const int LAYER_HEADER_WIDTH =
      ICONS_WIDTH + THUMBNAIL_WIDTH + LAYER_NUMBER_WIDTH + LAYER_NAME_WIDTH;
  const int FOLDED_LAYER_HEADER_HEIGHT = 8;
  const int FOLDED_LAYER_HEADER_WIDTH  = LAYER_HEADER_WIDTH;
  const int TRACKLEN                   = 60;
  const int SHIFTTRACE_DOT_OFFSET      = 5;
  const int LAYER_HEADER_PANEL_HEIGHT  = 20;
  const int LAYER_FOOTER_PANEL_HEIGHT  = 16;
  const int CAMERA_CELL_HEIGHT         = CELL_HEIGHT;

public:
  LeftToRightOrientation();

  virtual CellPosition xyToPosition(const QPoint &xy,
                                    const ColumnFan *fan) const override;
  virtual QPoint positionToXY(const CellPosition &position,
                              const ColumnFan *fan) const override;
  virtual CellPositionRatio xyToPositionRatio(const QPointF &xy) const override;
  virtual QPointF positionRatioToXY(
      const CellPositionRatio &ratio) const override;

  virtual int colToLayerAxis(int layer, const ColumnFan *fan) const override;
  virtual int rowToFrameAxis(int frame) const override;

  virtual QPoint frameLayerToXY(int frameAxis, int layerAxis) const override;
  virtual int layerAxis(const QPoint &xy) const override;
  virtual int frameAxis(const QPoint &xy) const override;

  virtual NumberRange layerSide(const QRect &area) const override;
  virtual NumberRange frameSide(const QRect &area) const override;
  virtual QPoint topRightCorner(const QRect &area) const override;

  virtual CellPosition arrowShift(int direction) const override;

  virtual QString name() const override { return "LeftToRight"; }
  virtual QString caption() const override { return QObject::tr("Timeline"); }
  virtual const Orientation *next() const override {
    return Orientations::topToBottom();
  }

  virtual bool isVerticalTimeline() const override { return false; }
  virtual bool flipVolume() const override { return false; }

  virtual int cellWidth() const override { return CELL_WIDTH; }
  virtual int cellHeight() const override { return CELL_HEIGHT; }
  virtual int foldedCellSize() const override { return FOLDED_CELL_SIZE; }
};

/// -------------------------------------------------------------------------------

int NumberRange::weight(double toWeight) const {  // weight ranging 0..1
  return _from + (_to - _from) * toWeight;
}

NumberRange NumberRange::adjusted(int addFrom, int addTo) const {
  return NumberRange(_from + addFrom, _to + addTo);
}

double NumberRange::ratio(int at) const {
  double result = ((double)at - _from) / (_to - _from);
  if (result < 0) result = 0;
  if (result > 1) result = 1;
  return result;
}

/// -------------------------------------------------------------------------------

// const int Orientations::COUNT = 2;

Orientations::Orientations() : _topToBottom(nullptr), _leftToRight(nullptr) {
  _topToBottom = new TopToBottomOrientation();
  _leftToRight = new LeftToRightOrientation();

  _all.push_back(_topToBottom);
  _all.push_back(_leftToRight);
}
Orientations::~Orientations() {
  delete _topToBottom;
  _topToBottom = nullptr;
  delete _leftToRight;
  _leftToRight = nullptr;
}

const Orientations &Orientations::instance() {
  static Orientations singleton;
  return singleton;
}

const Orientation *Orientations::topToBottom() {
  return instance()._topToBottom;
}
const Orientation *Orientations::leftToRight() {
  return instance()._leftToRight;
}
const std::vector<const Orientation *> &Orientations::all() {
  return instance()._all;
}
const Orientation *Orientations::byName(const QString &name) {
  std::vector<const Orientation *> m_all = all();
  for (auto it = m_all.begin(); it != m_all.end(); it++)
    if ((*it)->name() == name) return *it;
  throw std::runtime_error(
      (QString("no such orientation: ") + name).toStdString().c_str());
}

/// -------------------------------------------------------------------------------

QLine Orientation::verticalLine(int layerAxis,
                                const NumberRange &frameAxis) const {
  QPoint first  = frameLayerToXY(frameAxis.from(), layerAxis);
  QPoint second = frameLayerToXY(frameAxis.to(), layerAxis);
  return QLine(first, second);
}
QLine Orientation::horizontalLine(int frameAxis,
                                  const NumberRange &layerAxis) const {
  QPoint first  = frameLayerToXY(frameAxis, layerAxis.from());
  QPoint second = frameLayerToXY(frameAxis, layerAxis.to());
  return QLine(first, second);
}
QRect Orientation::frameLayerRect(const NumberRange &frameAxis,
                                  const NumberRange &layerAxis) const {
  QPoint topLeft     = frameLayerToXY(frameAxis.from(), layerAxis.from());
  QPoint bottomRight = frameLayerToXY(frameAxis.to(), layerAxis.to());
  return QRect(topLeft, bottomRight);
}

QRect Orientation::foldedRectangle(int layerAxis, const NumberRange &frameAxis,
                                   int i) const {
  QPoint topLeft = frameLayerToXY(frameAxis.from(), layerAxis + 1 + i * 3);
  QPoint size    = frameLayerToXY(frameAxis.length(), 2);
  return QRect(topLeft, QSize(size.x(), size.y()));
}
QLine Orientation::foldedRectangleLine(int layerAxis,
                                       const NumberRange &frameAxis,
                                       int i) const {
  return verticalLine(layerAxis + i * 3, frameAxis);
}

void Orientation::addRect(PredefinedRect which, const QRect &rect) {
  _rects.erase(which);
  _rects.emplace(which, rect);
}
void Orientation::addLine(PredefinedLine which, const QLine &line) {
  _lines.erase(which);
  _lines.emplace(which, line);
}
void Orientation::addDimension(PredefinedDimension which, int dimension) {
  _dimensions.erase(which);
  _dimensions.emplace(which, dimension);
}
void Orientation::addPath(PredefinedPath which, const QPainterPath &path) {
  _paths.erase(which);
  _paths.emplace(which, path);
}
void Orientation::addPoint(PredefinedPoint which, const QPoint &point) {
  _points.erase(which);
  _points.emplace(which, point);
}
void Orientation::addRange(PredefinedRange which, const NumberRange &range) {
  _ranges.erase(which);
  _ranges.emplace(which, range);
}
void Orientation::addFlag(PredefinedFlag which, const bool &flag) {
  _flags.erase(which);
  _flags.emplace(which, flag);
}
/// -------------------------------------------------------------------------------

TopToBottomOrientation::TopToBottomOrientation() {
  QString layout = Preferences::instance()->getLoadedXsheetLayout();

  int use_header_height = LAYER_HEADER_HEIGHT;

  if (layout == QString("Compact")) use_header_height -= 18;

  int user_folded_header_height = use_header_height;

  //
  // Area rectangles
  //

  // Cell viewer
  QRect cellRect(0, 0, CELL_WIDTH, CELL_HEIGHT);
  addRect(PredefinedRect::CELL, cellRect);
  addRect(PredefinedRect::CAMERA_CELL,
          QRect(0, 0, CAMERA_CELL_WIDTH, CELL_HEIGHT));
  addRect(PredefinedRect::DRAG_HANDLE_CORNER,
          QRect(0, 0, CELL_DRAG_WIDTH, CELL_HEIGHT));
  QRect keyRect(CELL_WIDTH - KEY_ICON_WIDTH,
                (CELL_HEIGHT - KEY_ICON_HEIGHT) / 2, KEY_ICON_WIDTH,
                KEY_ICON_HEIGHT);
  addRect(PredefinedRect::KEY_ICON, keyRect);
  QRect cameraKeyRect((CAMERA_CELL_WIDTH - KEY_ICON_WIDTH) / 2 + 1,
                      (CELL_HEIGHT - KEY_ICON_HEIGHT) / 2, KEY_ICON_WIDTH,
                      KEY_ICON_HEIGHT);
  addRect(PredefinedRect::CAMERA_KEY_ICON, cameraKeyRect);
  QRect nameRect = cellRect.adjusted(8, 0, -6, 0);
  addRect(PredefinedRect::CELL_NAME, nameRect);
  addRect(PredefinedRect::CELL_NAME_WITH_KEYFRAME,
          nameRect.adjusted(0, 0, -KEY_ICON_WIDTH, 0));
  addRect(PredefinedRect::END_EXTENDER,
          QRect(-EXTENDER_WIDTH - KEY_ICON_WIDTH, 0, EXTENDER_WIDTH,
                EXTENDER_HEIGHT));
  addRect(PredefinedRect::BEGIN_EXTENDER,
          QRect(-EXTENDER_WIDTH - KEY_ICON_WIDTH, -EXTENDER_HEIGHT,
                EXTENDER_WIDTH, EXTENDER_HEIGHT));
  addRect(PredefinedRect::KEYFRAME_AREA,
          QRect(CELL_WIDTH - KEY_ICON_WIDTH, 0, KEY_ICON_WIDTH, CELL_HEIGHT));
  addRect(PredefinedRect::DRAG_AREA, QRect(0, 0, CELL_DRAG_WIDTH, CELL_HEIGHT));
  QRect soundRect(CELL_DRAG_WIDTH, 0,
                  CELL_WIDTH - CELL_DRAG_WIDTH - SOUND_PREVIEW_WIDTH,
                  CELL_HEIGHT);
  addRect(PredefinedRect::SOUND_TRACK, soundRect);
  addRect(PredefinedRect::PREVIEW_TRACK,
          QRect(CELL_WIDTH - SOUND_PREVIEW_WIDTH + 1, 0, SOUND_PREVIEW_WIDTH,
                CELL_HEIGHT));
  addRect(PredefinedRect::BEGIN_SOUND_EDIT,
          QRect(CELL_DRAG_WIDTH, 0, CELL_WIDTH - CELL_DRAG_WIDTH, 2));
  addRect(
      PredefinedRect::END_SOUND_EDIT,
      QRect(CELL_DRAG_WIDTH, CELL_HEIGHT - 2, CELL_WIDTH - CELL_DRAG_WIDTH, 2));
  addRect(PredefinedRect::LOOP_ICON, QRect(keyRect.left(), 0, 10, 11));
  addRect(PredefinedRect::CAMERA_LOOP_ICON,
          QRect(cameraKeyRect.left(), 0, 10, 11));
  QRect frameMarker(CELL_WIDTH - FRAME_MARKER_SIZE - 4,
                    (CELL_HEIGHT - FRAME_MARKER_SIZE) / 2, FRAME_MARKER_SIZE,
                    FRAME_MARKER_SIZE);
  addRect(PredefinedRect::FRAME_MARKER_AREA, frameMarker);
  addRect(PredefinedRect::CAMERA_FRAME_MARKER_AREA,
          cameraKeyRect.translated(-1, 0));

  // Note viewer
  addRect(
      PredefinedRect::NOTE_AREA,
      QRect(QPoint(0, 0), QSize(FRAME_HEADER_WIDTH, use_header_height - 1)));
  addRect(PredefinedRect::NOTE_ICON,
          QRect(QPoint(0, 0), QSize(CELL_WIDTH - 2, CELL_HEIGHT - 2)));

  // Layer header panel
  addRect(PredefinedRect::LAYER_HEADER_PANEL, QRect(0, 0, -1, -1));   // hide
  addRect(PredefinedRect::PANEL_EYE, QRect(0, 0, -1, -1));            // hide
  addRect(PredefinedRect::PANEL_PREVIEW_LAYER, QRect(0, 0, -1, -1));  // hide
  addRect(PredefinedRect::PANEL_LOCK, QRect(0, 0, -1, -1));           // hide
  addRect(PredefinedRect::PANEL_LAYER_NAME, QRect(0, 0, -1, -1));     // hide

  // Row viewer
  addRect(PredefinedRect::FRAME_LABEL,
          QRect(0, 0, CELL_WIDTH - 4, CELL_HEIGHT));
  addRect(PredefinedRect::FRAME_HEADER,
          QRect(0, 0, FRAME_HEADER_WIDTH, CELL_HEIGHT));
  addRect(PredefinedRect::PLAY_RANGE,
          QRect(PLAY_RANGE_X, 0, PLAY_MARKER_SIZE, CELL_HEIGHT));
  addRect(PredefinedRect::ONION,
          QRect(ONION_X + (3 * ONION_DOT_SIZE - ONION_SIZE) / 2, ONION_Y,
                ONION_SIZE, ONION_SIZE)
              .adjusted(2, 1, 2, 1));
  int adjustOnion = (ONION_SIZE - ONION_DOT_SIZE) / 2;
  addRect(PredefinedRect::ONION_DOT,
          QRect(ONION_X + ONION_DOT_SIZE, ONION_Y + adjustOnion, ONION_DOT_SIZE,
                ONION_DOT_SIZE)
              .adjusted(1, 1, 1, 1));
  addRect(PredefinedRect::ONION_DOT_FIXED,
          QRect(ONION_X, ONION_Y + adjustOnion, ONION_DOT_SIZE, ONION_DOT_SIZE)
              .adjusted(1, 1, 1, 1));
  addRect(PredefinedRect::ONION_AREA,
          QRect(ONION_X, ONION_Y, PLAY_RANGE_X, CELL_HEIGHT));
  addRect(PredefinedRect::ONION_FIXED_DOT_AREA,
          QRect(ONION_X, ONION_Y, ONION_DOT_SIZE + 1, CELL_HEIGHT));
  addRect(PredefinedRect::ONION_DOT_AREA,
          QRect(ONION_X + ONION_DOT_SIZE + 1, ONION_Y, ONION_DOT_SIZE + 1,
                CELL_HEIGHT));
  addRect(PredefinedRect::PINNED_CENTER_KEY,
          QRect((FRAME_HEADER_WIDTH - PINNED_SIZE) / 2,
                (CELL_HEIGHT - PINNED_SIZE) / 2, PINNED_SIZE, PINNED_SIZE));
  addRect(
      PredefinedRect::PREVIEW_FRAME_AREA,
      QRect(PLAY_RANGE_X, 0, (FRAME_HEADER_WIDTH - PLAY_RANGE_X), CELL_HEIGHT));

  addRect(PredefinedRect::SHIFTTRACE_DOT,
          QRect(SHIFTTRACE_DOT_OFFSET, (CELL_HEIGHT - SHIFTTRACE_DOT_SIZE) / 2,
                SHIFTTRACE_DOT_SIZE, SHIFTTRACE_DOT_SIZE));
  addRect(PredefinedRect::SHIFTTRACE_DOT_AREA,
          QRect(SHIFTTRACE_DOT_OFFSET, 0, SHIFTTRACE_DOT_SIZE, CELL_HEIGHT));

  // Column viewer
  addRect(PredefinedRect::LAYER_HEADER,
          QRect(0, 1, CELL_WIDTH, use_header_height - 3));
  addRect(
      PredefinedRect::FOLDED_LAYER_HEADER,
      QRect(0, 1, FOLDED_LAYER_HEADER_WIDTH, user_folded_header_height - 3));
  addRect(PredefinedRect::CAMERA_LAYER_HEADER,
          QRect(0, 1, CAMERA_CELL_WIDTH, use_header_height - 3));

  static int THUMBNAIL_HEIGHT;
  static int HDRROW_HEIGHT;
  static int HDRROW_HEIGHT2;
  static int INDENT;
  static int HDRROW1;
  static int HDRROW2;
  static int HDRROW3;
  static int HDRROW4;
  static int HDRROW5;
  QRect layername, eyeArea, previewArea, lockArea, cameraLockArea, configArea,
      cameraConfigArea, thumbnailArea, thumbnail, cameraIconArea, pegbarname,
      volumeArea;
  QPoint soundTopLeft;

  if (layout == QString("Compact")) {
    THUMBNAIL_HEIGHT = 44;
    HDRROW_HEIGHT    = CELL_HEIGHT - 2;
    INDENT           = 0;
    HDRROW1          = 1;                        // Name, number
    HDRROW2          = HDRROW1 + HDRROW_HEIGHT;  // eye, preview, lock, config
    HDRROW3          = HDRROW2 + HDRROW_HEIGHT;  // thumbnail
    HDRROW4          = HDRROW3 + THUMBNAIL_HEIGHT;  // pegbar, parent handle

    addRect(PredefinedRect::DRAG_LAYER, QRect(0, 0, -1, -1));

    addRect(PredefinedRect::RENAME_COLUMN,
            QRect(0, HDRROW1, CELL_WIDTH, HDRROW_HEIGHT));

    layername =
        QRect(INDENT + 1, HDRROW1 + 1, CELL_WIDTH - 1, HDRROW_HEIGHT - 1);
    addRect(PredefinedRect::LAYER_NAME, layername);
    addRect(PredefinedRect::CAMERA_LAYER_NAME,
            QRect(INDENT + 1, HDRROW3, CAMERA_CELL_WIDTH - 1,
                  THUMBNAIL_HEIGHT + HDRROW_HEIGHT));
    addRect(PredefinedRect::LAYER_NUMBER,
            QRect(INDENT + layername.width() - 20, HDRROW1, 20, HDRROW_HEIGHT));

    eyeArea = QRect(INDENT, HDRROW2, ICON_WIDTH + 1, HDRROW_HEIGHT);  // Compact
    addRect(PredefinedRect::EYE_AREA, eyeArea);
    addRect(PredefinedRect::EYE,
            iconRect(eyeArea, ICON_WIDTH, ICON_HEIGHT - 1, 1));

    previewArea =
        QRect(INDENT + eyeArea.width(), HDRROW2, ICON_WIDTH + 1, HDRROW_HEIGHT);
    addRect(PredefinedRect::PREVIEW_LAYER_AREA, previewArea);
    addRect(PredefinedRect::PREVIEW_LAYER,
            iconRect(previewArea, ICON_WIDTH, ICON_HEIGHT - 1, 1));

    lockArea = QRect(INDENT + eyeArea.width() + previewArea.width(), HDRROW2,
                     ICON_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::LOCK_AREA, lockArea);
    addRect(PredefinedRect::LOCK,
            iconRect(lockArea, ICON_WIDTH - 1, ICON_HEIGHT - 1));
    cameraLockArea = QRect(INDENT, HDRROW1, CAMERA_CELL_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::CAMERA_LOCK_AREA, cameraLockArea);
    addRect(PredefinedRect::CAMERA_LOCK,
            iconRect(cameraLockArea, ICON_WIDTH - 1, ICON_HEIGHT - 1));

    configArea =
        QRect(INDENT + eyeArea.width() + previewArea.width() + lockArea.width(),
              HDRROW2, ICON_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::CONFIG_AREA, configArea);
    addRect(PredefinedRect::CONFIG,
            iconRect(configArea, ICON_WIDTH - 1, ICON_HEIGHT - 1));
    cameraConfigArea = QRect(INDENT, HDRROW2, CAMERA_CELL_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::CAMERA_CONFIG_AREA, cameraConfigArea);
    addRect(PredefinedRect::CAMERA_CONFIG,
            iconRect(cameraConfigArea, ICON_WIDTH - 1, ICON_HEIGHT - 1));

    thumbnailArea = QRect(INDENT, HDRROW3, CELL_WIDTH, THUMBNAIL_HEIGHT);
    addRect(PredefinedRect::THUMBNAIL_AREA, thumbnailArea);
    thumbnail = thumbnailArea.adjusted(1, 1, 0, 0);
    addRect(PredefinedRect::THUMBNAIL, thumbnail);

    addRect(PredefinedRect::CAMERA_ICON_AREA, QRect(0, 0, -1, -1));
    addRect(PredefinedRect::CAMERA_ICON, QRect(0, 0, -1, -1));

    addRect(PredefinedRect::FILTER_COLOR,
            QRect(thumbnail.right() - 14, thumbnail.top() + 3, 12, 12));

    addRect(PredefinedRect::SOUND_ICON,
            QRect(thumbnailArea.topLeft(), QSize(40, 30))
                .adjusted((thumbnailArea.width() / 2) - (40 / 2),
                          (thumbnailArea.height() / 2) - (30 / 2),
                          (thumbnailArea.width() / 2) - (40 / 2),
                          (thumbnailArea.height() / 2) - (30 / 2)));

    volumeArea =
        QRect(QPoint(thumbnailArea.left() + 3, thumbnailArea.bottom() - 16),
              QSize(TRACKLEN + 8, 14));
    addRect(PredefinedRect::VOLUME_AREA, volumeArea);

    pegbarname = QRect(INDENT, HDRROW4, CELL_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::PEGBAR_NAME, pegbarname);
    addRect(
        PredefinedRect::PARENT_HANDLE_NAME,
        QRect(INDENT + pegbarname.width() - 20, HDRROW4, 20, HDRROW_HEIGHT));

    addFlag(PredefinedFlag::DRAG_LAYER_BORDER, false);
    addFlag(PredefinedFlag::DRAG_LAYER_VISIBLE, false);
    addFlag(PredefinedFlag::LAYER_NAME_BORDER, false);
    addFlag(PredefinedFlag::LAYER_NAME_VISIBLE, true);
    addFlag(PredefinedFlag::LAYER_NUMBER_BORDER, false);
    addFlag(PredefinedFlag::LAYER_NUMBER_VISIBLE, true);
    addFlag(PredefinedFlag::EYE_AREA_BORDER, true);
    addFlag(PredefinedFlag::EYE_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::LOCK_AREA_BORDER, true);
    addFlag(PredefinedFlag::LOCK_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::PREVIEW_LAYER_AREA_BORDER, true);
    addFlag(PredefinedFlag::PREVIEW_LAYER_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::CONFIG_AREA_BORDER, true);
    addFlag(PredefinedFlag::CAMERA_CONFIG_AREA_BORDER, false);
    addFlag(PredefinedFlag::CONFIG_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::CAMERA_CONFIG_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::PEGBAR_NAME_BORDER, true);
    addFlag(PredefinedFlag::PEGBAR_NAME_VISIBLE, true);
    addFlag(PredefinedFlag::PARENT_HANDLE_NAME_BORDER, false);
    addFlag(PredefinedFlag::PARENT_HANDLE_NAME_VISIBILE, true);
    addFlag(PredefinedFlag::THUMBNAIL_AREA_BORDER, false);
    addFlag(PredefinedFlag::THUMBNAIL_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::CAMERA_ICON_VISIBLE, false);
    addFlag(PredefinedFlag::VOLUME_AREA_VERTICAL, false);
  } else if (layout == QString("Classic-revised")) {
    THUMBNAIL_HEIGHT = 44;
    HDRROW_HEIGHT    = CELL_HEIGHT - 2;
    INDENT           = 0;
    HDRROW1          = 1;                           // Name, number
    HDRROW2          = HDRROW1 + HDRROW_HEIGHT;     // eye, lock
    HDRROW3          = HDRROW2 + HDRROW_HEIGHT;     // preview, config
    HDRROW4          = HDRROW3 + HDRROW_HEIGHT;     // thumbnail
    HDRROW5          = HDRROW4 + THUMBNAIL_HEIGHT;  // pegbar, parent handle

    addRect(PredefinedRect::DRAG_LAYER,
            QRect(0, 0, -1, -1));  // hide - Theme/Compact

    addRect(PredefinedRect::RENAME_COLUMN,
            QRect(0, HDRROW1, CELL_WIDTH, HDRROW_HEIGHT));

    layername =
        QRect(INDENT + 1, HDRROW1 + 1, CELL_WIDTH - 1, HDRROW_HEIGHT - 1);
    addRect(PredefinedRect::LAYER_NAME, layername);
    addRect(PredefinedRect::CAMERA_LAYER_NAME,
            QRect(INDENT + 1, HDRROW4, CAMERA_CELL_WIDTH - 1,
                  THUMBNAIL_HEIGHT + HDRROW_HEIGHT));
    addRect(PredefinedRect::LAYER_NUMBER,
            QRect(INDENT + layername.width() - 20, HDRROW1, 20, HDRROW_HEIGHT));

    eyeArea = QRect(INDENT, HDRROW2, CELL_WIDTH - ICON_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::EYE_AREA, eyeArea);
    addRect(PredefinedRect::EYE,
            iconRect(eyeArea, ICON_WIDTH, ICON_HEIGHT - 1, 1));

    previewArea =
        QRect(INDENT, HDRROW3, CELL_WIDTH - ICON_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::PREVIEW_LAYER_AREA, previewArea);
    addRect(PredefinedRect::PREVIEW_LAYER,
            iconRect(previewArea, ICON_WIDTH, ICON_HEIGHT - 1, 1));

    lockArea =
        QRect(INDENT + eyeArea.width(), HDRROW2, ICON_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::LOCK_AREA, lockArea);
    addRect(PredefinedRect::LOCK,
            iconRect(lockArea, ICON_WIDTH - 1, ICON_HEIGHT - 1));
    cameraLockArea = QRect(INDENT, HDRROW2, CAMERA_CELL_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::CAMERA_LOCK_AREA, cameraLockArea);
    addRect(PredefinedRect::CAMERA_LOCK,
            iconRect(cameraLockArea, ICON_WIDTH - 1, ICON_HEIGHT - 1));

    configArea =
        QRect(INDENT + previewArea.width(), HDRROW3, ICON_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::CONFIG_AREA, configArea);
    addRect(PredefinedRect::CONFIG,
            iconRect(configArea, ICON_WIDTH - 1, ICON_HEIGHT - 1));
    cameraConfigArea = QRect(INDENT, HDRROW3, CAMERA_CELL_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::CAMERA_CONFIG_AREA, cameraConfigArea);
    addRect(PredefinedRect::CAMERA_CONFIG,
            iconRect(cameraConfigArea, ICON_WIDTH - 1, ICON_HEIGHT - 1));

    thumbnailArea = QRect(INDENT, HDRROW4, CELL_WIDTH, THUMBNAIL_HEIGHT);
    addRect(PredefinedRect::THUMBNAIL_AREA, thumbnailArea);
    thumbnail = thumbnailArea.adjusted(1, 1, 0, 0);
    addRect(PredefinedRect::THUMBNAIL, thumbnail);

    cameraIconArea = QRect(INDENT, HDRROW1, CAMERA_CELL_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::CAMERA_ICON_AREA, cameraIconArea);
    addRect(PredefinedRect::CAMERA_ICON,
            iconRect(cameraIconArea, ICON_WIDTH, ICON_HEIGHT));

    addRect(PredefinedRect::FILTER_COLOR,
            QRect(thumbnail.right() - 14, thumbnail.top() + 3, 12, 12));

    addRect(PredefinedRect::SOUND_ICON,
            QRect(thumbnailArea.topLeft(), QSize(40, 30))
                .adjusted((thumbnailArea.width() / 2) - (40 / 2),
                          (thumbnailArea.height() / 2) - (30 / 2),
                          (thumbnailArea.width() / 2) - (40 / 2),
                          (thumbnailArea.height() / 2) - (30 / 2)));

    volumeArea =
        QRect(QPoint(thumbnailArea.left() + 3, thumbnailArea.bottom() - 16),
              QSize(TRACKLEN + 8, 14));
    addRect(PredefinedRect::VOLUME_AREA, volumeArea);

    pegbarname = QRect(INDENT, HDRROW5, CELL_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::PEGBAR_NAME, pegbarname);
    addRect(
        PredefinedRect::PARENT_HANDLE_NAME,
        QRect(INDENT + pegbarname.width() - 20, HDRROW5, 20, HDRROW_HEIGHT));

    addFlag(PredefinedFlag::DRAG_LAYER_BORDER, false);
    addFlag(PredefinedFlag::DRAG_LAYER_VISIBLE, false);
    addFlag(PredefinedFlag::LAYER_NAME_BORDER, false);
    addFlag(PredefinedFlag::LAYER_NAME_VISIBLE, true);
    addFlag(PredefinedFlag::LAYER_NUMBER_BORDER, false);
    addFlag(PredefinedFlag::LAYER_NUMBER_VISIBLE, true);
    addFlag(PredefinedFlag::EYE_AREA_BORDER, true);
    addFlag(PredefinedFlag::EYE_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::LOCK_AREA_BORDER, true);
    addFlag(PredefinedFlag::LOCK_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::PREVIEW_LAYER_AREA_BORDER, true);
    addFlag(PredefinedFlag::PREVIEW_LAYER_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::CONFIG_AREA_BORDER, true);
    addFlag(PredefinedFlag::CAMERA_CONFIG_AREA_BORDER, false);
    addFlag(PredefinedFlag::CONFIG_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::CAMERA_CONFIG_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::PEGBAR_NAME_BORDER, true);
    addFlag(PredefinedFlag::PEGBAR_NAME_VISIBLE, true);
    addFlag(PredefinedFlag::PARENT_HANDLE_NAME_BORDER, false);
    addFlag(PredefinedFlag::PARENT_HANDLE_NAME_VISIBILE, true);
    addFlag(PredefinedFlag::THUMBNAIL_AREA_BORDER, true);
    addFlag(PredefinedFlag::THUMBNAIL_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::CAMERA_ICON_VISIBLE, true);
    addFlag(PredefinedFlag::VOLUME_AREA_VERTICAL, false);
  } else {
    THUMBNAIL_HEIGHT = 43;
    HDRROW_HEIGHT    = CELL_HEIGHT - 2;
    INDENT           = CELL_DRAG_WIDTH + 2;
    HDRROW1          = 7;                               // Name/eye
    HDRROW2          = HDRROW1 + CELL_HEIGHT;           // lock, preview
    HDRROW3          = HDRROW2 + CELL_HEIGHT;           // thumbnail
    HDRROW4          = HDRROW3 + THUMBNAIL_HEIGHT + 5;  // pegbar, parenthandle

    addRect(PredefinedRect::DRAG_LAYER,
            QRect(0, 0, CELL_DRAG_WIDTH, use_header_height - 3));

    addRect(PredefinedRect::RENAME_COLUMN,
            QRect(0, HDRROW1, CELL_WIDTH, HDRROW_HEIGHT));

    layername = QRect(INDENT + 1, HDRROW1 + 1, CELL_WIDTH - INDENT - 3,
                      HDRROW_HEIGHT - 1);
    addRect(PredefinedRect::LAYER_NAME, layername);
    addRect(PredefinedRect::CAMERA_LAYER_NAME,
            QRect(1, HDRROW3 + HDRROW_HEIGHT - 8, CAMERA_CELL_WIDTH - 1,
                  THUMBNAIL_HEIGHT + CELL_HEIGHT));
    addRect(PredefinedRect::LAYER_NUMBER, QRect(0, 0, -1, -1));

    eyeArea =
        QRect(INDENT, HDRROW1, CELL_WIDTH - INDENT - 2, HDRROW_HEIGHT - 1);
    addRect(PredefinedRect::EYE_AREA, eyeArea);
    addRect(PredefinedRect::EYE,
            eyeArea.adjusted(eyeArea.width() - ICON_WIDTH, 0, 0, 0));

    previewArea = QRect(INDENT, HDRROW2, CELL_WIDTH - INDENT - 2,
                        HDRROW_HEIGHT - 1);  // Legacy
    addRect(PredefinedRect::PREVIEW_LAYER_AREA, previewArea);
    addRect(PredefinedRect::PREVIEW_LAYER,
            previewArea.adjusted(previewArea.width() - ICON_WIDTH, 0, 0, 0));

    lockArea = QRect(INDENT, HDRROW2, ICON_WIDTH - 1, HDRROW_HEIGHT - 1);
    addRect(PredefinedRect::LOCK_AREA, lockArea);
    addRect(PredefinedRect::LOCK, lockArea);
    cameraLockArea = QRect(0, HDRROW2, CAMERA_CELL_WIDTH, HDRROW_HEIGHT - 1);
    addRect(PredefinedRect::CAMERA_LOCK_AREA, cameraLockArea);
    addRect(PredefinedRect::CAMERA_LOCK,
            iconRect(cameraLockArea, ICON_WIDTH - 1, ICON_HEIGHT - 1));

    addRect(PredefinedRect::CONFIG_AREA, QRect(0, 0, -1, -1));
    addRect(PredefinedRect::CONFIG, QRect(0, 0, -1, -1));
    cameraConfigArea =
        QRect(0, HDRROW3 - 2, CAMERA_CELL_WIDTH, HDRROW_HEIGHT - 4);
    addRect(PredefinedRect::CAMERA_CONFIG_AREA, cameraConfigArea);
    addRect(PredefinedRect::CAMERA_CONFIG,
            iconRect(cameraConfigArea, ICON_WIDTH - 1, ICON_HEIGHT - 1));

    thumbnailArea =
        QRect(INDENT - 1, HDRROW3, CELL_WIDTH - INDENT - 1, THUMBNAIL_HEIGHT);
    addRect(PredefinedRect::THUMBNAIL_AREA, thumbnailArea);
    thumbnail = thumbnailArea.adjusted(1, 1, 0, 0);
    addRect(PredefinedRect::THUMBNAIL, thumbnail);

    cameraIconArea = QRect(0, HDRROW1, CAMERA_CELL_WIDTH, HDRROW_HEIGHT);
    addRect(PredefinedRect::CAMERA_ICON_AREA, cameraIconArea);
    addRect(PredefinedRect::CAMERA_ICON,
            iconRect(cameraIconArea, ICON_WIDTH, ICON_HEIGHT));

    addRect(PredefinedRect::FILTER_COLOR,
            QRect(thumbnail.right() - 14, thumbnail.top() + 3, 12, 12));

    addRect(
        PredefinedRect::SOUND_ICON,
        QRect(thumbnailArea.topLeft(), QSize(40, 30)).adjusted(21, 19, 21, 19));

    volumeArea =
        QRect(QPoint(thumbnailArea.left() + 3, thumbnailArea.top() - 1),
              QSize(14, TRACKLEN + 8));
    addRect(PredefinedRect::VOLUME_AREA, volumeArea);

    pegbarname =
        QRect(INDENT, HDRROW4, CELL_WIDTH - INDENT - 2, HDRROW_HEIGHT - 1);
    addRect(PredefinedRect::PEGBAR_NAME, pegbarname);
    addRect(PredefinedRect::PARENT_HANDLE_NAME,
            QRect(INDENT + pegbarname.width() - 20, HDRROW4, 20,
                  HDRROW_HEIGHT - 1));

    addFlag(PredefinedFlag::DRAG_LAYER_BORDER, false);
    addFlag(PredefinedFlag::DRAG_LAYER_VISIBLE, true);
    addFlag(PredefinedFlag::LAYER_NAME_BORDER, false);
    addFlag(PredefinedFlag::LAYER_NAME_VISIBLE, true);
    addFlag(PredefinedFlag::LAYER_NUMBER_BORDER, false);
    addFlag(PredefinedFlag::LAYER_NUMBER_VISIBLE, false);
    addFlag(PredefinedFlag::EYE_AREA_BORDER, false);
    addFlag(PredefinedFlag::EYE_AREA_VISIBLE, true);
    //		  addFlag(PredefinedFlag::LOCK_AREA_BORDER, true);
    addFlag(PredefinedFlag::LOCK_AREA_BORDER, false);
    addFlag(PredefinedFlag::LOCK_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::PREVIEW_LAYER_AREA_BORDER, false);
    addFlag(PredefinedFlag::PREVIEW_LAYER_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::CONFIG_AREA_BORDER, false);
    addFlag(PredefinedFlag::CAMERA_CONFIG_AREA_BORDER, false);
    addFlag(PredefinedFlag::CONFIG_AREA_VISIBLE, false);
    addFlag(PredefinedFlag::CAMERA_CONFIG_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::PEGBAR_NAME_BORDER, false);
    addFlag(PredefinedFlag::PEGBAR_NAME_VISIBLE, true);
    addFlag(PredefinedFlag::PARENT_HANDLE_NAME_BORDER, false);
    addFlag(PredefinedFlag::PARENT_HANDLE_NAME_VISIBILE, true);
    addFlag(PredefinedFlag::THUMBNAIL_AREA_BORDER, false);
    addFlag(PredefinedFlag::THUMBNAIL_AREA_VISIBLE, true);
    addFlag(PredefinedFlag::CAMERA_ICON_VISIBLE, true);
    addFlag(PredefinedFlag::VOLUME_AREA_VERTICAL, true);
  }

  if (flag(PredefinedFlag::VOLUME_AREA_VERTICAL)) {
    soundTopLeft = QPoint(volumeArea.left() + 4, volumeArea.top() + 4);
    addRect(PredefinedRect::VOLUME_TRACK,
            QRect(soundTopLeft, QSize(3, TRACKLEN)));
  } else {
    soundTopLeft = QPoint(volumeArea.left() + 4, volumeArea.bottom() - 6);
    addRect(PredefinedRect::VOLUME_TRACK,
            QRect(soundTopLeft, QSize(TRACKLEN, 3)));
  }

  // Layer footer panel
  QRect layerFooterPanel(
      QRect(0, 0, LAYER_FOOTER_PANEL_WIDTH + 2, use_header_height));
  addRect(PredefinedRect::LAYER_FOOTER_PANEL, layerFooterPanel);

  QRect zoomSlider, zoomIn, zoomOut;

  zoomSlider = QRect(0, 17, LAYER_FOOTER_PANEL_WIDTH, use_header_height - 34);
  addRect(PredefinedRect::ZOOM_SLIDER_AREA, zoomSlider);
  addRect(PredefinedRect::ZOOM_SLIDER, zoomSlider.adjusted(0, 1, 0, 0));

  zoomIn = QRect(0, zoomSlider.bottom() + 1, LAYER_FOOTER_PANEL_WIDTH, 16);
  addRect(PredefinedRect::ZOOM_IN_AREA, zoomIn);
  addRect(PredefinedRect::ZOOM_IN, zoomIn.adjusted(1, 1, 0, 0));

  zoomOut = QRect(0, zoomSlider.top() - 17, LAYER_FOOTER_PANEL_WIDTH, 16);
  addRect(PredefinedRect::ZOOM_OUT_AREA, zoomOut);
  addRect(PredefinedRect::ZOOM_OUT, zoomOut.adjusted(1, 1, 0, 0));
  /*
  // Layer footer panel
  addRect(PredefinedRect::LAYER_FOOTER_PANEL, QRect(0, 0, -1, -1));  // hide
  addRect(PredefinedRect::ZOOM_SLIDER, QRect(0, 0, -1, -1));
  addRect(PredefinedRect::ZOOM_IN, QRect(0, 0, -1, -1));
  addRect(PredefinedRect::ZOOM_OUT, QRect(0, 0, -1, -1));
  */
  //
  // Lines
  //
  addLine(PredefinedLine::LOCKED,
          verticalLine((CELL_DRAG_WIDTH + 1) / 2, NumberRange(0, CELL_HEIGHT)));
  addLine(PredefinedLine::SEE_MARKER_THROUGH,
          horizontalLine(0, NumberRange(0, CELL_DRAG_WIDTH)));
  addLine(PredefinedLine::CONTINUE_LEVEL,
          verticalLine(CELL_WIDTH / 2, NumberRange(0, CELL_HEIGHT)));
  addLine(PredefinedLine::CONTINUE_LEVEL_WITH_NAME,
          verticalLine(CELL_WIDTH - 14, NumberRange(0, CELL_HEIGHT)));
  addLine(PredefinedLine::EXTENDER_LINE,
          horizontalLine(0, NumberRange(-EXTENDER_WIDTH - KEY_ICON_WIDTH, 0)));

  //
  // Dimensions
  //
  addDimension(PredefinedDimension::LAYER, CELL_WIDTH);
  addDimension(PredefinedDimension::FRAME, CELL_HEIGHT);
  addDimension(PredefinedDimension::INDEX, 0);
  addDimension(PredefinedDimension::SOUND_AMPLITUDE,
               int(sqrt(CELL_HEIGHT * soundRect.width()) / 2));
  addDimension(PredefinedDimension::FRAME_LABEL_ALIGN,
               Qt::AlignVCenter | Qt::AlignRight);
  addDimension(PredefinedDimension::ONION_TURN, 0);
  addDimension(PredefinedDimension::QBOXLAYOUT_DIRECTION,
               QBoxLayout::Direction::TopToBottom);
  addDimension(PredefinedDimension::CENTER_ALIGN, Qt::AlignHCenter);
  addDimension(PredefinedDimension::CAMERA_LAYER, CAMERA_CELL_WIDTH);
  addDimension(PredefinedDimension::SCALE_THRESHOLD, 57);

  //
  // Paths
  //
  QPainterPath corner(QPointF(0, CELL_HEIGHT));
  corner.lineTo(QPointF(CELL_DRAG_WIDTH, CELL_HEIGHT));
  corner.lineTo(QPointF(CELL_DRAG_WIDTH, CELL_HEIGHT - CELL_DRAG_WIDTH));
  corner.lineTo(QPointF(0, CELL_HEIGHT));
  addPath(PredefinedPath::DRAG_HANDLE_CORNER, corner);

  QPainterPath diamond(QPointF(0, -4));
  diamond.lineTo(4, 0);
  diamond.lineTo(0, 4);
  diamond.lineTo(-4, 0);
  diamond.lineTo(0, -4);
  addPath(PredefinedPath::FRAME_MARKER_DIAMOND, diamond);

  QPainterPath fromTriangle(QPointF(0, EASE_TRIANGLE_SIZE / 2));
  fromTriangle.lineTo(QPointF(EASE_TRIANGLE_SIZE, -EASE_TRIANGLE_SIZE / 2));
  fromTriangle.lineTo(QPointF(-EASE_TRIANGLE_SIZE, -EASE_TRIANGLE_SIZE / 2));
  fromTriangle.lineTo(QPointF(0, EASE_TRIANGLE_SIZE / 2));
  addPath(PredefinedPath::BEGIN_EASE_TRIANGLE, fromTriangle);

  QPainterPath toTriangle(QPointF(0, -EASE_TRIANGLE_SIZE / 2));
  toTriangle.lineTo(QPointF(EASE_TRIANGLE_SIZE, EASE_TRIANGLE_SIZE / 2));
  toTriangle.lineTo(QPointF(-EASE_TRIANGLE_SIZE, EASE_TRIANGLE_SIZE / 2));
  toTriangle.lineTo(QPointF(0, -EASE_TRIANGLE_SIZE / 2));
  addPath(PredefinedPath::END_EASE_TRIANGLE, toTriangle);

  QPainterPath playFrom(QPointF(0, 0));
  playFrom.lineTo(QPointF(PLAY_MARKER_SIZE, 0));
  playFrom.lineTo(QPointF(0, PLAY_MARKER_SIZE));
  playFrom.lineTo(QPointF(0, 0));
  playFrom.translate(PLAY_RANGE_X, 1);
  addPath(PredefinedPath::BEGIN_PLAY_RANGE, playFrom);

  QPainterPath playTo(QPointF(0, 0));
  playTo.lineTo(QPointF(PLAY_MARKER_SIZE, 0));
  playTo.lineTo(QPointF(0, -PLAY_MARKER_SIZE));
  playTo.lineTo(QPointF(0, 0));
  playTo.translate(PLAY_RANGE_X, CELL_HEIGHT - 1);
  addPath(PredefinedPath::END_PLAY_RANGE, playTo);

  QPainterPath track(QPointF(0, 0));
  if (flag(PredefinedFlag::VOLUME_AREA_VERTICAL)) {
    track.lineTo(QPointF(1, 1));
    track.lineTo(QPointF(1, TRACKLEN - 1));
    track.lineTo(QPointF(0, TRACKLEN));
    track.lineTo(QPointF(-1, TRACKLEN - 1));
    track.lineTo(QPointF(-1, 1));
    track.lineTo(QPointF(0, 0));
    track.translate(soundTopLeft + QPoint(3, 0));
  } else {
    track.lineTo(QPointF(1, 1));
    track.lineTo(QPointF(TRACKLEN - 1, 1));
    track.lineTo(QPointF(TRACKLEN, 0));
    track.lineTo(QPointF(TRACKLEN - 1, -1));
    track.lineTo(QPointF(1, -1));
    track.lineTo(QPointF(0, 0));
    track.translate(soundTopLeft);
  }
  addPath(PredefinedPath::VOLUME_SLIDER_TRACK, track);

  QPainterPath head(QPointF(0, 0));
  if (flag(PredefinedFlag::VOLUME_AREA_VERTICAL)) {
    head.lineTo(QPointF(4, 4));
    head.lineTo(QPointF(8, 4));
    head.lineTo(QPointF(8, -4));
    head.lineTo(QPointF(4, -4));
    head.lineTo(QPointF(0, 0));
  } else {
    head.lineTo(QPointF(4, -4));
    head.lineTo(QPointF(4, -8));
    head.lineTo(QPointF(-4, -8));
    head.lineTo(QPointF(-4, -4));
    head.lineTo(QPointF(0, 0));
  }
  addPath(PredefinedPath::VOLUME_SLIDER_HEAD, head);

  //
  // Points
  //
  addPoint(PredefinedPoint::KEY_HIDDEN, QPoint(KEY_ICON_WIDTH, 0));
  addPoint(PredefinedPoint::EXTENDER_XY_RADIUS, QPoint(30, 75));

  if (flag(PredefinedFlag::VOLUME_AREA_VERTICAL))
    addPoint(PredefinedPoint::VOLUME_DIVISIONS_TOP_LEFT, soundTopLeft);
  else
    addPoint(PredefinedPoint::VOLUME_DIVISIONS_TOP_LEFT,
             soundTopLeft + QPoint(0, 3));

  //
  // Ranges
  //
  addRange(PredefinedRange::HEADER_LAYER, NumberRange(0, FRAME_HEADER_WIDTH));
  addRange(PredefinedRange::HEADER_FRAME, NumberRange(0, use_header_height));
}

CellPosition TopToBottomOrientation::xyToPosition(const QPoint &xy,
                                                  const ColumnFan *fan) const {
  int layer = fan->layerAxisToCol(xy.x());
  int frame = xy.y() / CELL_HEIGHT;
  return CellPosition(frame, layer);
}
QPoint TopToBottomOrientation::positionToXY(const CellPosition &position,
                                            const ColumnFan *fan) const {
  int x = colToLayerAxis(position.layer(), fan);
  int y = rowToFrameAxis(position.frame());
  return QPoint(x, y);
}
CellPositionRatio TopToBottomOrientation::xyToPositionRatio(
    const QPointF &xy) const {
  double frame = xy.y() / (double)CELL_HEIGHT;
  double layer = xy.x() / (double)CELL_WIDTH;
  return CellPositionRatio{frame, layer};
}
QPointF TopToBottomOrientation::positionRatioToXY(
    const CellPositionRatio &ratio) const {
  double x = ratio.layer() * (double)CELL_WIDTH;
  double y = ratio.frame() * (double)CELL_HEIGHT;
  return QPointF(x, y);
}

int TopToBottomOrientation::colToLayerAxis(int layer,
                                           const ColumnFan *fan) const {
  return fan->colToLayerAxis(layer);
}
int TopToBottomOrientation::rowToFrameAxis(int frame) const {
  return frame * CELL_HEIGHT;
}
QPoint TopToBottomOrientation::frameLayerToXY(int frameAxis,
                                              int layerAxis) const {
  return QPoint(layerAxis, frameAxis);
}
int TopToBottomOrientation::layerAxis(const QPoint &xy) const { return xy.x(); }
int TopToBottomOrientation::frameAxis(const QPoint &xy) const { return xy.y(); }
NumberRange TopToBottomOrientation::layerSide(const QRect &area) const {
  return NumberRange(area.left(), area.right());
}
NumberRange TopToBottomOrientation::frameSide(const QRect &area) const {
  return NumberRange(area.top(), area.bottom());
}
QPoint TopToBottomOrientation::topRightCorner(const QRect &area) const {
  return area.topRight();
}

CellPosition TopToBottomOrientation::arrowShift(int direction) const {
  switch (direction) {
  case Qt::Key_Up:
    return CellPosition(-1, 0);
  case Qt::Key_Down:
    return CellPosition(1, 0);
  case Qt::Key_Left:
    return CellPosition(0, -1);
  case Qt::Key_Right:
    return CellPosition(0, 1);
  default:
    return CellPosition(0, 0);
  }
}

/// --------------------------------------------------------------------------------

LeftToRightOrientation::LeftToRightOrientation() {
  //
  // Ranges
  //

  // Cell viewer
  QRect cellRect(0, 0, CELL_WIDTH, CELL_HEIGHT);
  addRect(PredefinedRect::CELL, cellRect);
  addRect(PredefinedRect::CAMERA_CELL, cellRect);
  addRect(PredefinedRect::DRAG_HANDLE_CORNER,
          QRect(0, 0, CELL_WIDTH, CELL_DRAG_HEIGHT));
  QRect keyRect((CELL_WIDTH - KEY_ICON_WIDTH) / 2,
                CELL_HEIGHT - KEY_ICON_HEIGHT - 2, KEY_ICON_WIDTH,
                KEY_ICON_HEIGHT);
  addRect(PredefinedRect::KEY_ICON, keyRect);
  addRect(PredefinedRect::CAMERA_KEY_ICON, keyRect);
  QRect nameRect = cellRect.adjusted(4, 4, -6, 0);
  addRect(PredefinedRect::CELL_NAME, nameRect);
  addRect(PredefinedRect::CELL_NAME_WITH_KEYFRAME, nameRect);
  addRect(PredefinedRect::END_EXTENDER,
          QRect(0, -EXTENDER_HEIGHT - 14, EXTENDER_WIDTH, EXTENDER_HEIGHT));
  addRect(PredefinedRect::BEGIN_EXTENDER,
          QRect(-EXTENDER_WIDTH, -EXTENDER_HEIGHT - 14, EXTENDER_WIDTH,
                EXTENDER_HEIGHT));
  addRect(PredefinedRect::KEYFRAME_AREA, keyRect);
  addRect(PredefinedRect::DRAG_AREA, QRect(0, 0, CELL_WIDTH, CELL_DRAG_HEIGHT));
  QRect soundRect(0, CELL_DRAG_HEIGHT, CELL_WIDTH,
                  CELL_HEIGHT - CELL_DRAG_HEIGHT - SOUND_PREVIEW_HEIGHT);
  addRect(PredefinedRect::SOUND_TRACK, soundRect);
  addRect(PredefinedRect::PREVIEW_TRACK,
          QRect(0, CELL_HEIGHT - SOUND_PREVIEW_HEIGHT + 1, CELL_WIDTH,
                SOUND_PREVIEW_HEIGHT));
  addRect(PredefinedRect::BEGIN_SOUND_EDIT,
          QRect(0, CELL_DRAG_HEIGHT, 2, CELL_HEIGHT - CELL_DRAG_HEIGHT));
  addRect(PredefinedRect::END_SOUND_EDIT,
          QRect(CELL_WIDTH - 2, CELL_DRAG_HEIGHT, 2,
                CELL_HEIGHT - CELL_DRAG_HEIGHT));
  addRect(PredefinedRect::LOOP_ICON, QRect(0, keyRect.top(), 10, 11));
  addRect(PredefinedRect::CAMERA_LOOP_ICON, rect(PredefinedRect::LOOP_ICON));
  QRect frameMarker((CELL_WIDTH - FRAME_MARKER_SIZE) / 2 - 1,
                    CELL_HEIGHT - FRAME_MARKER_SIZE - 7, FRAME_MARKER_SIZE,
                    FRAME_MARKER_SIZE);
  addRect(PredefinedRect::FRAME_MARKER_AREA, frameMarker);
  addRect(PredefinedRect::CAMERA_FRAME_MARKER_AREA,
          rect(PredefinedRect::FRAME_MARKER_AREA));

  // Notes viewer
  addRect(
      PredefinedRect::NOTE_AREA,
      QRect(QPoint(0, 0), QSize(LAYER_HEADER_WIDTH - 1, FRAME_HEADER_HEIGHT)));
  addRect(PredefinedRect::NOTE_ICON,
          QRect(QPoint(0, 0), QSize(CELL_WIDTH - 2, CELL_HEIGHT - 2)));

  // Layer header panel
  addRect(PredefinedRect::LAYER_HEADER_PANEL,
          QRect(0, FRAME_HEADER_HEIGHT - CELL_HEIGHT, LAYER_HEADER_WIDTH,
                LAYER_HEADER_PANEL_HEIGHT));
  QRect panelEye(1, 0, ICON_WIDTH, ICON_HEIGHT);
  addRect(PredefinedRect::PANEL_EYE, panelEye.adjusted(1, 1, -1, -1));
  addRect(PredefinedRect::PANEL_PREVIEW_LAYER,
          panelEye.translated(ICON_OFFSET, 0).adjusted(1, 1, -1, -1));
  addRect(PredefinedRect::PANEL_LOCK,
          panelEye.translated(2 * ICON_OFFSET, 0).adjusted(1, 1, -1, -1));
  QRect panelName(ICONS_WIDTH + THUMBNAIL_WIDTH + 1, 0,
                  LAYER_NAME_WIDTH + LAYER_NUMBER_WIDTH - 4,
                  LAYER_HEADER_PANEL_HEIGHT);
  addRect(PredefinedRect::PANEL_LAYER_NAME, panelName);

  // Row viewer
  addRect(PredefinedRect::FRAME_LABEL,
          QRect(CELL_WIDTH / 4, 1, CELL_WIDTH / 2, FRAME_HEADER_HEIGHT - 2));
  addRect(PredefinedRect::FRAME_HEADER,
          QRect(0, 0, CELL_WIDTH, FRAME_HEADER_HEIGHT));
  addRect(PredefinedRect::PLAY_RANGE,
          QRect(0, PLAY_RANGE_Y, CELL_WIDTH, PLAY_MARKER_SIZE));
  addRect(PredefinedRect::ONION,
          QRect(ONION_X + (CELL_WIDTH - ONION_SIZE) / 2 - 1,
                ONION_Y + (3 * ONION_DOT_SIZE - ONION_SIZE) / 2, ONION_SIZE,
                ONION_SIZE)
              .adjusted(1, 2, 1, 2));
  int adjustOnion = (ONION_SIZE - ONION_DOT_SIZE) / 2;
  addRect(PredefinedRect::ONION_DOT,
          QRect(ONION_X + adjustOnion + (CELL_WIDTH - ONION_SIZE) / 2,
                ONION_Y + ONION_DOT_SIZE, ONION_DOT_SIZE, ONION_DOT_SIZE)
              .adjusted(1, 1, 1, 1));
  addRect(PredefinedRect::ONION_DOT_FIXED,
          QRect(ONION_X + adjustOnion + (CELL_WIDTH - ONION_SIZE) / 2, ONION_Y,
                ONION_DOT_SIZE, ONION_DOT_SIZE)
              .adjusted(1, 1, 1, 1));
  addRect(PredefinedRect::ONION_AREA,
          QRect(ONION_X, ONION_Y, CELL_WIDTH, ONION_SIZE));
  addRect(PredefinedRect::ONION_FIXED_DOT_AREA,
          QRect(ONION_X, ONION_Y, CELL_WIDTH, ONION_DOT_SIZE + 1));
  addRect(PredefinedRect::ONION_DOT_AREA,
          QRect(ONION_X, ONION_Y + ONION_DOT_SIZE + 1, CELL_WIDTH,
                ONION_DOT_SIZE + 1));
  addRect(
      PredefinedRect::PINNED_CENTER_KEY,
      QRect((CELL_WIDTH - PINNED_SIZE) / 2,
            (FRAME_HEADER_HEIGHT - PINNED_SIZE) / 2, PINNED_SIZE, PINNED_SIZE));
  addRect(
      PredefinedRect::PREVIEW_FRAME_AREA,
      QRect(0, PLAY_RANGE_Y, CELL_WIDTH, (FRAME_HEADER_HEIGHT - PLAY_RANGE_Y)));

  addRect(PredefinedRect::SHIFTTRACE_DOT,
          QRect((CELL_WIDTH - SHIFTTRACE_DOT_SIZE) / 2, SHIFTTRACE_DOT_OFFSET,
                SHIFTTRACE_DOT_SIZE, SHIFTTRACE_DOT_SIZE)
              .adjusted(-1, 0, -1, 0));
  addRect(PredefinedRect::SHIFTTRACE_DOT_AREA,
          QRect(0, SHIFTTRACE_DOT_OFFSET, CELL_WIDTH, SHIFTTRACE_DOT_SIZE));

  // Column viewer
  addRect(PredefinedRect::LAYER_HEADER,
          QRect(1, 0, LAYER_HEADER_WIDTH - 2, CELL_HEIGHT));
  addRect(
      PredefinedRect::FOLDED_LAYER_HEADER,
      QRect(1, 0, FOLDED_LAYER_HEADER_WIDTH - 2, FOLDED_LAYER_HEADER_HEIGHT));
  addRect(PredefinedRect::CAMERA_LAYER_HEADER,
          QRect(1, 0, LAYER_HEADER_WIDTH - 2, CAMERA_CELL_HEIGHT));
  QRect columnName(ICONS_WIDTH + THUMBNAIL_WIDTH + 1, 0,
                   LAYER_NAME_WIDTH + LAYER_NUMBER_WIDTH - 4, CELL_HEIGHT);
  addRect(PredefinedRect::RENAME_COLUMN, columnName);
  QRect eyeArea(1, 0, ICON_WIDTH, CELL_HEIGHT);
  QRect eye(1,
            eyeArea.top() + ((eyeArea.height() / 2) - ((ICON_HEIGHT - 1) / 2)),
            ICON_WIDTH, ICON_HEIGHT);
  addRect(PredefinedRect::EYE_AREA, eyeArea);
  addRect(PredefinedRect::EYE, eye.adjusted(1, 1, -1, -1));
  addRect(PredefinedRect::PREVIEW_LAYER_AREA,
          eyeArea.translated(ICON_OFFSET, 0));
  addRect(PredefinedRect::PREVIEW_LAYER,
          eye.translated(ICON_OFFSET, 0).adjusted(1, 1, -1, -1));
  addRect(PredefinedRect::LOCK_AREA, eyeArea.translated(2 * ICON_OFFSET, 0));
  addRect(PredefinedRect::LOCK,
          eye.translated(2 * ICON_OFFSET, 0).adjusted(1, 1, -1, -1));
  addRect(PredefinedRect::CAMERA_LOCK_AREA, rect(PredefinedRect::LOCK_AREA));
  addRect(PredefinedRect::CAMERA_LOCK, rect(PredefinedRect::LOCK));
  addRect(PredefinedRect::CONFIG_AREA, eyeArea.translated(3 * ICON_OFFSET, 0));
  addRect(PredefinedRect::CONFIG,
          eye.translated(3 * ICON_OFFSET, 0).adjusted(1, 1, -1, -1));
  addRect(PredefinedRect::CAMERA_CONFIG_AREA,
          rect(PredefinedRect::CONFIG_AREA));
  addRect(PredefinedRect::CAMERA_CONFIG, rect(PredefinedRect::CONFIG));
  addRect(PredefinedRect::DRAG_LAYER,
          QRect(ICONS_WIDTH + THUMBNAIL_WIDTH + 1, 0,
                LAYER_HEADER_WIDTH - ICONS_WIDTH - THUMBNAIL_WIDTH - 3,
                CELL_DRAG_HEIGHT));
  addRect(PredefinedRect::LAYER_NAME, columnName);
  addRect(PredefinedRect::CAMERA_LAYER_NAME, rect(PredefinedRect::LAYER_NAME));
  addRect(PredefinedRect::LAYER_NUMBER,
          QRect(ICONS_WIDTH + THUMBNAIL_WIDTH + 1, 0, LAYER_NUMBER_WIDTH,
                CELL_HEIGHT));
  QRect thumbnailArea = QRect(ICONS_WIDTH + 1, 0, THUMBNAIL_WIDTH, CELL_HEIGHT);
  addRect(PredefinedRect::THUMBNAIL_AREA, thumbnailArea);
  QRect thumbnail = thumbnailArea.adjusted(1, 1, 0, 0);
  addRect(PredefinedRect::THUMBNAIL, thumbnail);
  addRect(PredefinedRect::CAMERA_ICON_AREA,
          rect(PredefinedRect::THUMBNAIL_AREA));
  addRect(PredefinedRect::CAMERA_ICON, rect(PredefinedRect::THUMBNAIL));

  addRect(PredefinedRect::FILTER_COLOR,
          QRect(thumbnail.right() - 14, thumbnail.top() + 3, 12, 12));
  addRect(PredefinedRect::PEGBAR_NAME, QRect(0, 0, -1, -1));         // hide
  addRect(PredefinedRect::PARENT_HANDLE_NAME, QRect(0, 0, -1, -1));  // hide

  addRect(PredefinedRect::SOUND_ICON,
          QRect(thumbnailArea.topLeft(), QSize(27, thumbnailArea.height()))
              .adjusted((thumbnailArea.width() / 2) - (27 / 2), 0,
                        (thumbnailArea.width() / 2) - (27 / 2), 0));

  QRect volumeArea(
      QRect(columnName.topRight(), QSize(TRACKLEN + 8, 14))
          .adjusted(-77, CELL_DRAG_HEIGHT + 1, -77, CELL_DRAG_HEIGHT + 1));
  addRect(PredefinedRect::VOLUME_AREA, volumeArea);
  QPoint soundTopLeft(volumeArea.left() + 4, volumeArea.bottom() - 6);
  addRect(PredefinedRect::VOLUME_TRACK,
          QRect(soundTopLeft, QSize(TRACKLEN, 3)));

  // Layer footer panel
  QRect layerFooterPanel(
      QRect(0, 0, LAYER_HEADER_WIDTH + 2, LAYER_FOOTER_PANEL_HEIGHT));
  addRect(PredefinedRect::LAYER_FOOTER_PANEL, layerFooterPanel);

  QRect zoomSlider, zoomIn, zoomOut;

  zoomSlider =
      QRect(layerFooterPanel.width() - 100, 0, 81, LAYER_FOOTER_PANEL_HEIGHT);
  addRect(PredefinedRect::ZOOM_SLIDER_AREA, zoomSlider);
  addRect(PredefinedRect::ZOOM_SLIDER, zoomSlider.adjusted(1, 0, 0, 0));

  zoomIn = QRect(zoomSlider.right() + 1, 0, 16, LAYER_FOOTER_PANEL_HEIGHT);
  addRect(PredefinedRect::ZOOM_IN_AREA, zoomIn);
  addRect(PredefinedRect::ZOOM_IN, zoomIn.adjusted(1, 1, 0, 0));

  zoomOut = QRect(zoomSlider.left() - 16, 0, 16, LAYER_FOOTER_PANEL_HEIGHT);
  addRect(PredefinedRect::ZOOM_OUT_AREA, zoomOut);
  addRect(PredefinedRect::ZOOM_OUT, zoomOut.adjusted(1, 1, 0, 0));

  // Flags
  addFlag(PredefinedFlag::DRAG_LAYER_BORDER, false);
  addFlag(PredefinedFlag::DRAG_LAYER_VISIBLE, true);
  addFlag(PredefinedFlag::LAYER_NAME_BORDER, true);
  addFlag(PredefinedFlag::LAYER_NAME_VISIBLE, true);
  addFlag(PredefinedFlag::LAYER_NUMBER_BORDER, true);
  addFlag(PredefinedFlag::LAYER_NUMBER_VISIBLE, true);
  addFlag(PredefinedFlag::EYE_AREA_BORDER, true);
  addFlag(PredefinedFlag::EYE_AREA_VISIBLE, true);
  addFlag(PredefinedFlag::LOCK_AREA_BORDER, true);
  addFlag(PredefinedFlag::LOCK_AREA_VISIBLE, true);
  addFlag(PredefinedFlag::PREVIEW_LAYER_AREA_BORDER, true);
  addFlag(PredefinedFlag::PREVIEW_LAYER_AREA_VISIBLE, true);
  addFlag(PredefinedFlag::CONFIG_AREA_BORDER, true);
  addFlag(PredefinedFlag::CAMERA_CONFIG_AREA_BORDER, true);
  addFlag(PredefinedFlag::CONFIG_AREA_VISIBLE, true);
  addFlag(PredefinedFlag::CAMERA_CONFIG_AREA_VISIBLE, true);
  addFlag(PredefinedFlag::PEGBAR_NAME_BORDER, false);
  addFlag(PredefinedFlag::PEGBAR_NAME_VISIBLE, false);
  addFlag(PredefinedFlag::PARENT_HANDLE_NAME_BORDER, false);
  addFlag(PredefinedFlag::PARENT_HANDLE_NAME_VISIBILE, false);
  addFlag(PredefinedFlag::THUMBNAIL_AREA_BORDER, true);
  addFlag(PredefinedFlag::THUMBNAIL_AREA_VISIBLE, true);
  addFlag(PredefinedFlag::CAMERA_ICON_VISIBLE, true);
  addFlag(PredefinedFlag::VOLUME_AREA_VERTICAL, false);

  //
  // Lines
  //
  addLine(PredefinedLine::LOCKED,
          verticalLine((CELL_DRAG_HEIGHT + 1) / 2, NumberRange(0, CELL_WIDTH)));
  addLine(PredefinedLine::SEE_MARKER_THROUGH,
          horizontalLine(0, NumberRange(0, CELL_DRAG_HEIGHT)));
  addLine(PredefinedLine::CONTINUE_LEVEL,
          verticalLine(CELL_HEIGHT / 2, NumberRange(0, CELL_WIDTH)));
  addLine(PredefinedLine::CONTINUE_LEVEL_WITH_NAME,
          verticalLine(CELL_HEIGHT / 2, NumberRange(0, CELL_WIDTH)));
  addLine(PredefinedLine::EXTENDER_LINE,
          horizontalLine(0, NumberRange(-EXTENDER_WIDTH - KEY_ICON_WIDTH, 0)));

  //
  // Dimensions
  //
  addDimension(PredefinedDimension::LAYER, CELL_HEIGHT);
  addDimension(PredefinedDimension::FRAME, CELL_WIDTH);
  addDimension(PredefinedDimension::INDEX, 1);
  addDimension(PredefinedDimension::SOUND_AMPLITUDE, soundRect.height() / 2);
  addDimension(PredefinedDimension::FRAME_LABEL_ALIGN,
               Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap);
  addDimension(PredefinedDimension::ONION_TURN, 90);
  addDimension(PredefinedDimension::QBOXLAYOUT_DIRECTION,
               QBoxLayout::Direction::LeftToRight);
  addDimension(PredefinedDimension::CENTER_ALIGN, Qt::AlignVCenter);
  addDimension(PredefinedDimension::CAMERA_LAYER, CAMERA_CELL_HEIGHT);
  addDimension(PredefinedDimension::SCALE_THRESHOLD, 50);

  //
  // Paths
  //
  QPainterPath corner(QPointF(CELL_WIDTH, 0));
  corner.lineTo(QPointF(CELL_WIDTH, CELL_DRAG_HEIGHT));
  corner.lineTo(QPointF(CELL_WIDTH - CELL_DRAG_HEIGHT, CELL_DRAG_HEIGHT));
  corner.lineTo(QPointF(CELL_WIDTH, 0));
  addPath(PredefinedPath::DRAG_HANDLE_CORNER, corner);

  QPainterPath diamond(QPointF(0, -4));
  diamond.lineTo(4, 0);
  diamond.lineTo(0, 4);
  diamond.lineTo(-4, 0);
  diamond.lineTo(0, -4);
  addPath(PredefinedPath::FRAME_MARKER_DIAMOND, diamond);

  QPainterPath fromTriangle(QPointF(EASE_TRIANGLE_SIZE / 2, 0));
  fromTriangle.lineTo(QPointF(-EASE_TRIANGLE_SIZE / 2, EASE_TRIANGLE_SIZE));
  fromTriangle.lineTo(QPointF(-EASE_TRIANGLE_SIZE / 2, -EASE_TRIANGLE_SIZE));
  fromTriangle.lineTo(QPointF(EASE_TRIANGLE_SIZE / 2, 0));
  addPath(PredefinedPath::BEGIN_EASE_TRIANGLE, fromTriangle);

  QPainterPath toTriangle(QPointF(-EASE_TRIANGLE_SIZE / 2, 0));
  toTriangle.lineTo(QPointF(EASE_TRIANGLE_SIZE / 2, EASE_TRIANGLE_SIZE));
  toTriangle.lineTo(QPointF(EASE_TRIANGLE_SIZE / 2, -EASE_TRIANGLE_SIZE));
  toTriangle.lineTo(QPointF(-EASE_TRIANGLE_SIZE / 2, 0));
  addPath(PredefinedPath::END_EASE_TRIANGLE, toTriangle);

  QPainterPath playFrom(QPointF(0, 0));
  playFrom.lineTo(QPointF(PLAY_MARKER_SIZE, 0));
  playFrom.lineTo(QPointF(0, PLAY_MARKER_SIZE));
  playFrom.lineTo(QPointF(0, 0));
  playFrom.translate(1, PLAY_RANGE_Y);
  addPath(PredefinedPath::BEGIN_PLAY_RANGE, playFrom);

  QPainterPath playTo(QPointF(0, 0));
  playTo.lineTo(QPointF(-PLAY_MARKER_SIZE, 0));
  playTo.lineTo(QPointF(0, PLAY_MARKER_SIZE));
  playTo.lineTo(QPointF(0, 0));
  playTo.translate(CELL_WIDTH - 2, PLAY_RANGE_Y);
  addPath(PredefinedPath::END_PLAY_RANGE, playTo);

  QPainterPath track(QPointF(0, 0));
  track.lineTo(QPointF(1, 1));
  track.lineTo(QPointF(TRACKLEN - 1, 1));
  track.lineTo(QPointF(TRACKLEN, 0));
  track.lineTo(QPointF(TRACKLEN - 1, -1));
  track.lineTo(QPointF(1, -1));
  track.lineTo(QPointF(0, 0));
  track.translate(soundTopLeft);
  addPath(PredefinedPath::VOLUME_SLIDER_TRACK, track);

  QPainterPath head(QPointF(0, 0));
  head.lineTo(QPointF(4, -4));
  head.lineTo(QPointF(4, -8));
  head.lineTo(QPointF(-4, -8));
  head.lineTo(QPointF(-4, -4));
  head.lineTo(QPointF(0, 0));
  addPath(PredefinedPath::VOLUME_SLIDER_HEAD, head);

  QPainterPath timeIndicator(QPointF(0, 0));
  timeIndicator.lineTo(QPointF(-9, -5));
  timeIndicator.lineTo(QPointF(-9, -18));
  timeIndicator.lineTo(QPointF(9, -18));
  timeIndicator.lineTo(QPointF(9, -4));
  timeIndicator.lineTo(QPointF(0, 0));
  addPath(PredefinedPath::TIME_INDICATOR_HEAD, timeIndicator);

  //
  // Points
  //
  addPoint(PredefinedPoint::KEY_HIDDEN, QPoint(0, 10));
  addPoint(PredefinedPoint::EXTENDER_XY_RADIUS, QPoint(75, 30));
  addPoint(PredefinedPoint::VOLUME_DIVISIONS_TOP_LEFT,
           soundTopLeft + QPoint(0, 3));

  //
  // Ranges
  //
  addRange(PredefinedRange::HEADER_LAYER, NumberRange(0, FRAME_HEADER_HEIGHT));
  addRange(PredefinedRange::HEADER_FRAME, NumberRange(0, LAYER_HEADER_WIDTH));
}

CellPosition LeftToRightOrientation::xyToPosition(const QPoint &xy,
                                                  const ColumnFan *fan) const {
  int layer = fan->layerAxisToCol(xy.y());
  int frame = xy.x() / CELL_WIDTH;
  return CellPosition(frame, layer);
}
QPoint LeftToRightOrientation::positionToXY(const CellPosition &position,
                                            const ColumnFan *fan) const {
  int y = colToLayerAxis(position.layer(), fan);
  int x = rowToFrameAxis(position.frame());
  return QPoint(x, y);
}
CellPositionRatio LeftToRightOrientation::xyToPositionRatio(
    const QPointF &xy) const {
  double frame = xy.x() / (double)CELL_WIDTH;
  double layer = xy.y() / (double)CELL_HEIGHT;
  return CellPositionRatio{frame, layer};
}
QPointF LeftToRightOrientation::positionRatioToXY(
    const CellPositionRatio &ratio) const {
  double x = ratio.frame() * (double)CELL_WIDTH;
  double y = ratio.layer() * (double)CELL_HEIGHT;
  return QPointF(x, y);
}

int LeftToRightOrientation::colToLayerAxis(int layer,
                                           const ColumnFan *fan) const {
  return fan->colToLayerAxis(layer);
}
int LeftToRightOrientation::rowToFrameAxis(int frame) const {
  return frame * CELL_WIDTH;
}
QPoint LeftToRightOrientation::frameLayerToXY(int frameAxis,
                                              int layerAxis) const {
  return QPoint(frameAxis, layerAxis);
}
int LeftToRightOrientation::layerAxis(const QPoint &xy) const { return xy.y(); }
int LeftToRightOrientation::frameAxis(const QPoint &xy) const { return xy.x(); }
NumberRange LeftToRightOrientation::layerSide(const QRect &area) const {
  return NumberRange(area.top(), area.bottom());
}
NumberRange LeftToRightOrientation::frameSide(const QRect &area) const {
  return NumberRange(area.left(), area.right());
}
QPoint LeftToRightOrientation::topRightCorner(const QRect &area) const {
  return area.bottomLeft();
}
CellPosition LeftToRightOrientation::arrowShift(int direction) const {
  switch (direction) {
  case Qt::Key_Up:
    return CellPosition(0, 1);
  case Qt::Key_Down:
    return CellPosition(0, -1);
  case Qt::Key_Left:
    return CellPosition(-1, 0);
  case Qt::Key_Right:
    return CellPosition(1, 0);
  default:
    return CellPosition(0, 0);
  }
}
