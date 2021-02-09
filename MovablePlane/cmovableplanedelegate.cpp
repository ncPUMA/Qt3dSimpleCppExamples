#include "cmovableplanedelegate.h"

#include <QLineEdit>
#include <QDoubleSpinBox>

#include "cmovableplanemodel.h"
#include "movableplanemodel_types.h"

using namespace MovablePlaneModel;

CMovablePlaneDelegate::CMovablePlaneDelegate() :
    QStyledItemDelegate(nullptr)
{

}

QWidget *CMovablePlaneDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    (void)option;

    QWidget *res = nullptr;
    if(index.isValid())
    {
        const TItemId itmId = static_cast <TItemId> (index.data(Qt::UserRole).toInt());
        switch(itmId)
        {
            case ENID_NAME:
                res = new QLineEdit(parent);
                break;
            case ENID_OX:
            case ENID_OY:
            case ENID_OZ:
            {
                QDoubleSpinBox * const dsBox = new QDoubleSpinBox(parent);
                dsBox->setRange(-10.f, 10.f);
                dsBox->setDecimals(6);
                res = dsBox;
                break;
            }
            case ENID_RX:
            case ENID_RY:
            case ENID_RZ:
            {
                QDoubleSpinBox * const dsBox = new QDoubleSpinBox(parent);
                dsBox->setRange(0.f, 359.999f);
                dsBox->setDecimals(3);
                res = dsBox;
                break;
            }
            case ENID_WIDTH:
            case ENID_HEIGHT:
            {
                QDoubleSpinBox * const dsBox = new QDoubleSpinBox(parent);
                dsBox->setRange(.5f, 10.f);
                dsBox->setDecimals(3);
                res = dsBox;
                break;
            }
            default:
                break;
        }
    }
    return res;
}

void CMovablePlaneDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const TItemId itmId = static_cast <TItemId> (index.data(Qt::UserRole).toInt());
    switch(itmId)
    {
        case ENID_NAME:
        {
            QLineEdit * const le = qobject_cast <QLineEdit *> (editor);
            if (le)
                le->setText(index.data().toString());
            break;
        }
        case ENID_OX:
        case ENID_OY:
        case ENID_OZ:
        case ENID_RX:
        case ENID_RY:
        case ENID_RZ:
        case ENID_WIDTH:
        case ENID_HEIGHT:
        {
            QDoubleSpinBox * const dsBox = qobject_cast <QDoubleSpinBox *> (editor);
            if (dsBox)
                dsBox->setValue(index.data().toDouble());
            break;
        }
        default:
            break;
    }
}

void CMovablePlaneDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    const TItemId itmId =
            static_cast <TItemId> (index.data(Qt::UserRole).toInt());
    switch(itmId)
    {
        case ENID_NAME:
        {
            QLineEdit * const le = qobject_cast <QLineEdit *> (editor);
            if (le)
                model->setData(index, le->text());
            break;
        }
        case ENID_OX:
        case ENID_OY:
        case ENID_OZ:
        case ENID_RX:
        case ENID_RY:
        case ENID_RZ:
        case ENID_WIDTH:
        case ENID_HEIGHT:
        {
            QDoubleSpinBox * const dsBox = qobject_cast <QDoubleSpinBox *> (editor);
            if(dsBox)
                model->setData(index, dsBox->value());
            break;
        }
        default:
            break;
    }
}

