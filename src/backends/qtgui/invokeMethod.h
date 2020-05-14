#pragma once

#include <QCoreApplication>
#include <QEvent>
#include <QPointer>
#include <QThread>
#include <atomic>
#include <condition_variable>
#include <mutex>

// In Qt 5.10 and later, we can just use QObject::invokeMethod
//
// QWidget and subclasses are not thread-safe! This helper takes arbitrary code
// from any thread, safely runs it on the appropriate GUI thread, waits for it
// to finish, and returns the result.
//
// If the target object is destructed before the code gets to run, the QPointer
// will be nulled and the function will return nullopt.

template <typename F>
void invokeMethod(QObject* object, F&& functor) {
  // If we queue up a functor on the current thread, it won't run until we return to the event loop,
  // which means waiting for it to finish will never complete. Instead, run it immediately.
  if (object->thread() == QThread::currentThread())
    return functor();

  class Event {
  public:
    void set() {
        std::lock_guard<std::mutex> lk(mutex);
        value = true;
        condvar.notify_all();
    }
    
    void wait() {
        std::unique_lock<std::mutex> lk(mutex);
        condvar.wait(lk, [&] { return (bool)value; });
    }
    
    std::atomic<bool> value;
    std::mutex mutex;
    std::condition_variable condvar;
  };

  class FnInvokeEvent : public QEvent {
  public:
    FnInvokeEvent(F&& functor, QObject* obj, Event& event)
        : QEvent(QEvent::None), m_func(std::move(functor)), m_obj(obj), m_event(event)
    {}

    ~FnInvokeEvent() {
      if (m_obj) {
        m_func();
      }
      m_event.set();
    }

    F m_func;
    QPointer<QObject> m_obj;
    Event& m_event;
  };

  Event event;
  QCoreApplication::postEvent(object,
                              new FnInvokeEvent(std::forward<F>(functor), object, event));
  event.wait();
  return;
}