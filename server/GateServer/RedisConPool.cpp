#include "RedisMgr.h"

RedisConPool::RedisConPool(size_t poolSize, const char* host, int port, const char* pwd)
	: poolSize_(poolSize), host_(host), port_(port), b_stop_(false), pwd_(pwd), counter_(0) 
{
	for (size_t i = 0; i < poolSize_; ++i) {
		auto* context = redisConnect(host, port);
		if (context == nullptr || context->err != 0) {
			if (context != nullptr) {
				redisFree(context);
			}
			continue;
		}

		auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd);
		if (reply->type == REDIS_REPLY_ERROR) {
			std::cout << "��֤ʧ��" << std::endl;
			//ִ�гɹ� �ͷ�redisCommandִ�к󷵻ص�redisReply��ռ�õ��ڴ�
			freeReplyObject(reply);
			redisFree(context);
			continue;
		}

		//ִ�гɹ� �ͷ�redisCommandִ�к󷵻ص�redisReply��ռ�õ��ڴ�
		freeReplyObject(reply);
		std::cout << "��֤�ɹ�" << std::endl;
		connections_.push(context);
	}

	/*check_thread_ = std::thread([this]() {
		while (!b_stop_) {
			counter_++;
			if (counter_ >= 60) {
				checkThread();
				counter_ = 0;
			}

			std::this_thread::sleep_for(std::chrono::seconds(1)); // ÿ�� 30 �뷢��һ�� PING ����
		}
		});*/

}

RedisConPool::~RedisConPool()
{
	ClearConnections();
}

void RedisConPool::ClearConnections()
{
	std::lock_guard<std::mutex> lock(mutex_);
	while (!connections_.empty()) {
		auto* context = connections_.front();
		redisFree(context);
		connections_.pop();
	}
}

redisContext* RedisConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(mutex_);
	cond_.wait(lock, [this] {
		if (b_stop_) {
			return true; //����pool�Ѿ� close���̲߳�����𣬻����������
		}
		return !connections_.empty();
		});
	//���ֹͣ��ֱ�ӷ��ؿ�ָ��
	if (b_stop_) {
		return  nullptr;
	}
	auto* context = connections_.front();
	connections_.pop();
	return context;
}

void RedisConPool::returnConnection(redisContext* context)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) {
		return;
	}
	connections_.push(context);
	cond_.notify_one();
}

void RedisConPool::Close()
{
	b_stop_ = true;
	cond_.notify_all();
	//check_thread_.join();
}


void RedisConPool::checkThread()
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) {
		return;
	}
	auto pool_size = connections_.size();
	for (int i = 0; i < pool_size && !b_stop_; i++) {
		auto* context = connections_.front();
		connections_.pop();
		try {
			auto reply = (redisReply*)redisCommand(context, "PING");
			if (!reply) {
				std::cout << "redis ping failed" << std::endl;
				connections_.push(context);
				continue;
			}
			freeReplyObject(reply);
			connections_.push(context);
		}
		catch (std::exception& exp) {
			std::cout << "Error keeping connection alive: " << exp.what() << std::endl;
			redisFree(context);
			context = redisConnect(host_, port_);
			if (context == nullptr || context->err != 0) {
				if (context != nullptr) {
					redisFree(context);
				}
				continue;
			}

			auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd_);
			if (reply->type == REDIS_REPLY_ERROR) {
				std::cout << "��֤ʧ��" << std::endl;
				//ִ�гɹ� �ͷ�redisCommandִ�к󷵻ص�redisReply��ռ�õ��ڴ�
				freeReplyObject(reply);
				continue;
			}

			//ִ�гɹ� �ͷ�redisCommandִ�к󷵻ص�redisReply��ռ�õ��ڴ�
			freeReplyObject(reply);
			std::cout << "��֤�ɹ�" << std::endl;
			connections_.push(context);
		}
	}
}
