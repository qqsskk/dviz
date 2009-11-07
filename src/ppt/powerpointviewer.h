/****************************************************************************
**
** Namespace PowerPointViewer generated by dumpcpp from type library
** C:\Program Files\PowerPoint Viewer\msppt8vr.olb
**
****************************************************************************/

#ifndef QAX_DUMPCPP_POWERPOINTVIEWER_H
#define QAX_DUMPCPP_POWERPOINTVIEWER_H

// Define this symbol to __declspec(dllexport) or __declspec(dllimport)
#ifndef POWERPOINTVIEWER_EXPORT
#define POWERPOINTVIEWER_EXPORT
#endif

#include <qaxobject.h>
#include <qaxwidget.h>
#include <qdatetime.h>
#include <qpixmap.h>

struct IDispatch;


// Referenced namespace

namespace PowerPointViewer {

    enum PPVTriState {
        ppVTrue                 = -1,
        ppVFalse                = 0,
        ppVCTrue                = 1,
        ppVTriStateToggle       = -3,
        ppVTriStateMixed        = -2
    };

    enum PpViewerSlideShowAdvanceMode {
        ppViewerSlideShowManualAdvance= 1,
        ppViewerSlideShowUseSlideTimings= 2
    };

// forward declarations
    enum PPVTriState;
    enum PpViewerSlideShowAdvanceMode;
    enum PPVTriState;
    enum PpViewerSlideShowAdvanceMode;
    class SlideShowView;

class POWERPOINTVIEWER_EXPORT _Application : public QAxObject
{
public:
    _Application(IDispatch *subobject = 0, QAxObject *parent = 0)
    : QAxObject((IUnknown*)subobject, parent)
    {
        internalRelease();
    }

