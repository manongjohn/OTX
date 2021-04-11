#include "xdtsio.h"

#include "tsystem.h"
#include "toonz/toonzscene.h"
#include "toonz/tproject.h"
#include "toonz/levelset.h"
#include "toonz/txsheet.h"
#include "toonz/txshcell.h"
#include "toonz/txshsimplelevel.h"
#include "toonz/txshchildlevel.h"
#include "toonz/txsheethandle.h"
#include "toonz/tscenehandle.h"
#include "toonz/preferences.h"
#include "toonz/sceneproperties.h"
#include "toonz/tstageobject.h"
#include "toutputproperties.h"

#include "toonzqt/menubarcommand.h"
#include "toonzqt/gutil.h"

#include "tapp.h"
#include "menubarcommandids.h"
#include "xdtsimportpopup.h"
#include "filebrowserpopup.h"

#include <iostream>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegExp>
#include <QFile>
#include <QJsonDocument>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
using namespace XdtsIo;
namespace {
static QByteArray identifierStr("exchangeDigitalTimeSheet Save Data");
}
//-----------------------------------------------------------------------------
void XdtsHeader::read(const QJsonObject &json) {
  QRegExp rx("\\d{1,4}");
  // TODO: We could check if the keys are valid
  // before attempting to read them with QJsonObject::contains(),
  // but we assume that they are.
  m_cut = json["cut"].toString();
  if (!rx.exactMatch(m_cut))  // TODO: should handle an error
    std::cout << "The XdtsHeader value \"cut\" does not match the pattern."
              << std::endl;
  m_scene = json["scene"].toString();
  if (!rx.exactMatch(m_scene))
    std::cout << "The XdtsHeader value \"scene\" does not match the pattern."
              << std::endl;
}

void XdtsHeader::write(QJsonObject &json) const {
  json["cut"]   = m_cut;
  json["scene"] = m_scene;
}

//-----------------------------------------------------------------------------

TFrameId XdtsFrameDataItem::str2Fid(const QString &str) const {
  bool ok;
  int frame = str.toInt(&ok);
  if (ok) return TFrameId(frame);
  // separate the last word as suffix
  frame = str.left(str.size() - 1).toInt(&ok);
  if (!ok) return TFrameId(-1);                              // EMPTY
  if (!str[str.size() - 1].isLetter()) return TFrameId(-1);  // EMPTY
  char c = str[str.size() - 1].toLatin1();

  return TFrameId(frame, c);
}

QString XdtsFrameDataItem::fid2Str(const TFrameId &fid) const {
  if (fid.getLetter() == 0) return QString::number(fid.getNumber());
  return QString::number(fid.getNumber()) + QString(fid.getLetter());
}

void XdtsFrameDataItem::read(const QJsonObject &json) {
  m_id                   = DataId(qRound(json["id"].toDouble()));
  QJsonArray valuesArray = json["values"].toArray();
  for (int vIndex = 0; vIndex < valuesArray.size(); ++vIndex) {
    m_values.append(valuesArray[vIndex].toString());
  }
}

void XdtsFrameDataItem::write(QJsonObject &json) const {
  json["id"] = int(m_id);
  QJsonArray valuesArray;
  foreach (const QString &value, m_values) { valuesArray.append(value); }
  json["values"] = valuesArray;
}

TFrameId XdtsFrameDataItem::getFrameId() const {
  // int getCellNumber() const {
  if (m_values.isEmpty())
    return TFrameId(-1);  // EMPTY
                          // if (m_values.isEmpty()) return 0;
  QString val = m_values.at(0);
  if (val == "SYMBOL_NULL_CELL")
    return TFrameId(-1);  // EMPTY
                          // ignore sheet symbols for now
  else if (val == "SYMBOL_HYPHEN" || val == "SYMBOL_TICK_1" ||
           val == "SYMBOL_TICK_2")
    return TFrameId(-2);  // IGNORE
                          // return -1;
                          // return cell number
  return str2Fid(m_values.at(0));
}

//-----------------------------------------------------------------------------
void XdtsTrackFrameItem::read(const QJsonObject &json) {
  QJsonArray dataArray = json["data"].toArray();
  for (int dataIndex = 0; dataIndex < dataArray.size(); ++dataIndex) {
    QJsonObject dataObject = dataArray[dataIndex].toObject();
    XdtsFrameDataItem data;
    data.read(dataObject);
    m_data.append(data);
  }
  m_frame = json["frame"].toInt();
}

