#ifndef CameraTest_h
#define CameraTest_h

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QDebug>

#include <QGLWidget>

class CameraThread;
class CameraServer;
class CameraTest : public QGLWidget
{
	Q_OBJECT
public:
	CameraTest();
	~CameraTest();

public slots:
	void newFrame(QImage);

signals:
	void readyForNextFrame();

protected:
	void paintEvent(QPaintEvent*);

private:
	CameraThread * m_thread;
	QImage m_frame;
	CameraServer * m_server;
};



#endif //CameraTest_h