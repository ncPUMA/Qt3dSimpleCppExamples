#ifndef CMOVABLEPLANEDELEGATE_H
#define CMOVABLEPLANEDELEGATE_H

#include <QStyledItemDelegate>

class CMovablePlaneDelegate : public QStyledItemDelegate
{
public:
    CMovablePlaneDelegate();

protected:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};

#endif // CMOVABLEPLANEDELEGATE_H
