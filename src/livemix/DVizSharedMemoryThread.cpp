
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QDebug>
#include <QApplication>

#include <assert.h>

#include "DVizSharedMemoryThread.h"

// For defenition of FRAME_WIDTH/_HEIGHT
#include "../SharedMemoryImageWriter.h"

QMap<QString,DVizSharedMemoryThread *> DVizSharedMemoryThread::m_threadMap;
QMutex DVizSharedMemoryThread::threadCacheMutex;

DVizSharedMemoryThread::DVizSharedMemoryThread(const QString& key, QObject *parent)
	: VideoSource(parent)
	, m_fps(10)
	, m_key(key)
	, m_timeAccum(0)
	, m_frameCount(0)
{
}

void DVizSharedMemoryThread::destroySource()
{
	qDebug() << "DVizSharedMemoryThread::destroySource(): "<<this;
	QMutexLocker lock(&threadCacheMutex);
	m_threadMap.remove(m_key);
	
	VideoSource::destroySource();
}

DVizSharedMemoryThread * DVizSharedMemoryThread::threadForKey(const QString& key)
{
	if(key.isEmpty())
		return 0;
		
	QMutexLocker lock(&threadCacheMutex);
	
	if(m_threadMap.contains(key))
	{
		DVizSharedMemoryThread *v = m_threadMap[key];
		qDebug() << "DVizSharedMemoryThread::threadForKey(): "<<v<<": "<<key<<": [CACHE HIT] +";
		return v;
	}
	else
	{
		DVizSharedMemoryThread *v = new DVizSharedMemoryThread(key);
		m_threadMap[key] = v;
		qDebug() << "DVizSharedMemoryThread::threadForCamera(): "<<v<<": "<<key<<": [CACHE MISS] -";
		v->start();

		return v;
	}
}


void DVizSharedMemoryThread::run()
{
	m_sharedMemory.setKey(m_key);
	if(m_sharedMemory.isAttached())
		m_sharedMemory.detach();
		
	m_sharedMemory.attach(QSharedMemory::ReadWrite);
	
	int frameCount = 0;
	int timeAccum = 0;
	QTime time;
	
	int counter = 0;
	while(!m_killed)
	{
		time.restart();
		
		if(!m_sharedMemory.isAttached())
			m_sharedMemory.attach(QSharedMemory::ReadWrite);
		else
		{
			QImage image(FRAME_WIDTH,
			             FRAME_HEIGHT,
				     FRAME_FORMAT);
				
			uchar *to = image.scanLine(0);
			
			m_sharedMemory.lock();
			const uchar *from = (uchar*)m_sharedMemory.data();
			memcpy(to, from, qMin(m_sharedMemory.size(), image.byteCount()));
		
			m_sharedMemory.unlock();
				
			//qDebug() << "DVizSharedMemoryThread::run(): image.size:"<<image.size();
				
			enqueue(VideoFrame(image,1000/m_fps));
		}
		
		
		frameCount ++;
		timeAccum  += time.elapsed();
	
		if(frameCount % m_fps == 0)
		{
			QString msPerFrame;
			msPerFrame.setNum(((double)timeAccum) / ((double)frameCount), 'f', 2);
		
			//qDebug() << "DVizSharedMemoryThread::run(): Avg MS per Frame:"<<msPerFrame;
		}
				
		if(frameCount % (m_fps * 10) == 0)
		{
			timeAccum  = 0;
			frameCount = 0;
		}
		
		msleep(1000 / m_fps / 1.5); // / (m_deinterlace ? 1 : 2));
	};
	
	m_sharedMemory.detach();
}


void DVizSharedMemoryThread::setFps(int fps)
{
	m_fps = fps;
}

DVizSharedMemoryThread::~DVizSharedMemoryThread()
{
	m_killed = true;
	quit();
	wait();
}
