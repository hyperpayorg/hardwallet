#pragma once
#ifndef COINPASS_SINGLETON_HPP
#define COINPASS_SINGLETON_HPP

template<typename T>
class Singleton {

public:
    template<typename... Args>
    static T* GetInstance(Args&&... args) {
        if (nullptr == _pInstance) {
            _pInstance = new T(std::forward<Args>(args)...);
        }
        return _pInstance;
    }

    static void DestroyInstance() {
        delete _pInstance;
        _pInstance = nullptr;
    }

private:
    Singleton(void);
    virtual ~Singleton(void);
    Singleton(const Singleton&);
    Singleton& operator = (const Singleton&);

private:
    static T* _pInstance;
}; // class Singleton end

template <class T> T* Singleton<T>::_pInstance = nullptr;

#endif
