
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QDebug>
#include <QApplication>

#include "CameraThread.h"

extern "C" {
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}

//#include "ccvt/ccvt.h"

#include "CameraViewerWidget.h"

#if defined(Q_OS_WIN)
	#define RAW_PIX_FMT PIX_FMT_BGR565
#else 
	#define RAW_PIX_FMT PIX_FMT_RGB565
#endif


QMap<QString,CameraThread *> CameraThread::m_threadMap;
QStringList CameraThread::m_enumeratedDevices;
bool CameraThread::m_devicesEnumerated = false;

CameraThread::CameraThread(const QString& camera, QObject *parent)
	: QThread(parent)
	, m_inited(false)
	, m_cameraFile(camera)
	, m_frameCount(0)
	, m_yuvBuffer(0)
	, m_killed(false)
{
	m_time_base_rational.num = 1;
	m_time_base_rational.den = AV_TIME_BASE;

	m_sws_context = NULL;
	m_frame = NULL;
}

CameraThread * CameraThread::threadForCamera(const QString& camera)
{
	if(camera.isEmpty())
		return 0;
		
	QStringList devices = enumerateDevices();
	
	if(!devices.contains(camera))
		return 0;
	
	if(m_threadMap.contains(camera))
	{
		CameraThread *v = m_threadMap[camera];
		v->m_refCount++;
		return v;
	}
	else
	{
		CameraThread *v = new CameraThread(camera);
		m_threadMap[camera] = v;
		v->m_refCount=1;
		v->start(QThread::HighPriority);

		return v;
	}
}

void CameraThread::registerConsumer(CameraViewerWidget *consumer)
{
	m_consumerList.append(consumer);
}


void CameraThread::release(CameraViewerWidget *consumer)
{
	if(!consumer)
		return;
		
	if(!m_consumerList.contains(consumer))
		return;
		
	m_consumerList.removeAll(consumer);
	
	m_refCount --;
	//qDebug() << "CameraThread::release: m_refCount:"<<m_refCount;
	if(m_refCount <= 0)
	{
		m_killed = true;
		quit();
		wait();
		deleteLater();
	}
}

QStringList CameraThread::enumerateDevices(bool forceReenum)
{
	if(!forceReenum && m_devicesEnumerated)
		return m_enumeratedDevices;
		
	m_enumeratedDevices.clear();
	m_devicesEnumerated = true;
	
	#ifdef Q_OS_WIN32
		QString deviceBase = "vfwcap://";
	#else
		QString deviceBase = "/dev/video";
	#endif
	QStringList list;
	
	avdevice_register_all();

	avcodec_init();
	avcodec_register_all();
	
	
	AVInputFormat *inFmt = NULL;
	AVFormatParameters formatParams;
	
	for(int i=0; i<10; i++)
	{
		memset(&formatParams, 0, sizeof(AVFormatParameters));

		#ifdef Q_OS_WIN32
		QString fmt = "vfwcap";
		QString file = QString::number(i);
		#else
		QString fmt = "video4linux";
		QString file = QString("/dev/video%1").arg(i);
		#endif

		inFmt = av_find_input_format(qPrintable(fmt));
		if( !inFmt )
		{
			qDebug() << "[ERROR] CameraThread::load(): Unable to find input format:"<<list[0];
			break;
		}

		formatParams.time_base.num = 1;
		formatParams.time_base.den = 35; //25;
		//formatParams.width = 352;
		//formatParams.height = 288;
		//formatParams.channel = 0;
		//formatParams.pix_fmt = PIX_FMT_RGB24 ;
	
	
		// Open video file
		//
		AVFormatContext * formatCtx;
		if(av_open_input_file(&formatCtx, qPrintable(file), inFmt, 0, &formatParams) != 0)
		//if(av_open_input_file(&m_av_format_context, "1", inFmt, 0, NULL) != 0)
		{
			qDebug() << "[WARN] CameraThread::load(): av_open_input_file() failed, file:"<<file;
			break;
		}
		else
		{
			list << QString("%1%2").arg(deviceBase).arg(i);
			av_close_input_file(formatCtx);
		}
	}
	
	qDebug() << "enumerateDevices: Found: "<<list;
	
	m_enumeratedDevices = list;
	return list;
}


