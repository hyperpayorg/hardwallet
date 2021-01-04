#pragma once
#ifndef __xManager__
#define __xManager__

#include "JUB_SDK.h"

#include <map>

template <class T>
class xManager {
public:
    xManager() {
        _ID = 0;
        _last = nullptr;
    };
    ~xManager() {
        ClearAll();
    };

    JUB_UINT16 AddOne(T* x) {
        //srand((unsigned int)(time(NULL)));
        //JUB_UINT16 ID = rand();
        if (65535 == _ID) {
            _ID = 0;
        }
        //JUB_UINT16 ID  = ++_ID;

        //std::cout << "Add ID:" << ID << std::endl;
        _mapList.insert(std::make_pair(++_ID, x));
        return _ID;
    };

    JUB_UINT16 AddOne(JUB_UINT16 ID, T* x) {
        _mapList.insert(std::make_pair(ID, x));

        return ID;
    };

    T* GetLast() {
        return _last;
    };

    void ClearLast() { _last = nullptr; };

    void ClearOne(JUB_UINT16 ID) {
        //std::cout << "Clear ID:" << ID << std::endl;
        auto it = _mapList.find(ID);
        if (it != _mapList.end()) {
            delete it->second;
            _mapList.erase(it);
        }
    }

    void ClearAll() {
        for (auto it : _mapList) {
            delete it.second;
        }
        _mapList.clear();
    }

    T* GetOne(JUB_UINT16 ID) {
        auto it = _mapList.find(ID);
        if (it != _mapList.end()) {
            _last = it->second;
            return it->second;
        }
        return nullptr;
    }

    std::vector<JUB_UINT16> GetHandleList() {
        std::vector<JUB_UINT16> vHandleList;
        for (auto it : _mapList) {
            vHandleList.push_back(it.first);
        }
        return vHandleList;
    }

protected:
    std::map<JUB_UINT16, T*> _mapList;
    T* _last;
    JUB_UINT16 _ID;
}; // class xManager end

#endif