void XdtsTrackFrameItem::write(QJsonObject &json) const {
  QJsonArray dataArray;
  foreach (const XdtsFrameDataItem &data, m_data) {
    QJsonObject dataObject;
    data.write(dataObject);
    dataArray.append(dataObject);
  }
  json["data"] = dataArray;

  json["frame"] = m_frame;
}

//-----------------------------------------------------------------------------

QPair<int, TFrameId> XdtsTrackFrameItem::frameFid() const {
  return QPair<int, TFrameId>(m_frame, m_data[0].getFrameId());
}

//-----------------------------------------------------------------------------
void XdtsFieldTrackItem::read(const QJsonObject &json) {
  QJsonArray frameArray = json["frames"].toArray();
  for (int frameIndex = 0; frameIndex < frameArray.size(); ++frameIndex) {
    QJsonObject frameObject = frameArray[frameIndex].toObject();
    XdtsTrackFrameItem frame;
    frame.read(frameObject);
    m_frames.append(frame);
  }
  m_trackNo = json["trackNo"].toInt();
}

void XdtsFieldTrackItem::write(QJsonObject &json) const {
  QJsonArray frameArray;
  foreach (const XdtsTrackFrameItem &frame, m_frames) {
    QJsonObject frameObject;
    frame.write(frameObject);
    frameArray.append(frameObject);
  }
  json["frames"] = frameArray;

  json["trackNo"] = m_trackNo;
}
//-----------------------------------------------------------------------------

static bool frameLessThan(const QPair<int, TFrameId> &v1,
                          const QPair<int, TFrameId> &v2) {
  return v1.first < v2.first;
}

QVector<TFrameId> XdtsFieldTrackItem::getCellFrameIdTrack() const {
  QList<QPair<int, TFrameId>> frameFids;
  for (const XdtsTrackFrameItem &frame : m_frames)
    frameFids.append(frame.frameFid());
  std::sort(frameFids.begin(), frameFids.end(), frameLessThan);

  QVector<TFrameId> cells;
  int currentFrame       = 0;
  TFrameId initialNumber = TFrameId();
  for (QPair<int, TFrameId> &frameFid : frameFids) {
    while (currentFrame < frameFid.first) {
      cells.append((cells.isEmpty()) ? initialNumber : cells.last());
      currentFrame++;
    }
    // CSP may export negative frame data (although it is not allowed in XDTS
    // format specification) so handle such case.
    if (frameFid.first < 0) {
      initialNumber = frameFid.second;
      continue;
    }
    // ignore sheet symbols for now
    TFrameId cellFid = frameFid.second;
    if (cellFid.getNumber() == -2)  // IGNORE case
      cells.append((cells.isEmpty()) ? TFrameId(-1) : cells.last());
    else
      cells.append(cellFid);
    currentFrame++;
  }
  return cells;
}

QString XdtsFieldTrackItem::build(TXshCellColumn *column) {
  // register the firstly-found level
  TXshLevel *level = nullptr;
  TXshCell prevCell;
  int r0, r1;
  column->getRange(r0, r1);
  if (r0 > 0) addFrame(0, TFrameId(-1));
  for (int row = r0; row <= r1; row++) {
    TXshCell cell = column->getCell(row);
    // try to register the level. simple levels and sub xsheet levels are
    // exported
    if (!level) level = cell.getSimpleLevel();
    if (!level) level = cell.getChildLevel();
    // if the level does not match with the registered one,
    // handle as the empty cell
    if (!level || cell.m_level != level) cell = TXshCell();
    // continue if the cell is continuous
    if (prevCell == cell) continue;

    if (cell.isEmpty())
      addFrame(row, TFrameId(-1));
    else
      addFrame(row, cell.getFrameId());

    prevCell = cell;
  }
  addFrame(r1 + 1, TFrameId(-1));
  if (level)
    return QString::fromStdWString(level->getName());
  else {
    m_frames.clear();
    return QString();
  }
}
//-----------------------------------------------------------------------------

void XdtsTimeTableFieldItem::read(const QJsonObject &json) {
  m_fieldId             = FieldId(qRound(json["fieldId"].toDouble()));
  QJsonArray trackArray = json["tracks"].toArray();
  for (int trackIndex = 0; trackIndex < trackArray.size(); ++trackIndex) {
    QJsonObject trackObject = trackArray[trackIndex].toObject();
    XdtsFieldTrackItem track;
    track.read(trackObject);
    m_tracks.append(track);
  }
}

void XdtsTimeTableFieldItem::write(QJsonObject &json) const {
  json["fieldId"] = int(m_fieldId);
  QJsonArray trackArray;
  foreach (const XdtsFieldTrackItem &track, m_tracks) {
    QJsonObject trackObject;
    track.write(trackObject);
    trackArray.append(trackObject);
  }
  json["tracks"] = trackArray;
}

