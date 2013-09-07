/*****************************************************
 * Copyright zhengyang
 * FileName: signaler.hpp
 * Description: signal notify
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef UTIL_SINGALER_H
#define UTIL_SINGALER_H

class CSignal
{
public:
	CSignal();
    ~CSignal();

	bool Init();
	void UnInit();
	int GetFd();
	void Send();
	void Recv();
private:
	bool SetUnBlock(int fd);
	//  Creates a pair of socket
	static bool MakePair(int& rdFd, int& wrFd);
	int m_WRFD;
	int m_RDFD;
private:
	CSignal(const CSignal&);
	const CSignal &operator=(const CSignal&);
};

#endif
