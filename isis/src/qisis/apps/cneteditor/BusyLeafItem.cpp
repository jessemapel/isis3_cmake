#include "BusyLeafItem.h"

#include <QString>

#include "iException.h"
#include "iString.h"

#include "AbstractTreeItem.h"


namespace Isis
{
  BusyLeafItem::BusyLeafItem(AbstractTreeItem * parent) : AbstractTreeItem(parent)
  {
    calcDataWidth(1);
  }


  BusyLeafItem::~BusyLeafItem()
  {
  }


  QString BusyLeafItem::getData() const
  {
    return QString("Working...");
  }


  void BusyLeafItem::deleteSource()
  {
    iString msg = "Cannot delete source on BusyLeafItem";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  AbstractTreeItem::InternalPointerType BusyLeafItem::getPointerType() const
  {
    return None;
  }


  void * BusyLeafItem::getPointer() const
  {
    return NULL;
  }


  bool BusyLeafItem::isSelectable() const
  {
    return false;
  }
}