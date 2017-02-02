/* \brief

This is a fixed circular buffer class to be used in thread programming
the class use thread synchronization mechanism through conditional variables 

This class is used to save images primarly but also can save objects of different type
and has the synchronization mechanism built in, so the user can define a new buffer
and leave the class manage the thread memory access

Code use part of code from:
Copyright (c) 2013, Juan Palacios <juan.palacios.puyana@gmail.com>
All rights reserved.

date:	November.27.2016
author: Henry Portilla
Special
thanks:	God
*/

#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

// include circular buffer to be used like a reconfigurable fixed size buffer
#include <boost\circular_buffer.hpp>

template <class T>
class Buffer
{
public:
	Buffer();
	Buffer(const Buffer& other);
	// methods
	//Buffer& operator=(const T& item);
	void setSize(int size);
	int getSize() const;
	T pop();
	void pop(T& item);
	void push(const T& item);
	void push(T&& item);

private:
	mutable std::mutex bufferMutex;
	std::condition_variable bufferCondVar;
	boost::circular_buffer_space_optimized<T> ringBuffer;
};

template <class T>
Buffer<T>::Buffer()
{

}

template <class T>
Buffer<T>::Buffer(const Buffer& other)
{
	std::unique_lock<std::mutex> mLock(other.bufferMutex);
	ringBuffer = other.ringBuffer;
}

template <class T>
void Buffer<T>::setSize(int size) {
	std::unique_lock<std::mutex> mLock(bufferMutex);
	// copy in constructor body rather than 
	// the member initializer list 
	// in order to ensure that the mutex is held across the copy. (BogotoBogo Blog on thread issues 05.12.16)
	ringBuffer.set_capacity(size);
}

template <class T>
int Buffer<T>::getSize() const {
	std::unique_lock<std::mutex> mLock(bufferMutex);
	return ringBuffer.size();
}

template <class T>
T Buffer<T>::pop() {
	std::unique_lock<std::mutex> mLock(bufferMutex);
	while (ringBuffer.empty()) {
		bufferCondVar.wait(mLock);
	}
	auto item = ringBuffer.front();
	ringBuffer.pop_front();
	return item;
}

template <class T>
void Buffer<T>::pop(T& item) {
	std::unique_lock<std::mutex> mLock(bufferMutex);
	while (ringBuffer.empty()) {
		bufferCondVar.wait(mLock);
	}
	item = ringBuffer.front();
	ringBuffer.pop_front();
}

template <class T>
void Buffer<T>::push(const T& item) {
	std::unique_lock<std::mutex> mLock(bufferMutex);
	ringBuffer.push_back(item);
	mLock.unlock();
	bufferCondVar.notify_one();
}

template <class T>
void Buffer<T>::push(T&& item) {
	std::unique_lock<std::mutex> mLock(bufferMutex);
	ringBuffer.push_back(std::move(item));
	mLock.unlock();
	bufferCondVar.notify_one();
}

