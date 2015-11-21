#ifndef SHAREDPTR_HPP_
#define SHAREDPTR_HPP_
#include<pthread.h>

namespace cs540{

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

class proxyBase{
	public:
		proxyBase() : count_(0) {
		}
		virtual ~proxyBase(){
		}
		void incrementCount(){
			pthread_mutex_lock(&lock);
			++count_;
			pthread_mutex_unlock(&lock);
		}
		int decrementCount(){
			int a;
			pthread_mutex_lock(&lock);
				a = --count_;
			pthread_mutex_unlock(&lock);
			return a;
		}
	private:
		int count_;
};


template <typename U>
class proxyDerived: public proxyBase{
	public:
		proxyDerived(U *p): proxyBase(){
			this->ptr_ = p;
		}
		~proxyDerived(){
			delete this->ptr_;
		}
	private:
		U* ptr_;
};


template <typename T>
class SharedPtr{
	public:
		T* data_;
		proxyBase* baseptr_;
		SharedPtr()
			:data_(nullptr), baseptr_(nullptr){
			
		}
		SharedPtr(const SharedPtr &);
		SharedPtr(SharedPtr &&);
		SharedPtr &operator=(const SharedPtr &);
		SharedPtr &operator=(SharedPtr &&);
		template <typename U> explicit SharedPtr(U *);
		template <typename U> SharedPtr(const SharedPtr<U> &);
		template <typename U> SharedPtr(SharedPtr<U> &&);
		template <typename U> SharedPtr& operator=(const SharedPtr<U> &);
		template <typename U> SharedPtr& operator=(SharedPtr<U> &&);
		~SharedPtr();
		void reset();
		template <typename U> void reset(U *);
		T *get() const;
		T& operator*() const;
		T* operator->() const;
		explicit operator bool() const;
		template <typename T1, typename T2>
		friend bool operator==(const SharedPtr<T1> &, const SharedPtr<T2> &);
		friend bool operator==(const SharedPtr<T> &, std::nullptr_t);
		friend bool operator==(std::nullptr_t, const SharedPtr<T> &);
		template <typename T1, typename T2>
		friend bool operator!=(const SharedPtr<T1>&, const SharedPtr<T2> &);
		friend bool operator!=(const SharedPtr<T> &, std::nullptr_t);
		friend bool operator!=(std::nullptr_t, const SharedPtr<T> &);
		template <typename U>
		friend SharedPtr static_pointer_cast(const SharedPtr<U> &);
		template <typename U>
		friend SharedPtr dynamic_pointer_cast(const SharedPtr<U> &);
};


template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr<T> &p)
	:data_(p.data_), baseptr_(p.baseptr_){
	if(p.baseptr_ != nullptr){
		this->baseptr_->incrementCount();
	}
}


template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr<T> &&p)
	:data_(p.data_), baseptr_(p.baseptr_){
	p.data_ = nullptr;
	p.baseptr_ = nullptr;
}


template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(U *p)
	:data_(static_cast<T*>(p)), baseptr_(new proxyDerived<U>(p)){
	this->baseptr_->incrementCount();
}


template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(const SharedPtr<U> &p)
	:data_(static_cast<T*>(p.data_)), baseptr_(p.baseptr_){
	if(p.baseptr_ != nullptr){
		this->baseptr_->incrementCount();
	}
}


template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(SharedPtr<U> &&p)
	:data_((dynamic_cast<T>(p))->data_), baseptr_(p.baseptr_){
	p.data_ = nullptr;
	p.baseptr_ = nullptr;
}


template <typename T>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<T> &p){
	if(this != &p){
		if(this->baseptr_ != nullptr){
			if(this->baseptr_->decrementCount() == 0){
				delete baseptr_;
				baseptr_ = nullptr;
				data_ = nullptr;
			}
		}
		this->data_ = p.data_;
		this->baseptr_ = p.baseptr_;	
		if(p.data_ != nullptr){
			this->baseptr_->incrementCount();
		}
	}
	return *this;
}


