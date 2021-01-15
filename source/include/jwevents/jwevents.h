//
//  jwevents.h
//  Amtp
//
//  Created by jiangwei on 2020/5/22.
//  Copyright © 2020 jiangwei. All rights reserved.
//

#ifndef jwevents_h
#define jwevents_h

#include <map>


template<typename Func>
class JwEvents 
{
public:
    JwEvents() :m_nextKey(0)
    {}
    ~JwEvents(){}
    

    int operator += (Func&& f)
    {
        return Connect(std::forward<Func>(f));
    }

    int operator += (Func& f)
    {
        return Connect(f);
    }

    template<typename... Args>
    void operator()(Args&&... args)
    {
        Notify(std::forward<Args>(args)...);
    }

    JwEvents& operator -= (int key)
    {
        Disconnect(key);
        return *this;
    }

    void Clear()
    {
        m_connections.clear();
    }

private:
    int Connect(Func&& f)
    {
        return Assgin(std::forward<Func>(f));
    }

    int Connect(Func& f)
    {
        return Assgin(f);
    }

    void Disconnect(int key)
    {
        m_connections.erase(key);
    }

    template<typename... Args>
    void Notify(Args&&... args)
    {
        for (auto& it : m_connections)
        {
            it.second(std::forward<Args>(args)...);
        }
    }

    template<typename F>
    int Assgin(F&& f)
    {
        int index = m_nextKey++;
        Push(index, std::forward<Func>(f));
        return index;
    }

    template<typename F>
    void Push(int index, F&& f)
    {
        m_connections.emplace(index, std::move(f));
    }

    template<typename F>
    void Push(int index, F& f)
    {
        m_connections.emplace(index, f);
    }

private:
    int m_nextKey;
    std::map<int, Func> m_connections;
};
#endif /* jwevents_h */
