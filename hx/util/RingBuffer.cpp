﻿#include "RingBuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RingBuffer::RingBuffer(uint32_t min, uint32_t max) {
	size_ = min;
	max_ = max;
	buff_ = (char*)malloc(size_);
	head_ = tail_ = 0;
	used_ = 0;

	cache_ = NULL;
	cacheSize_ = 0;
}

RingBuffer::~RingBuffer() {
	free(buff_);
	if (cache_) {
		free(cache_);
	}
}

char* RingBuffer::PreWrite(uint32_t& size) {
	size = size_ - used_;
	if (size <= 0) {
		if (!Realloc()) {
			return NULL;
		}
		size = size_ - used_;
	}

	if (tail_ >= head_) {
		size = size_ - tail_;
	}
	return buff_ + tail_;
}

void RingBuffer::Commit(uint32_t size) {
	tail_ += size;
	if (tail_ >= size_) {
		tail_ -= size_;
	}
	used_ += size;
}

char* RingBuffer::Read(uint32_t size) {
	if (size > used_) {
		return NULL;
	}

	char* result = NULL;
	if (size > size_ - head_) {
		if (size > cacheSize_) {
			if (cache_) {
				free(cache_);
			}
			cache_ = (char*)malloc(size);
			cacheSize_ = size;
		}

		uint32_t len = size_ - head_;
		memcpy(cache_, buff_ + head_, len);
		memcpy(cache_ + len, buff_, size - len);
		result = cache_;

		head_ += size;
	} else {
		result = buff_ + head_;
		head_ = size - (size_ - head_);
	}

	used_ -= size;
	
	return result;
}

bool RingBuffer::Realloc() {
	if (size_ >= max_) {
		return false;
	}

	size_t nsize = size_ * 2;
	char* nbuff = (char*)malloc(nsize);
	if (used_ > size_ - head_) {
		uint32_t len = size_ - head_;
		memcpy(nbuff, buff_ + head_, len);
		memcpy(nbuff + len, buff_, used_ - len);
	} else {
		if (used_ > 0) {
			memcpy(nbuff, buff_ + head_, used_);
		}
	}

	free(buff_);
	buff_ = nbuff;
	size_ = nsize;
	head_ = 0;
	tail_ = used_;
	return true;
}