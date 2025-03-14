#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <thread>           //  用于多线程支持，允许在独立线程中接收订阅的消息
#include <functional>
using namespace std;

/*
redis作为集群服务器通信的基于发布-订阅消息队列时，会遇到两个难搞的bug问题，参考我的博客详细描述：
https://blog.csdn.net/QIANGWEIYUAN/article/details/97895611
*/
class Redis
{
public:
    Redis();
    ~Redis();

    // 连接redis服务器 
    bool connect();

    // 向redis指定的通道channel发布消息
    bool publish(int channel, string message);

    // 向redis指定的通道subscribe订阅消息
    bool subscribe(int channel);

    // 向redis指定的通道unsubscribe取消订阅消息
    bool unsubscribe(int channel);

    // 在独立线程中接收订阅通道中的消息         
    // 监听订阅的频道并接收消息，收到消息之后通过回调函数init_notify_handler将消息传递给业务层
    void observer_channel_message();

    // 初始化向业务层上报通道消息的回调对象，用于将接收到的消息传递给业务层。
    void init_notify_handler(function<void(int, string)> fn);

private:
    // hiredis同步上下文对象，负责publish消息       相当于一个客户端
    redisContext *_publish_context;

    // hiredis同步上下文对象，负责subscribe消息       Redis 的订阅模式会阻塞当前上下文，无法同时用于发布和订阅。
    redisContext *_subcribe_context;

    // 回调操作，收到订阅的消息，给service层上报
    function<void(int, string)> _notify_message_handler;
};
#endif