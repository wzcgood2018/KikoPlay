#include "clickslider.h"
#include <QMouseEvent>
#include <QStyleOptionSlider>
#include <QStylePainter>

void ClickSlider::setEventMark(const QList<DanmuEvent> &eventList)
{
    this->eventList=eventList;
}

void ClickSlider::setChapterMark(const QList<MPVPlayer::ChapterInfo> &chapters)
{
    this->chapters=chapters;
}

void ClickSlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);
    if(!isSliderDown() && event->button() == Qt::LeftButton)
    {
        int dur = maximum()-minimum();
        double x=event->x();
        if(x<0)x=0;
        if(x>maximum())x=maximum();
        int pos = minimum() + dur * (x /width());
        emit sliderClick(pos);
        setValue(pos);
    }

}

void ClickSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if(isSliderDown())
    {
        int dur = maximum()-minimum();
        double x=event->x();
        if(x<0)x=0;
        if(x>maximum())x=maximum();
        int pos = minimum() + dur * (x /width());
        emit sliderUp(pos);
        setValue(pos);
    }
    QSlider::mouseReleaseEvent(event);
}

void ClickSlider::mouseMoveEvent(QMouseEvent *event)
{
    int dur = maximum()-minimum();
    double x=event->x();
    if(x<0)x=0;
    if(x>maximum())x=maximum();
    int pos = minimum() + dur * (x /width());
    QString desc;
    if(!chapters.isEmpty())
    {
        for(auto &c:chapters)
        {
            if(qAbs(pos-c.position)<3000)
            {
                desc = tr("Chapter: %1").arg(c.title);
                break;
            }
        }
    }
    if(!eventList.isEmpty())
    {
        for(auto &e:eventList)
        {
            if(pos>=e.start-3000)
            {
                if(pos<=e.start+e.duration+3000)
                {
                    if(!desc.isEmpty()) desc += "\n";
                    desc += e.description;
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    emit mouseMove(event->x(),event->y(),pos,desc);
}

void ClickSlider::paintEvent(QPaintEvent *event)
{
    QSlider::paintEvent(event);
    if(eventList.isEmpty() && chapters.isEmpty()) return;

    QStylePainter painter(this);
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    static QColor eColor1(255,117,0),eColor2(0xff,0x00,0x33), cColor1(0xbc,0xbc,0xbc), cColor2(0x43,0x9c,0xf3);
    int h = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove,this).height();
    int y =height()-h;
    y=(y&1)?y/2+1:y/2;

    for(auto &e:eventList)
    {
        int left = QStyle::sliderPositionFromValue(minimum(), maximum(), qBound(minimum(), e.start, maximum()), width());
        int right = QStyle::sliderPositionFromValue(minimum(), maximum(), qBound(minimum(), e.start+e.duration, maximum()), width());
        painter.fillRect(left,y,right-left,h,e.start+e.duration<value()?eColor2:eColor1);
    }
    for(auto &c:chapters)
    {
        int left = QStyle::sliderPositionFromValue(minimum(), maximum(), qBound(minimum(), c.position, maximum()), width());
        painter.fillRect(left,y/2,2,y-y/2,c.position<value()?cColor2:cColor1);
    }

}
