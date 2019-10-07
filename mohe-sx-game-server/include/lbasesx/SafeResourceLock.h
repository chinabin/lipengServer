#ifndef _SAFE_RESOURCE_LOCK_H_
#define _SAFE_RESOURCE_LOCK_H_

#include "LBase.h"

class CResourceLock
{
	template<class T> friend class CSafeResourceLock;
public:
	CResourceLock()
	{
		m_uLockedSum = 0;
	}
	virtual ~CResourceLock()
	{

	}
protected:
	boost::thread::id m_lockedThreadId;
	unsigned int m_uLockedSum;
};

template<class T> class CSafeResourceLock
{
public:
	CSafeResourceLock(boost::shared_ptr<T> res)
	{
		m_bLocked = false;
		m_safeResource = res;

		_lockUser();
	}

	~CSafeResourceLock()
	{
		_unlockUser();
	}
public:
	boost::shared_ptr<T>& getResource()
	{
		return m_safeResource;
	}

	bool isValid()
	{
		if(!m_bLocked || m_safeResource.get() == NULL)
		{
			return false;
		}

		return true;
	}
protected:
	void _lockUser()
	{
		if(m_safeResource.get() == NULL || m_bLocked)
		{
			return;
		}

		int iLockTime = 0;
		while(true)
		{
			boost::recursive_mutex::scoped_lock l(m_mutexLockResource);

			boost::thread::id thisThredId = boost::this_thread::get_id();

			if(m_safeResource->m_uLockedSum == 0) //��һ������
			{
				m_safeResource->m_lockedThreadId = thisThredId;
				m_safeResource->m_uLockedSum++;
				break;
			}

			if(m_safeResource->m_uLockedSum > 0 && m_safeResource->m_lockedThreadId == thisThredId) //���߳�����
			{
				m_safeResource->m_uLockedSum++;
				break;
			}

			//��������ڱ������߳��������ȴ�1���롣��������
			l.unlock();
			boost::this_thread::sleep(boost::posix_time::millisec(1));

			iLockTime++;
			if((iLockTime % 5000) == 0)
			{
				LLOG_ERROR("********* Lock time more than %d MS. return NULL *********", iLockTime);
				// ����ʧ�� Ϊ�˷�ֹ��һ������ ���ؿյ� �������log��һ���ҵ�ԭ�����
				m_safeResource.reset();
				return;
			}
		}

		m_bLocked = true;
	}

	void _unlockUser()
	{
		if(!m_bLocked)
		{
			return;
		}

		boost::recursive_mutex::scoped_lock l(m_mutexLockResource);
		if(m_safeResource->m_uLockedSum > 0)
		{
			m_safeResource->m_uLockedSum--;
		}

		m_bLocked = false;
	}
private:
	boost::shared_ptr<T> m_safeResource;
	bool m_bLocked;
private:
	static boost::recursive_mutex m_mutexLockResource;
private:
	CSafeResourceLock(){}
};

template<class T> boost::recursive_mutex CSafeResourceLock<T>::m_mutexLockResource;

#endif