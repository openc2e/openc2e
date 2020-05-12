#pragma once

#include <QWidget>
#include <thread>

class QLabel;
class QPlainTextEdit;

class PrayToolQt : public QWidget
{
    Q_OBJECT

public:
    PrayToolQt();
    ~PrayToolQt();

    enum WindowState {
        STATE_DEFAULT,
        STATE_DRAGGING,
        STATE_PROCESSING,
    };
    
    void setState(WindowState state);
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    
    std::thread work_thread;
    WindowState state;
    QLabel *dropArea;
    QPlainTextEdit *plain_text_edit;
};