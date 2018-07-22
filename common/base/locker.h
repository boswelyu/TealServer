#ifndef _TEAL_LOCKER_H_
#define _TEAL_LOCKER_H_

#include <pthread.h>

class ILockObject
{
public:
    virtual ~ILockObject() {}
    virtual void Lock() = 0;
    virtual bool TryLock() = 0;
    virtual void Unlock() = 0;
};

class SpinLock : public ILockObject
{
public:
    SpinLock()  { pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE); }
    ~SpinLock() { pthread_spin_destroy(&m_lock); }

    virtual void Lock()    { pthread_spin_lock(&m_lock); }
    virtual bool TryLock() { return pthread_spin_trylock(&m_lock) == 0 ? true : false; }
    virtual void Unlock()  { pthread_spin_unlock(&m_lock); }

private:
    pthread_spinlock_t m_lock;
};

class MutexLock : public ILockObject
{
public:
    MutexLock()  { pthread_mutex_init(&m_lock, NULL); }
    ~MutexLock() { pthread_mutex_destroy(&m_lock); }

    virtual void Lock()    { pthread_mutex_lock(&m_lock); }
    virtual bool TryLock() { return pthread_mutex_trylock(&m_lock) == 0 ? true : false; }
    virtual void Unlock()  { pthread_mutex_unlock(&m_lock); }
private:
    pthread_mutex_t m_lock;
};

class Locker
{
public:
    Locker(ILockObject * lock)
    {
        if(lock == NULL) return;
        m_lockObject = lock;
        m_lockObject->Lock();
    }

    ~Locker()
    {
        if(m_lockObject)
        {
            m_lockObject->Unlock();
            m_lockObject = NULL;
        }
    }

private:
    // Hide the default constructor
    Locker() {}

    ILockObject * m_lockObject;
};

#endif