QList<int> XdtsTimeTableFieldItem::getOccupiedColumns() const {
  QList<int> ret;
  for (const XdtsFieldTrackItem &track : m_tracks) {
    if (!track.isEmpty()) ret.append(track.getTrackNo());
  }
  return ret;
}

QVector<TFrameId> XdtsTimeTableFieldItem::getColumnTrack(int col) const {
  for (const XdtsFieldTrackItem &track : m_tracks) {
    if (track.getTrackNo() != col) continue;
    return track.getCellFrameIdTrack();
  }
  return QVector<TFrameId>();
}

void XdtsTimeTableFieldItem::build(TXsheet *xsheet, QStringList &columnLabels) {
  m_fieldId = CELL;
  for (int col = 0; col < xsheet->getFirstFreeColumnIndex(); col++) {
    if (xsheet->isColumnEmpty(col)) {
      columnLabels.append("");
      continue;
    }
    TXshCellColumn *column = xsheet->getColumn(col)->getCellColumn();
    if (!column) {
      columnLabels.append("");
      continue;
    }
    XdtsFieldTrackItem track(col);
    columnLabels.append(track.build(column));
    if (!track.isEmpty()) m_tracks.append(track);
  }
}
//-----------------------------------------------------------------------------

void XdtsTimeTableHeaderItem::read(const QJsonObject &json) {
  m_fieldId             = FieldId(qRound(json["fieldId"].toDouble()));
  QJsonArray namesArray = json["names"].toArray();
  for (int nIndex = 0; nIndex < namesArray.size(); ++nIndex) {
    m_names.append(namesArray[nIndex].toString());
  }
}

void XdtsTimeTableHeaderItem::write(QJsonObject &json) const {
  json["fieldId"] = int(m_fieldId);
  QJsonArray namesArray;
  foreach (const QString name, m_names) { namesArray.append(name); }
  json["names"] = namesArray;
}

void XdtsTimeTableHeaderItem::build(QStringList &columnLabels) {
  m_names.append(columnLabels);
}

//-----------------------------------------------------------------------------

void XdtsTimeTableItem::read(const QJsonObject &json) {
  if (json.contains("fields")) {
    QJsonArray fieldArray = json["fields"].toArray();
    for (int fieldIndex = 0; fieldIndex < fieldArray.size(); ++fieldIndex) {
      QJsonObject fieldObject = fieldArray[fieldIndex].toObject();
      XdtsTimeTableFieldItem field;
      field.read(fieldObject);
      m_fields.append(field);
      if (field.isCellField()) m_cellFieldIndex = fieldIndex;
    }
  }
  m_duration             = json["duration"].toInt();
  m_name                 = json["name"].toString();
  QJsonArray headerArray = json["timeTableHeaders"].toArray();
  for (int headerIndex = 0; headerIndex < headerArray.size(); ++headerIndex) {
    QJsonObject headerObject = headerArray[headerIndex].toObject();
    XdtsTimeTableHeaderItem header;
    header.read(headerObject);
    m_timeTableHeaders.append(header);
    if (header.isCellField()) m_cellHeaderIndex = headerIndex;
  }
}

void XdtsTimeTableItem::write(QJsonObject &json) const {
  if (!m_fields.isEmpty()) {
    QJsonArray fieldArray;
    foreach (const XdtsTimeTableFieldItem &field, m_fields) {
      QJsonObject fieldObject;
      field.write(fieldObject);
      fieldArray.append(fieldObject);
    }
    json["fields"] = fieldArray;
  }
  json["duration"] = m_duration;
  json["name"]     = m_name;
  QJsonArray headerArray;
  foreach (const XdtsTimeTableHeaderItem header, m_timeTableHeaders) {
    QJsonObject headerObject;
    header.write(headerObject);
    headerArray.append(headerObject);
  }
  json["timeTableHeaders"] = headerArray;
}

QStringList XdtsTimeTableItem::getLevelNames() const {
  // obtain column labels from the header
  assert(m_cellHeaderIndex >= 0);
  QStringList labels = m_timeTableHeaders.at(m_cellHeaderIndex).getLayerNames();
  // obtain occupied column numbers in the field
  assert(m_cellFieldIndex >= 0);
  QList<int> occupiedColumns =
      m_fields.at(m_cellFieldIndex).getOccupiedColumns();
  // return the names of occupied columns
  QStringList ret;
  for (const int columnId : occupiedColumns) ret.append(labels.at(columnId));
  return ret;
}

