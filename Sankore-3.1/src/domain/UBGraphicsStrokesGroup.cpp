#include "UBGraphicsStrokesGroup.h"

#include "core/memcheck.h"

UBGraphicsStrokesGroup::UBGraphicsStrokesGroup(QGraphicsItem *parent):QGraphicsItemGroup(parent)
{
    mDelegate = new UBGraphicsItemDelegate(this, 0, true, true);
    mDelegate->init();
    mDelegate->setFlippable(true);
    setData(UBGraphicsItemData::ItemLayerType, UBItemLayerType::Object);

    setData(UBGraphicsItemData::itemLayerType, QVariant(itemLayerType::ObjectItem)); //Necessary to set if we want z value to be assigned correctly
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

UBGraphicsStrokesGroup::~UBGraphicsStrokesGroup()
{
    if(mDelegate){
        delete mDelegate;
    }
}

void UBGraphicsStrokesGroup::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (mDelegate->mousePressEvent(event))
    {
        //NOOP
    }
    else
    {
//        QGraphicsItemGroup::mousePressEvent(event);
    }
}

void UBGraphicsStrokesGroup::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (mDelegate->mouseMoveEvent(event))
    {
        // NOOP;
    }
    else
    {
        QGraphicsItemGroup::mouseMoveEvent(event);
    }
}

void UBGraphicsStrokesGroup::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    mDelegate->mouseReleaseEvent(event);
    QGraphicsItemGroup::mouseReleaseEvent(event);
}

UBItem* UBGraphicsStrokesGroup::deepCopy() const
{
   UBGraphicsStrokesGroup* copy = new UBGraphicsStrokesGroup();

   copy->setPos(this->pos());
   copy->setTransform(this->transform());
   copy->setFlag(QGraphicsItem::ItemIsMovable, true);
   copy->setFlag(QGraphicsItem::ItemIsSelectable, true);
   copy->setData(UBGraphicsItemData::ItemLayerType, this->data(UBGraphicsItemData::ItemLayerType));
   copy->setData(UBGraphicsItemData::ItemLocked, this->data(UBGraphicsItemData::ItemLocked));

   return copy;
}

void UBGraphicsStrokesGroup::remove()
{
    if (mDelegate)
        mDelegate->remove(true);
}

void UBGraphicsStrokesGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Never draw the rubber band, we draw our custom selection with the DelegateFrame
    QStyleOptionGraphicsItem styleOption = QStyleOptionGraphicsItem(*option);
    styleOption.state &= ~QStyle::State_Selected;

    QGraphicsItemGroup::paint(painter, &styleOption, widget);
}

QVariant UBGraphicsStrokesGroup::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QVariant newValue = mDelegate->itemChange(change, value);
    return QGraphicsItemGroup::itemChange(change, newValue);
}
