//
// Created by wangyuelin on 2022/10/20.
//

#ifndef IAUDIOPLAYER_BLOCKINGQUEUE_HPP
#define IAUDIOPLAYER_BLOCKINGQUEUE_HPP

#include <cstdlib>
#include <mutex>
#include <queue>

template<typename T>
class BlockingQueue
{
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_not_full, m_not_empty;
    int m_max_size = 20;

public:
    explicit BlockingQueue(int mMaxSize) : m_max_size(mMaxSize) {}

    // 禁止拷贝构造
    BlockingQueue(BlockingQueue &) = delete;

    /**
    * 添加一个元素，队列满时阻塞
    * @param t
     *
     * 关于condition_variable的wait 语法糖参考文章：https://www.cnblogs.com/Arnold-Zhang/p/15732544.html
    */
    void put(T &&value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        // TODO 理解语法糖和lambda表达式，参考文章：https://blog.csdn.net/jusu10/article/details/117567456
        m_not_full.wait(lock, [this]{ return m_queue.size() < m_max_size; });
        m_queue.push(std::forward<T>(value));
        m_not_empty.notify_one();
    }

    /**
    * 获取一个元素，队列为空时阻塞
    * @return
    */
    T take()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_not_empty.wait(lock, [this]{ return !m_queue.empty(); });
        // TODO 为什么这么做？而不是直接： T front = std::move(m_queue.pop());
        T front(T(std::move(m_queue.front()))); // 删除值时把值从它们的位置转移到临时对象。它们仍然可以通过普通的引用和const引用以及rvalue const引用获取，但不能右值获取。
        m_queue.pop();
        m_not_full.notify_one();
        return front;
    }

    /**
     * 获取队列中元素的个数
     * @return
     */
    size_t size()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    // 清空队列
    void clear()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        // TODO 清空队列为什么使用swap来实现
        std::queue<T>().swap(m_queue);
    }
};


#endif //IAUDIOPLAYER_BLOCKINGQUEUE_HPP
