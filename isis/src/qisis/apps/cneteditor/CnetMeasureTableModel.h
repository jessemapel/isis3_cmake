#ifndef CnetMeasureTableModel_H
#define CnetMeasureTableModel_H


#include "AbstractCnetTableModel.h"


class QStringList;
template< class T > class QList;


namespace Isis
{
  class AbstractTreeItem;
  class TreeModel;

  class CnetMeasureTableModel : public AbstractCnetTableModel
  {
      Q_OBJECT

    public:
      explicit CnetMeasureTableModel(TreeModel * model);
      virtual ~CnetMeasureTableModel();

      virtual QList< AbstractTreeItem * > getItems(int, int);
      virtual QList< AbstractTreeItem * > getItems(AbstractTreeItem *,
          AbstractTreeItem *);
      virtual int getVisibleRowCount() const;
      virtual QList< AbstractTreeItem * > getSelectedItems();
      virtual QString getWarningMessage(AbstractTreeItem const *,
          CnetTableColumn const *, QString valueToSave) const;
      virtual void setGlobalSelection(bool selected);
      virtual int indexOfVisibleItem(AbstractTreeItem const * item) const;

      static QString getMeasureWarningMessage(AbstractTreeItem const *,
          CnetTableColumn const *, QString valueToSave);
      
      
    public slots:
      void handleTreeSelectionChanged(QList< AbstractTreeItem * >);

    protected:
      virtual CnetTableColumnList * createColumns();
      
      
    private slots:
      void calculateFilterCounts();


    private:
      CnetMeasureTableModel(CnetMeasureTableModel const &);
      CnetMeasureTableModel & operator=(CnetMeasureTableModel const &);
  };
}

#endif