template <typename T>
SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr<T> &&p){
	if(this->baseptr_ != nullptr){
		if(this->baseptr_->decrementCount() == 0){
			delete baseptr_;
			baseptr_ = nullptr;
			data_ = nullptr;
		}
	}
	this->data_ = p.data_;
	this->baseptr_ = p.baseptr_;
	p.data_ = nullptr;
	p.baseptr_ = nullptr;
	return *this;
}


template <typename T>
template <typename U>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<U> &p){
	if(this != (SharedPtr<T>*)&p){
		if(this->baseptr_ != nullptr){
			if(this->baseptr_->decrementCount() == 0){
				delete baseptr_;
				baseptr_ = nullptr;
				data_ = nullptr;
			}
		}
		this->data_ = p.data_;
		this->baseptr_ = p.baseptr_;
		if(p.data_ != nullptr){
			this->baseptr_->incrementCount();
		}
	}
	return *this;
}


template <typename T>
template <typename U>
SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr<U> &&p){
	if(this->baseptr_ != nullptr){
		if(this->baseptr_->decrementCount() == 0){
			delete baseptr_;
			baseptr_ = nullptr;
			data_ = nullptr;
		}
	}
	this->data_ = p.data_;
	this->baseptr_ = p.baseptr_;
	p.data_ = nullptr;
	p.baseptr_ = nullptr;
	return *this;
}


template <typename T>
SharedPtr<T>::~SharedPtr(){
	if(this->baseptr_ != nullptr){
		if(this->baseptr_->decrementCount()== 0){
			delete baseptr_;
			baseptr_ = nullptr;
			data_ = nullptr;
		}
	}	
}


template <typename T>
void SharedPtr<T>::reset(){
	if(this->baseptr_ != nullptr){
		if(this->baseptr_->decrementCount() == 0){
			delete this->baseptr_;
		}
	}
	this->data_ = nullptr;
	this->baseptr_ = nullptr;
}


template <typename T>
template <typename U>
void SharedPtr<T>::reset(U *p){
	if(this->baseptr_ != nullptr){
		if(this->baseptr_->decrementCount() == 0){
			delete this->baseptr_;
			this->baseptr_ = nullptr;
			this->data_ = nullptr;
		}
	}
	this->data_ = p;
	this->baseptr_ = new proxyDerived<U>(p);
	if(p != nullptr){
		this->baseptr_->incrementCount();
	}
}


template <typename T>
T *SharedPtr<T>::get() const{
	return this->data_;
}


template <typename T>
T &SharedPtr<T>::operator*() const{
	return *(this->data_);
}


template <typename T>
T *SharedPtr<T>::operator->() const{
	return this->data_;
}


template <typename T>
SharedPtr<T>::operator bool() const{
	if(this->data_ == nullptr){
		return false;
	}
	return true;
}


template <typename T1, typename T2>
bool operator==(const SharedPtr<T1> &p1, const SharedPtr<T2> &p2){
	if(p1.data_ == p2.data_){
		return true;
	}
	return false;
}


template <typename T>
bool operator==(const SharedPtr<T> &p, std::nullptr_t pnull){
	if(p == pnull){
		return true;
	}
	return false;
}


template <typename T>
bool operator==(std::nullptr_t pnull, const SharedPtr<T> &p){
	if(p == pnull){
		return true;
	}
	return false;
}


template <typename T1, typename T2>
bool operator!=(const SharedPtr<T1> &p1, const SharedPtr<T2> &p2){
	return !(p1 == p2);
}


template <typename T>
bool operator!=(const SharedPtr<T> &p, std::nullptr_t pnull){
	return !(p.get() == nullptr);
}


template <typename T>
bool operator!=(std::nullptr_t pnull, const SharedPtr<T> &p){
	return !(p.get() == nullptr);
}


template<typename T, typename U>
SharedPtr<T> static_pointer_cast(const SharedPtr<U> &p){
	SharedPtr<T> shptr(p);
	shptr.data_ = static_cast<T*>(p.data_);
	return shptr;
}


template<typename T, typename U>
SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &p){
	SharedPtr<T> shptr(p);
	shptr.data_ = dynamic_cast<T*>(p.data_);
	return shptr;
}

}

#endif
