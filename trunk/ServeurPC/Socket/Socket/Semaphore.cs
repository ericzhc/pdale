using System;
using System.Threading;

namespace Socket.TestSemaphore
{
	public class Semaphore
	{
		private Mutex[] m_mutex;
		private Thread[] m_threadIds;
		private int m_threadLimit;

		// constructor
		// parameter - threadLimit (number of threads to control)
		public Semaphore(int threadLimit)
		{
			m_threadLimit = threadLimit;
			m_mutex = new Mutex[m_threadLimit];
			m_threadIds = new Thread[m_threadLimit];
			for (int i=0; i<m_threadLimit; i++)
			{
				m_mutex[i] = new Mutex();
				m_threadIds[i] = null;
			}
		}

		// if there is a timeout then WaitHandle.WaitTimeout is returned
		// calling thread should check for this
		public int Wait()
		{
			int index = WaitHandle.WaitAny(m_mutex);
			if (index != WaitHandle.WaitTimeout)
				m_threadIds[index] = Thread.CurrentThread;
			return index;
		}

		// release the mutex locked by the thread
		public void Release()
		{
			for (int i=0; i<m_threadLimit; i++)
			{
				if (m_threadIds[i] == Thread.CurrentThread)
				{
					m_mutex[i].ReleaseMutex();
				}
			}
		}

	}
}