void XdtsTimeTableItem::build(TXsheet *xsheet, QString name, int duration) {
  m_duration = duration;
  m_name     = name;
  QStringList columnLabels;
  XdtsTimeTableFieldItem field;
  field.build(xsheet, columnLabels);
  m_fields.append(field);
  while (!columnLabels.isEmpty() && columnLabels.last().isEmpty())
    columnLabels.removeLast();
  if (columnLabels.isEmpty()) {
    m_fields.clear();
    return;
  }
  XdtsTimeTableHeaderItem header;
  header.build(columnLabels);
  m_timeTableHeaders.append(header);
  m_cellHeaderIndex = 0;
  m_cellFieldIndex  = 0;
}
//-----------------------------------------------------------------------------

void XdtsData::read(const QJsonObject &json) {
  if (json.contains("header")) {
    QJsonObject headerObject = json["header"].toObject();
    m_header.read(headerObject);
  }
  QJsonArray tableArray = json["timeTables"].toArray();
  for (int tableIndex = 0; tableIndex < tableArray.size(); ++tableIndex) {
    QJsonObject tableObject = tableArray[tableIndex].toObject();
    XdtsTimeTableItem table;
    table.read(tableObject);
    m_timeTables.append(table);
  }
  m_version = Version(qRound(json["version"].toDouble()));
}

void XdtsData::write(QJsonObject &json) const {
  if (!m_header.isEmpty()) {
    QJsonObject headerObject;
    m_header.write(headerObject);
    json["header"] = headerObject;
  }
  QJsonArray tableArray;
  foreach (const XdtsTimeTableItem &table, m_timeTables) {
    QJsonObject tableObject;
    table.write(tableObject);
    tableArray.append(tableObject);
  }
  json["timeTables"] = tableArray;
  json["version"]    = int(m_version);
}

QStringList XdtsData::getLevelNames() const {
  // currently support only the first page of time tables
  return m_timeTables.at(0).getLevelNames();
}

void XdtsData::build(TXsheet *xsheet, QString name, int duration) {
  XdtsTimeTableItem timeTable;
  timeTable.build(xsheet, name, duration);
  if (timeTable.isEmpty()) return;
  m_timeTables.append(timeTable);
}

//-----------------------------------------------------------------------------

bool XdtsIo::loadXdtsScene(ToonzScene *scene, const TFilePath &scenePath) {
  QApplication::restoreOverrideCursor();
  // read the Json file
  QFile loadFile(scenePath.getQString());
  if (!loadFile.open(QIODevice::ReadOnly)) {
    qWarning("Couldn't open save file.");
    return false;
  }

  QByteArray dataArray = loadFile.readAll();

  if (!dataArray.startsWith(identifierStr)) {
    qWarning("The first line does not start with XDTS identifier string.");
    return false;
  }
  // remove identifier
  dataArray.remove(0, identifierStr.length());

  QJsonDocument loadDoc(QJsonDocument::fromJson(dataArray));

  XdtsData xdtsData;
  xdtsData.read(loadDoc.object());

  // obtain level names
  QStringList levelNames = xdtsData.getLevelNames();
  // in case multiple columns have the same name
  levelNames.removeDuplicates();

  scene->clear();

  TProjectManager *pm    = TProjectManager::instance();
  TProjectP sceneProject = pm->loadSceneProject(scenePath);
  if (!sceneProject) return false;
  scene->setProject(sceneProject.getPointer());
  std::string sceneFileName = scenePath.getName() + ".tnz";
  scene->setScenePath(scenePath.getParentDir() + sceneFileName);
  // set the current scene here in order to use $scenefolder node properly
  // in the file browser which opens from XDTSImportPopup
  TApp::instance()->getCurrentScene()->setScene(scene);

  XDTSImportPopup popup(levelNames, scene, scenePath);

  int ret = popup.exec();
  if (ret == QDialog::Rejected) return false;

  QMap<QString, TXshLevel *> levels;
  try {
    for (QString name : levelNames) {
      QString levelPath = popup.getLevelPath(name);
      TXshLevel *level  = nullptr;
      if (!levelPath.isEmpty())
        level = scene->loadLevel(scene->decodeFilePath(TFilePath(levelPath)),
                                 nullptr, name.toStdWString());
      if (!level) {
        int levelType = Preferences::instance()->getDefLevelType();
        level         = scene->createNewLevel(levelType, name.toStdWString());
      }
      levels.insert(name, level);
    }
  } catch (...) {
    return false;
  }

  TXsheet *xsh                       = scene->getXsheet();
  XdtsTimeTableFieldItem cellField   = xdtsData.timeTable().getCellField();
  XdtsTimeTableHeaderItem cellHeader = xdtsData.timeTable().getCellHeader();
  int duration                       = xdtsData.timeTable().getDuration();
  QStringList layerNames             = cellHeader.getLayerNames();
  QList<int> columns                 = cellField.getOccupiedColumns();
  for (int column : columns) {
    QString levelName       = layerNames.at(column);
    TXshLevel *level        = levels.value(levelName);
    QVector<TFrameId> track = cellField.getColumnTrack(column);

    int row = 0;
    std::vector<TFrameId>::iterator it;
    for (TFrameId fid : track) {
      if (fid.getNumber() == -1)  // EMPTY cell case
        row++;
      else
        xsh->setCell(row++, column, TXshCell(level, fid));
    }
    // if the last cell is not "SYMBOL_NULL_CELL", continue the cell
    // to the end of the sheet
    TFrameId lastFid = track.last();
    if (lastFid.getNumber() != -1) {
      for (; row < duration; row++)
        xsh->setCell(row, column, TXshCell(level, TFrameId(lastFid)));
    }

    TStageObject *pegbar =
        xsh->getStageObject(TStageObjectId::ColumnId(column));
    if (pegbar) pegbar->setName(levelName.toStdString());
  }
  xsh->updateFrameCount();

  // if the duration is shorter than frame count, then set it both in
  // preview range and output range.
  if (duration < xsh->getFrameCount()) {
    scene->getProperties()->getPreviewProperties()->setRange(0, duration - 1,
                                                             1);
    scene->getProperties()->getOutputProperties()->setRange(0, duration - 1, 1);
  }

  // emit signal here for updating the frame slider range of flip console
  TApp::instance()->getCurrentScene()->notifySceneChanged();

  return true;
}

