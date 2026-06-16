#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>
#include <QDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QList>
#include <QMap>
#include <QDate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

#include "emailparser.h"

class TimelineItem : public QGraphicsEllipseItem
{
public:
    TimelineItem(qreal x, qreal y, qreal width, qreal height, const Email &email, QGraphicsItem *parent = nullptr);
    
    Email getEmail() const { return email; }
    
private:
    Email email;
};

class TimelineWidget : public QGraphicsView
{
    Q_OBJECT

public:
    explicit TimelineWidget(QWidget *parent = nullptr);
    ~TimelineWidget();
    
    void setEmails(const QList<Email> &emails);
    void addEmail(const Email &email);
    void clear();
    int emailCount() const { return emails.size(); }
    
    void setTimeRange(const QDate &start, const QDate &end);
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void setZoomLevel(double level);

signals:
    void emailClicked(const Email &email);
    void emailDoubleClicked(const Email &email);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void buildTimeline();
    void updateTimeAxis();
    void updateItems();
    
    QGraphicsScene *scene;
    QList<Email> emails;
    QMap<QDate, QList<Email>> dateMap;
    
    QDate startDate;
    QDate endDate;
    
    double zoomLevel;
    int itemSpacing;
    int timelineHeight;
    
    QList<TimelineItem*> timelineItems;
    QList<QGraphicsLineItem*> lineItems;
    QList<QGraphicsTextItem*> textItems;
};

class TimelineWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TimelineWindow(QWidget *parent = nullptr);
    ~TimelineWindow();
    
    void setEmails(const QList<Email> &emails);
    void addEmail(const Email &email);
    void clear();

private slots:
    void onZoomIn();
    void onZoomOut();
    void onResetZoom();
    void onEmailClicked(const Email &email);
    void onEmailDoubleClicked(const Email &email);

private:
    void setupUI();
    
    TimelineWidget *timelineWidget;
    QPushButton *zoomInButton;
    QPushButton *zoomOutButton;
    QPushButton *resetZoomButton;
    QSlider *zoomSlider;
    QLabel *infoLabel;
};

#endif // TIMELINE_H