    /*
    Property Name

    For more information, see help context 501001 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline QString Name() const; //Returns the value of Name

    /*
    Property Version

    For more information, see help context 501002 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline QString Version() const; //Returns the value of Version

    /*
    Method NewShow

    For more information, see help context 501003 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline PowerPointViewer::SlideShowView* NewShow(const QString& FileName, PowerPointViewer::PpViewerSlideShowAdvanceMode AdvanceMode, PowerPointViewer::PPVTriState Kiosk);

    /*
    Method Quit

    For more information, see help context 501004 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline void Quit();

// meta object functions
    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const { return &staticMetaObject; }
    virtual void *qt_metacast(const char *);
};

class POWERPOINTVIEWER_EXPORT SlideShowView : public QAxObject
{
public:
    SlideShowView(IDispatch *subobject = 0, QAxObject *parent = 0)
    : QAxObject((IUnknown*)subobject, parent)
    {
        internalRelease();
    }

    /*
    Property Parent

    For more information, see help context 503001 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline IDispatch* Parent() const; //Returns the value of Parent

    /*
    Property SlideNumber

    For more information, see help context 503006 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline int SlideNumber() const; //Returns the value of SlideNumber

    /*
    Property SlidesCount

    For more information, see help context 503005 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline int SlidesCount() const; //Returns the value of SlidesCount

    /*
    Method Exit

    For more information, see help context 503007 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline void Exit();

    /*
    Method GotoSlide

    For more information, see help context 503004 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline void GotoSlide(int Index, PowerPointViewer::PPVTriState ResetSlide);

    /*
    Method Next

    For more information, see help context 503002 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline void Next();

    /*
    Method Previous

    For more information, see help context 503003 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline void Previous();

// meta object functions
    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const { return &staticMetaObject; }
    virtual void *qt_metacast(const char *);
};

// Actual coclasses
class POWERPOINTVIEWER_EXPORT Application : public QAxObject
{
public:
    Application(QObject *parent = 0)
    : QAxObject(parent)
    {
        setControl("{fce18141-b12b-11d0-b06a-00aa0060271a}");
    }

    Application(_Application *iface)
    : QAxObject()
    {
        initializeFrom(iface);
        delete iface;
    }

    /*
    Property Name

    For more information, see help context 501001 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline QString Name() const; //Returns the value of Name

    /*
    Property Version

    For more information, see help context 501002 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline QString Version() const; //Returns the value of Version

    /*
    Method NewShow

    For more information, see help context 501003 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline PowerPointViewer::SlideShowView* NewShow(const QString& FileName, PowerPointViewer::PpViewerSlideShowAdvanceMode AdvanceMode, PowerPointViewer::PPVTriState Kiosk);

    /*
    Method Quit

    For more information, see help context 501004 in C:\Program Files\PowerPoint Viewer\VBAPPT8.HLP.
    */
    inline void Quit();

// meta object functions
    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const { return &staticMetaObject; }
    virtual void *qt_metacast(const char *);
};

// member function implementation
#ifndef QAX_DUMPCPP_POWERPOINTVIEWER_NOINLINES
inline QString _Application::Name() const
{
    QVariant qax_result = property("Name");
    Q_ASSERT(qax_result.isValid());
    return *(QString*)qax_result.constData();
}

inline QString _Application::Version() const
{
    QVariant qax_result = property("Version");
    Q_ASSERT(qax_result.isValid());
    return *(QString*)qax_result.constData();
}

inline PowerPointViewer::SlideShowView* _Application::NewShow(const QString& FileName, PowerPointViewer::PpViewerSlideShowAdvanceMode AdvanceMode, PowerPointViewer::PPVTriState Kiosk)
{
    PowerPointViewer::SlideShowView* qax_result = 0;
    qRegisterMetaType("SlideShowView*", &qax_result);
    qRegisterMetaType("SlideShowView", qax_result);
    void *_a[] = {(void*)&qax_result, (void*)&FileName, (void*)&AdvanceMode, (void*)&Kiosk};
    qt_metacall(QMetaObject::InvokeMetaMethod, 7, _a);
    return qax_result;
}

inline void _Application::Quit()
{
    void *_a[] = {0};
    qt_metacall(QMetaObject::InvokeMetaMethod, 8, _a);
}


inline QString Application::Name() const
{
    QVariant qax_result = property("Name");
    Q_ASSERT(qax_result.isValid());
    return *(QString*)qax_result.constData();
}

inline QString Application::Version() const
{
    QVariant qax_result = property("Version");
    Q_ASSERT(qax_result.isValid());
    return *(QString*)qax_result.constData();
}

inline PowerPointViewer::SlideShowView* Application::NewShow(const QString& FileName, PowerPointViewer::PpViewerSlideShowAdvanceMode AdvanceMode, PowerPointViewer::PPVTriState Kiosk)
{
    PowerPointViewer::SlideShowView* qax_result = 0;
    qRegisterMetaType("SlideShowView*", &qax_result);
    qRegisterMetaType("SlideShowView", qax_result);
    void *_a[] = {(void*)&qax_result, (void*)&FileName, (void*)&AdvanceMode, (void*)&Kiosk};
    qt_metacall(QMetaObject::InvokeMetaMethod, 7, _a);
    return qax_result;
}

inline void Application::Quit()
{
    void *_a[] = {0};
    qt_metacall(QMetaObject::InvokeMetaMethod, 8, _a);
}


inline IDispatch* SlideShowView::Parent() const
{
    QVariant qax_result = property("Parent");
    if (!qax_result.constData()) return 0;
    Q_ASSERT(qax_result.isValid());
    return *(IDispatch**)qax_result.constData();
}

inline int SlideShowView::SlideNumber() const
{
    QVariant qax_result = property("SlideNumber");
    Q_ASSERT(qax_result.isValid());
    return *(int*)qax_result.constData();
}

inline int SlideShowView::SlidesCount() const
{
    QVariant qax_result = property("SlidesCount");
    Q_ASSERT(qax_result.isValid());
    return *(int*)qax_result.constData();
}

inline void SlideShowView::Exit()
{
    void *_a[] = {0};
    qt_metacall(QMetaObject::InvokeMetaMethod, 7, _a);
}

inline void SlideShowView::GotoSlide(int Index, PowerPointViewer::PPVTriState ResetSlide)
{
    void *_a[] = {0, (void*)&Index, (void*)&ResetSlide};
    qt_metacall(QMetaObject::InvokeMetaMethod, 8, _a);
}

inline void SlideShowView::Next()
{
    void *_a[] = {0};
    qt_metacall(QMetaObject::InvokeMetaMethod, 9, _a);
}

inline void SlideShowView::Previous()
{
    void *_a[] = {0};
    qt_metacall(QMetaObject::InvokeMetaMethod, 10, _a);
}



#endif

}

template<>
inline void *qMetaTypeConstructHelper(const PowerPointViewer::_Application *t)
{ Q_ASSERT(!t); return new PowerPointViewer::_Application; }

template<>
inline void *qMetaTypeConstructHelper(const PowerPointViewer::Application *t)
{ Q_ASSERT(!t); return new PowerPointViewer::Application; }

template<>
inline void *qMetaTypeConstructHelper(const PowerPointViewer::SlideShowView *t)
{ Q_ASSERT(!t); return new PowerPointViewer::SlideShowView; }

#endif