class ExportXDTSCommand final : public MenuItemHandler {
public:
  ExportXDTSCommand() : MenuItemHandler(MI_ExportXDTS) {}
  void execute() override;
} exportXDTSCommand;

void ExportXDTSCommand::execute() {
  ToonzScene *scene = TApp::instance()->getCurrentScene()->getScene();
  TXsheet *xsheet   = TApp::instance()->getCurrentXsheet()->getXsheet();
  TFilePath fp      = scene->getScenePath().withType("xdts");

  // if the current xsheet is top xsheet in the scene and the output
  // frame range is specified, set the "to" frame value as duration
  int duration;
  TOutputProperties *oprop = scene->getProperties()->getOutputProperties();
  int from, to, step;
  if (scene->getTopXsheet() == xsheet && oprop->getRange(from, to, step))
    duration = to + 1;
  else
    duration = xsheet->getFrameCount();

  XdtsData xdtsData;
  xdtsData.build(xsheet, QString::fromStdString(fp.getName()), duration);
  if (xdtsData.isEmpty()) {
    DVGui::error(QObject::tr("No columns can be exported."));
    return;
  }

  static GenericSaveFilePopup *savePopup = 0;
  if (!savePopup) {
    savePopup = new GenericSaveFilePopup(
        QObject::tr("Export Exchange Digital Time Sheet (XDTS)"));
    savePopup->addFilterType("xdts");
  }
  if (!scene->isUntitled())
    savePopup->setFolder(fp.getParentDir());
  else
    savePopup->setFolder(
        TProjectManager::instance()->getCurrentProject()->getScenesPath());
  savePopup->setFilename(fp.withoutParentDir());
  fp = savePopup->getPath();
  if (fp.isEmpty()) return;

  QFile saveFile(fp.getQString());

  if (!saveFile.open(QIODevice::WriteOnly)) {
    qWarning("Couldn't open save file.");
    return;
  }

  QJsonObject xdtsObject;
  xdtsData.write(xdtsObject);
  QJsonDocument saveDoc(xdtsObject);
  QByteArray jsonByteArrayData = saveDoc.toJson();
  jsonByteArrayData.prepend(identifierStr + '\n');
  saveFile.write(jsonByteArrayData);

  QString str = QObject::tr("The file %1 has been exported successfully.")
                    .arg(QString::fromStdString(fp.getLevelName()));

  std::vector<QString> buttons = {QObject::tr("OK"),
                                  QObject::tr("Open containing folder")};
  int ret = DVGui::MsgBox(DVGui::INFORMATION, str, buttons);

  if (ret == 2) {
    TFilePath folderPath = fp.getParentDir();
    if (TSystem::isUNC(folderPath))
      QDesktopServices::openUrl(QUrl(folderPath.getQString()));
    else
      QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath.getQString()));
  }
}
