/*
 * CyclicBuffer.h
 *
 *  Created on: 18 sie 2015
 *      Author: bsp
 */

#ifndef UTILITIES_CYCLICBUFFER_H_
#define UTILITIES_CYCLICBUFFER_H_

#include <cstddef>
#include <utility>

template <class T> class CyclicBuffer;

template <class T>
class CyclicBufferIterator {
public:
  CyclicBufferIterator(const CyclicBuffer<T> & cb, bool end, std::size_t first)
    : _buf(cb), _end(end), _curr(first)
  {}

  const T & operator*() const {
    return _buf._data[_curr];
  }

  bool operator!=(const CyclicBufferIterator & other) const {
    if(_end && other._end) {
      return false;
    }
    else if(!_end && !other._end) {
      return _curr != other._curr;
    }
    else {
      return true;
    }
  }

  void operator++() {
    _curr = (_curr + 1) % _buf._capacity;
    if(_curr == _buf._afterLast) {
      _end = true;
    }
  }

private:
  const CyclicBuffer<T> & _buf;
  bool _end;
  std::size_t _curr;
};

template <class T>
class CyclicBuffer {
public:
  typedef CyclicBufferIterator<T> ConstIterator;

  CyclicBuffer()
    : _capacity(0)
    , _first(0)
    , _afterLast(0)
    , _isFull(true)
  {
    _data = NULL;
  }

  CyclicBuffer(std::size_t capacity)
    : _capacity(capacity)
    , _first(0)
    , _afterLast(0)
  {
    if(_capacity > 0) {
      _isFull = false;
      _data = new T [_capacity];
    }
    else {
      _isFull = true;
      _data = NULL;
    }
  }

  CyclicBuffer(CyclicBuffer && other)
    : CyclicBuffer()
  {
    moveFrom(std::move(other));
  }

  ~CyclicBuffer() {
    delete [] _data;
  }

  CyclicBuffer & operator=(CyclicBuffer && other) {
    moveFrom(std::move(other));
    return *this;
  }

  void push_back(const T & elem) {
    _data[_afterLast] = elem;

    _afterLast = (_afterLast + 1) % _capacity;
    if(_isFull) {
      _first = _afterLast;
    }
    else {
      _isFull = (_first == _afterLast);
    }
  }

  std::size_t capacity() const { return _capacity; }

  std::size_t size() const {
    if(_isFull) {
      return _capacity;
    }
    if(_afterLast >= _first) {
      return _afterLast - _first;
    }
    else {
      return _capacity - (_first - _afterLast);
    }
  }

  bool isFull() const { return _isFull; }

  bool isEmpty() const { return (!_isFull && _first == _afterLast) || (_data == NULL); }

  ConstIterator begin() const {
    return ConstIterator(*this, isEmpty(), _first);
  }

  ConstIterator end() const {
    return ConstIterator(*this, true, 0);
  }

private:
  void moveFrom(CyclicBuffer && other) {
    std::swap(_capacity, other._capacity);
    std::swap(_data, other._data);
    std::swap(_first, other._first);
    std::swap(_afterLast, other._afterLast);
    std::swap(_isFull, other._isFull);
  }

  std::size_t _capacity;
  T * _data;
  std::size_t _first;
  std::size_t _afterLast;
  bool _isFull;

  friend class CyclicBufferIterator<T>;
};

#endif /* UTILITIES_CYCLICBUFFER_H_ */
