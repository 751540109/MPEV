#include "timeline.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QScrollBar>
#include <QDebug>
#include <algorithm>

// TimelineItem 实现
TimelineItem::TimelineItem(qreal x, qreal y, qreal width, qreal height, const Email &email, QGraphicsItem *parent)
    : QGraphicsEllipseItem(x, y, width, height, parent)
    , email(email)
{
    setAcceptHoverEvents(true);
    
    // 根据星标状态设置颜色
    if (email.isStarred) {
        setBrush(QBrush(QColor(255, 215, 0))); // 金色
    } else {
        setBrush(QBrush(QColor(100, 149, 237))); // 蓝色
    }
    
    setPen(QPen(Qt::black, 1));
    setToolTip(QString("主题: %1\n发件人: %2\n时间: %3")
        .arg(email.subject)
        .arg(email.from)
        .arg(email.date.toString("yyyy-MM-dd HH:mm")));
}

// TimelineWidget 实现
TimelineWidget::TimelineWidget(QWidget *parent)
    : QGraphicsView(parent)
    , zoomLevel(1.0)
    , itemSpacing(50)
    , timelineHeight(100)
{
    scene = new QGraphicsScene(this);
    setScene(scene);
    
    setRenderHint(QPainter::Antialiasing);
    setDragMode(ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    setMinimumHeight(150);
    setMaximumHeight(300);
}

TimelineWidget::~TimelineWidget()
{
    delete scene;
}

void TimelineWidget::setEmails(const QList<Email> &newEmails)
{
    emails = newEmails;
    buildTimeline();
}

void TimelineWidget::addEmail(const Email &email)
{
    emails.append(email);
    buildTimeline();
}

void TimelineWidget::clear()
{
    emails.clear();
    dateMap.clear();
    scene->clear();
    timelineItems.clear();
    lineItems.clear();
    textItems.clear();
}

void TimelineWidget::setTimeRange(const QDate &start, const QDate &end)
{
    startDate = start;
    endDate = end;
    buildTimeline();
}

void TimelineWidget::zoomIn()
{
    zoomLevel *= 1.2;
    updateItems();
}

void TimelineWidget::zoomOut()
{
    zoomLevel /= 1.2;
    updateItems();
}

void TimelineWidget::resetZoom()
{
    zoomLevel = 1.0;
    updateItems();
}

void TimelineWidget::setZoomLevel(double level)
{
    zoomLevel = level;
    updateItems();
}

void TimelineWidget::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void TimelineWidget::mousePressEvent(QMouseEvent *event)
{
    QGraphicsItem *item = itemAt(event->pos());
    if (item && item->type() == QGraphicsEllipseItem::Type) {
        TimelineItem *timelineItem = qgraphicsitem_cast<TimelineItem*>(item);
        if (timelineItem) {
            emit emailClicked(timelineItem->getEmail());
        }
    }
    
    QGraphicsView::mousePressEvent(event);
}

void TimelineWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsItem *item = itemAt(event->pos());
    if (item && item->type() == QGraphicsEllipseItem::Type) {
        TimelineItem *timelineItem = qgraphicsitem_cast<TimelineItem*>(item);
        if (timelineItem) {
            emit emailDoubleClicked(timelineItem->getEmail());
        }
    }
    
    QGraphicsView::mouseDoubleClickEvent(event);
}

void TimelineWidget::buildTimeline()
{
    scene->clear();
    timelineItems.clear();
    lineItems.clear();
    textItems.clear();
    dateMap.clear();
    
    if (emails.isEmpty()) {
        return;
    }
    
    // 按日期分组
    for (const Email &email : emails) {
        QDate date = email.date.date();
        dateMap[date].append(email);
    }
    
    // 获取日期范围
    QList<QDate> dates = dateMap.keys();
    std::sort(dates.begin(), dates.end());
    
    if (dates.isEmpty()) {
        return;
    }
    
    startDate = dates.first();
    endDate = dates.last();
    
    // 绘制时间线
    updateTimeAxis();
    updateItems();
}

void TimelineWidget::updateTimeAxis()
{
    if (startDate.isNull() || endDate.isNull()) {
        return;
    }
    
    // 绘制时间轴线
    QList<QDate> axisDates = dateMap.keys();
    int width = (axisDates.size() - 1) * itemSpacing * zoomLevel + 200;
    int height = timelineHeight;
    
    scene->setSceneRect(0, 0, width, height);
    
    // 绘制主轴线
    QPen linePen(Qt::black, 2);
    QGraphicsLineItem *mainLine = scene->addLine(50, height/2, width - 50, height/2, linePen);
    lineItems.append(mainLine);
    
    // 绘制日期标签
    QFont font("Arial", 10);
    int x = 50;
    for (const QDate &date : axisDates) {
        // 绘制刻度线
        QPen tickPen(Qt::black, 1);
        QGraphicsLineItem *tickLine = scene->addLine(x, height/2 - 10, x, height/2 + 10, tickPen);
        lineItems.append(tickLine);
        
        // 绘制日期文本
        QGraphicsTextItem *dateText = scene->addText(date.toString("yyyy-MM-dd"), font);
        dateText->setPos(x - dateText->boundingRect().width()/2, height/2 + 15);
        textItems.append(dateText);
        
        x += itemSpacing * zoomLevel;
    }
}

