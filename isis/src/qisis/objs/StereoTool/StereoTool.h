#ifndef StereoTool_h
#define StereoTool_h

#include "Tool.h"

#include <QFile>
#include <QPointer>
#include <QString>

#include "AbstractPlotTool.h"
#include "Distance.h"

class QAction;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QMainWindow;

class ProfileDialog;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;
  class ControlPointEdit;
  class Cube;
  class CubeViewport;
  class MdiCubeViewport;
  class SerialNumberList;
  class Stretch;
  class UniversalGroundMap;

  /**
    * @brief Tool for computing parallax
    *
    * @ingroup Visualization Tools
    *
    * @author 2011-12-07 Tracie Sucharski
    *
    */
  class StereoTool : public AbstractPlotTool {
      Q_OBJECT

    public:
      StereoTool(QWidget *parent);
      void paintViewport(MdiCubeViewport *cvp, QPainter *painter);
      static QString lastPtIdValue;

    signals:
      void tieToolSave();
      void editPointChanged();
      void stretchChipViewport(Stretch *, CubeViewport *);

    public slots:
      void createPoint(double lat, double lon);
      void modifyPoint(ControlPoint *point);
      void deletePoint(ControlPoint *point);

    protected:
      QWidget *createToolBarWidget(QStackedWidget *parent);
      QAction *toolPadAction(ToolPad *pad);
      void enableRubberBandTool();
      PlotWindow *createWindow();
      void detachCurves();

    protected slots:
      void rubberBandComplete();
      void activateTool();

    private slots:
      void showHelp();
      void paintAllViewports();
      void calculateElevation();
      void profile();
      void saveElevations();
      void saveAsElevations();

      void userBaseRadius();
      void updateRadiusLineEdit();

      void measureSaved();

      void setTemplateFile();
      void viewTemplateFile();

      void clearProfile();
      void createStartPoint();
      void createEndPoint();

    private:
      void createStereoTool(QWidget *parent);
      void setupFiles();
      void clearNetData();
      void setFiles(Cube *leftCube, Cube *rightCube);
      void clearFiles();
      void paintProfile(MdiCubeViewport *vp, QPainter *painter,
                        std::string serialNumber);
      void calculateElevation(ControlPoint *point);

      void createMenus();

      void loadPoint();
      void updateLabels();

      void warningDialog();
      void readSettings();
      void writeSettings();

      QMainWindow *m_stereoTool;
      QComboBox *m_radiusBox;
      QLineEdit *m_radiusLineEdit;
      ControlPointEdit *m_pointEditor;
      QLabel *m_ptIdValue;
      QLabel *m_leftCubeLabel;
      QLabel *m_rightCubeLabel;
      QLabel *m_elevationLabel;
      QLabel *m_elevationErrorLabel;
      QLabel *m_baseRadiiLabel;
      QLabel *m_leftDemRadiiLabel;
      QLabel *m_rightDemRadiiLabel;

      bool m_showWarning;

      ControlPoint *m_startPoint;
      ControlPoint *m_endPoint;

      enum CubeIndex {
        Left,
        Right
      };

      SerialNumberList *m_serialNumberList;
      QPointer<ControlNet> m_controlNet;
      Distance m_targetRadius;
      Distance m_baseRadius;
      ControlPoint *m_editPoint;
      int m_ptIdIndex;

      QList<CubeViewport *> m_linkedViewports;

      Cube *m_leftCube;
      Cube *m_rightCube;
      std::string m_leftSN;
      std::string m_rightSN;
      UniversalGroundMap *m_leftGM;
      UniversalGroundMap *m_rightGM;

      QFile m_currentFile;
      QAction *m_save;


      ProfileDialog *m_profileDialog;

  };
};

#endif