int CameraThread::initCamera()
{
	avdevice_register_all();

	avcodec_init();
	avcodec_register_all();

	AVInputFormat *inFmt = NULL;
	AVFormatParameters formatParams;
	memset(&formatParams, 0, sizeof(AVFormatParameters));

	QString fileTmp = m_cameraFile;

	#ifdef Q_OS_WIN32
	QString fmt = "vfwcap";
	if(fileTmp.startsWith("vfwcap://"))
		fileTmp = fileTmp.replace("vfwcap://","");
	#else
	QString fmt = "video4linux";
	#endif

	qDebug() << "[DEBUG] CameraThread::load(): fmt:"<<fmt<<", filetmp:"<<fileTmp;

	inFmt = av_find_input_format(qPrintable(fmt));
	if( !inFmt )
	{
		qDebug() << "[ERROR] CameraThread::load(): Unable to find input format:"<<fmt;
		return -1;
	}

	formatParams.time_base.num = 1;
	formatParams.time_base.den = 35; //25;
// 	formatParams.width = 352;
// 	formatParams.height = 288;
	formatParams.width = 640;
	formatParams.height = 480;
// 	formatParams.channel = 1;
	//formatParams.pix_fmt = PIX_FMT_RGB24 ;


	// Open video file
	 //
	if(av_open_input_file(&m_av_format_context, qPrintable(fileTmp), inFmt, 0, &formatParams) != 0)
	//if(av_open_input_file(&m_av_format_context, "1", inFmt, 0, NULL) != 0)
	{
		qDebug() << "[WARN] CameraThread::load(): av_open_input_file() failed, fileTmp:"<<fileTmp;
		return false;
	}

	//dump_format(m_av_format_context, 0, qPrintable(m_cameraFile), 0);
	qDebug() << "[DEBUG] dump_format():";
	dump_format(m_av_format_context, 0, qPrintable(fileTmp), false);


	uint i;

	// Find the first video stream
	m_video_stream = -1;
	m_audio_stream = -1;
	for(i = 0; i < m_av_format_context->nb_streams; i++)
	{
		if(m_av_format_context->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
		{
			m_video_stream = i;
		}
		if(m_av_format_context->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
		{
			m_audio_stream = i;
		}
	}
	if(m_video_stream == -1)
	{
		qDebug() << "[WARN] CameraThread::load(): Cannot find video stream.";
		return false;
	}

	// Get a pointer to the codec context for the video and audio streams
	m_video_codec_context = m_av_format_context->streams[m_video_stream]->codec;
// 	m_video_codec_context->get_buffer = our_get_buffer;
// 	m_video_codec_context->release_buffer = our_release_buffer;

	// Find the decoder for the video stream
	m_video_codec = avcodec_find_decoder(m_video_codec_context->codec_id);
	if(m_video_codec == NULL)
	{
		qDebug() << "[WARN] CameraThread::load(): avcodec_find_decoder() failed for codec_id:" << m_video_codec_context->codec_id;
		//return false;
	}

	// Open codec
	if(avcodec_open(m_video_codec_context, m_video_codec) < 0)
	{
		qDebug() << "[WARN] CameraThread::load(): avcodec_open() failed.";
		//return false;
	}

	// Allocate video frame
	m_av_frame = avcodec_alloc_frame();

	// Allocate an AVFrame structure
	m_av_rgb_frame =avcodec_alloc_frame();
	if(m_av_rgb_frame == NULL)
	{
		qDebug() << "[WARN] CameraThread::load(): avcodec_alloc_frame() failed.";
		return false;
	}

	qDebug() << "[DEBUG] codec context size:"<<m_video_codec_context->width<<"x"<<m_video_codec_context->height;

	// Determine required buffer size and allocate buffer
	int num_bytes = avpicture_get_size(RAW_PIX_FMT, m_video_codec_context->width, m_video_codec_context->height);

	m_buffer = (uint8_t *)av_malloc(num_bytes * sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture
	avpicture_fill((AVPicture *)m_av_rgb_frame, m_buffer, RAW_PIX_FMT,
					m_video_codec_context->width, m_video_codec_context->height);
	
	if(m_audio_stream != -1)
	{
		m_audio_codec_context = m_av_format_context->streams[m_audio_stream]->codec;

		m_audio_codec = avcodec_find_decoder(m_audio_codec_context->codec_id);
		if(!m_audio_codec)
		{
			//unsupported codec
			return false;
		}
		avcodec_open(m_audio_codec_context, m_audio_codec);
	}

	m_timebase = m_av_format_context->streams[m_video_stream]->time_base;

	m_inited = true;
	return 0;
}

void CameraThread::run()
{
	initCamera();
	
	
	//moveToThread(currentThread());
	//qDebug() << "CameraThread::run(): thread:"<<thread()<<", main:"<<QApplication::instance()->thread()<<", running?"<<isRunning()<<", in main?" << ( QApplication::instance()->thread() == currentThread() ? true:false);
	qDebug() << "CameraThread::run: In Thread ID "<<QThread::currentThreadId(); 
	
	//m_readTimer = new QTimer();
	//connect(m_readTimer, SIGNAL(timeout()), this, SLOT(readFrame()));
	//m_readTimer->setInterval(1000/30); //(formatParams.time_base.den+5));

	//m_readTimer->start();
	//exec();
	while(!m_killed)
	{
		readFrame();
		usleep(1000000 / 30 / 1.5);
	};
}


CameraThread::~CameraThread()
{
	m_killed = true;
	quit();
	wait();

	freeResources();

	if(m_sws_context != NULL)
	{
		sws_freeContext(m_sws_context);
		m_sws_context = NULL;
	}

	if(m_frame != NULL)
	{
		delete m_frame;
		m_frame = 0;
	}
	
	if(m_yuvBuffer)
	{
		delete m_yuvBuffer;
		m_yuvBuffer = 0;
	}
}

void CameraThread::freeResources()
{
	if(!m_inited)
		return;
		
	// Free the RGB image
	if(m_buffer != NULL)
		av_free(m_buffer);
	if(m_av_rgb_frame != NULL)
		av_free(m_av_rgb_frame);

	// Free the YUV frame
	//av_free(m_av_frame);
	//mutex.unlock();

	// Close the codec
	if(m_video_codec_context != NULL) 
		avcodec_close(m_video_codec_context);

	// Close the video file
	if(m_av_format_context != NULL)
		av_close_input_file(m_av_format_context);
}

void CameraThread::readFrame()
{
	if(!m_inited)
	{
	    //emit newImage(QImage());
	    emit frameReady();
	    return;
	}
	AVPacket pkt1, *packet = &pkt1;
	double pts;

	//qDebug() << "CameraThread::readFrame(): My Frame Count # "<<m_frameCount ++;

	int frame_finished = 0;
	while(!frame_finished && !m_killed)
	{
		if(av_read_frame(m_av_format_context, packet) >= 0)
		{
			// Is this a packet from the video stream?
			if(packet->stream_index == m_video_stream)
			{
				//global_video_pkt_pts = packet->pts;

// 				mutex.lock();
				avcodec_decode_video(m_video_codec_context, m_av_frame, &frame_finished, packet->data, packet->size);
// 				mutex.unlock();

				if(packet->dts == AV_NOPTS_VALUE &&
						  m_av_frame->opaque &&
				  *(uint64_t*)m_av_frame->opaque != AV_NOPTS_VALUE)
				{
					pts = *(uint64_t *)m_av_frame->opaque;
				}
				else if(packet->dts != AV_NOPTS_VALUE)
				{
					pts = packet->dts;
				}
				else
				{
					pts = 0;
				}

				pts *= av_q2d(m_timebase);

				// Did we get a video frame?
				if(frame_finished)
				{

					// Convert the image from its native format to RGB, then copy the image data to a QImage
					if(m_sws_context == NULL)
					{
						//mutex.lock();
						//qDebug() << "Creating software scaler for pix_fmt: "<<m_video_codec_context->pix_fmt;
						m_sws_context = sws_getContext(
							m_video_codec_context->width, m_video_codec_context->height,
							m_video_codec_context->pix_fmt,
							m_video_codec_context->width, m_video_codec_context->height,
							//PIX_FMT_RGB32,SWS_BICUBIC,
							RAW_PIX_FMT, SWS_FAST_BILINEAR,
							NULL, NULL, NULL); //SWS_PRINT_INFO
						//mutex.unlock();
						//printf("decode(): created m_sws_context\n");
					}
					//printf("decode(): got frame\n");
					
// 						int w = m_video_codec_context->width;
// 						int h = m_video_codec_context->height;
// 						int sz0 = w * h;//m_av_frame->linesize[0] * h;
// 						//int sz1 = m_av_frame->linesize[1]/2 * h/2;
// 						int sz1 = w/2 * h/2;
// 						
// 						if(m_yuvBuffer == 0)
// 							m_yuvBuffer = (uchar*)malloc(sizeof(uchar) * (sz0 + 2 * sz1));
// 							
// 						unsigned char Output[4 * w * h];
// 						
// 						memcpy(m_yuvBuffer,             m_av_frame->data[0], sz0);
// 						memcpy(m_yuvBuffer + sz0,       m_av_frame->data[1], sz1);
// 						memcpy(m_yuvBuffer + sz0 + sz1, m_av_frame->data[2], sz1);
// 						
// 						ccvt_420p_rgb32(w, h, m_yuvBuffer, &Output);
// 						
// 						QImage frame((uchar*)&Output, w, h, QImage::Format_RGB16);
						
					
						sws_scale(m_sws_context,
							m_av_frame->data,
							m_av_frame->linesize, 0,
							m_video_codec_context->height,
							m_av_rgb_frame->data,
							m_av_rgb_frame->linesize);
	
						
	
						QImage frame(m_av_rgb_frame->data[0],
							m_video_codec_context->width,
							m_video_codec_context->height,
							QImage::Format_RGB16);
					

					av_free_packet(packet);

					// This block from the synchronize_video(VideoState *is, AVFrame *src_frame, double pts) : double
					// function given at: http://www.dranger.com/ffmpeg/tutorial05.html
					{
						// update the frame pts
						double frame_delay;

						if(pts != 0)
						{
							/* if we have pts, set video clock to it */
							m_video_clock = pts;
						} else {
							/* if we aren't given a pts, set it to the clock */
							pts = m_video_clock;
						}
						/* update the video clock */
						frame_delay = av_q2d(m_timebase);
						/* if we are repeating a frame, adjust clock accordingly */
						frame_delay += m_av_frame->repeat_pict * (frame_delay * 0.5);
						m_video_clock += frame_delay;
						//qDebug() << "Frame Dealy: "<<frame_delay;
					}


					//QFFMpegVideoFrame video_frame;
					//video_frame.frame = m_frame;
					//video_frame.pts = pts;
					//video_frame.previous_pts = m_previous_pts;

					//m_current_frame = video_frame;

					//emit newFrame(video_frame);
					//qDebug() << "emit newImage(), frameSize:"<<frame.size()<<", thread:"<<thread();
					//qDebug() << "CameraThread::readFrame: In Thread ID "<<QThread::currentThreadId();

					m_bufferMutex.lock();
					m_bufferImage = frame;
					m_bufferMutex.unlock();

					//emit newImage(frame);
					emit frameReady();

					m_previous_pts = pts;

					//QTimer::singleShot(5, this, SLOT(decode()));
				}

			}
			else if(packet->stream_index == m_audio_stream)
			{
// 				mutex.lock();
				//decode audio packet, store in queue
				av_free_packet(packet);
// 				mutex.unlock();

			}
			else
			{
// 				mutex.lock();
				av_free_packet(packet);
// 				mutex.unlock();

			}
		}
		else
		{
			//emit reachedEnd();
			qDebug() << "reachedEnd()";
		}
	}
}

QImage CameraThread::getImage()
{
	QImage ref;
	m_bufferMutex.lock();
	ref = m_bufferImage.copy();
	m_bufferMutex.unlock();
	return ref;
}