void TimelineWidget::updateItems()
{
    // 清除现有项目
    for (TimelineItem *item : timelineItems) {
        scene->removeItem(item);
        delete item;
    }
    timelineItems.clear();
    
    if (dateMap.isEmpty()) {
        return;
    }
    
    // 重新绘制时间轴
    updateTimeAxis();
    
    // 绘制邮件点
    QList<QDate> dates = dateMap.keys();
    std::sort(dates.begin(), dates.end());
    
    int x = 50;
    for (const QDate &date : dates) {
        const QList<Email> &dateEmails = dateMap[date];
        int emailCount = dateEmails.size();
        
        // 计算垂直位置（堆叠显示）
        for (int i = 0; i < emailCount; ++i) {
            qreal y = timelineHeight/2 - 20 - (i * 15);
            qreal itemSize = 10 * zoomLevel;
            
            TimelineItem *item = new TimelineItem(
                x - itemSize/2,
                y - itemSize/2,
                itemSize,
                itemSize,
                dateEmails[i]
            );
            
            scene->addItem(item);
            timelineItems.append(item);
        }
        
        x += itemSpacing * zoomLevel;
    }
}

// TimelineWindow 实现
TimelineWindow::TimelineWindow(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("时间线");
    setMinimumSize(800, 400);
    resize(1000, 500);
}

TimelineWindow::~TimelineWindow()
{
}

void TimelineWindow::setEmails(const QList<Email> &emails)
{
    timelineWidget->setEmails(emails);
    infoLabel->setText(QString("共 %1 封邮件").arg(emails.size()));
}

void TimelineWindow::addEmail(const Email &email)
{
    timelineWidget->addEmail(email);
    infoLabel->setText(QString("共 %1 封邮件").arg(timelineWidget->emailCount()));
}

void TimelineWindow::clear()
{
    timelineWidget->clear();
    infoLabel->setText("无邮件");
}

void TimelineWindow::onZoomIn()
{
    timelineWidget->zoomIn();
    zoomSlider->setValue(zoomSlider->value() + 10);
}

void TimelineWindow::onZoomOut()
{
    timelineWidget->zoomOut();
    zoomSlider->setValue(zoomSlider->value() - 10);
}

void TimelineWindow::onResetZoom()
{
    timelineWidget->resetZoom();
    zoomSlider->setValue(50);
}

void TimelineWindow::onEmailClicked(const Email &email)
{
    qDebug() << "邮件点击:" << email.subject;
}

void TimelineWindow::onEmailDoubleClicked(const Email &email)
{
    qDebug() << "邮件双击:" << email.subject;
}

void TimelineWindow::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 工具栏
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    
    zoomInButton = new QPushButton("放大", this);
    zoomOutButton = new QPushButton("缩小", this);
    resetZoomButton = new QPushButton("重置", this);
    
    zoomSlider = new QSlider(Qt::Horizontal, this);
    zoomSlider->setRange(10, 100);
    zoomSlider->setValue(50);
    
    infoLabel = new QLabel("无邮件", this);
    
    toolbarLayout->addWidget(zoomInButton);
    toolbarLayout->addWidget(zoomOutButton);
    toolbarLayout->addWidget(resetZoomButton);
    toolbarLayout->addWidget(zoomSlider);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(infoLabel);
    
    mainLayout->addLayout(toolbarLayout);
    
    // 时间线视图
    timelineWidget = new TimelineWidget(this);
    mainLayout->addWidget(timelineWidget);
    
    // 连接信号和槽
    connect(zoomInButton, &QPushButton::clicked, this, &TimelineWindow::onZoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &TimelineWindow::onZoomOut);
    connect(resetZoomButton, &QPushButton::clicked, this, &TimelineWindow::onResetZoom);
    
    connect(timelineWidget, &TimelineWidget::emailClicked, this, &TimelineWindow::onEmailClicked);
    connect(timelineWidget, &TimelineWidget::emailDoubleClicked, this, &TimelineWindow::onEmailDoubleClicked);
    
    connect(zoomSlider, &QSlider::valueChanged, this, [this](int value) {
        double zoom = value / 50.0;
        timelineWidget->setZoomLevel(zoom);
    });
}
