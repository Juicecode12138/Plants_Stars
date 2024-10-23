#ifndef _TIMER_H_
#define _TIMER_H_

#include <functional>

class Timer
{
public:
	Timer() = default;
	~Timer() = default;

	void restart()
	{
		pass_time = 0;
		shotted = false;
	}

	void set_wait_time(int val)
	{
		wait_time = val;
	}

	void set_one_shot(bool flag)
	{
		one_shot = flag;
	}

	void set_callback(std::function<void()> callback)
	{
		this->callback = callback;
	}

	void pause()
	{
		paused = true;
	}

	void resume()
	{
		paused = false;
	}

	void on_update(int delta)
	{
		if (paused) return;

		pass_time += delta;
		if (pass_time >= wait_time)
		{
			// 提前记录状态，从而可以确保在回调函数中重置定时器
			// 从而实现基于回调函数的循环计时
			bool can_shot = (!one_shot || (one_shot && !shotted));
			shotted = true;
			if (can_shot && callback)
				callback();
			// 我们在这里把定时器的实现变为 "消费" 的思路
			// 每次时间到达时便消耗一个等待时长
			// 这样可以更好地平衡计时误差
			// 在宏观效果上确保多次触发均匀
			pass_time -= wait_time;
		}
	}

private:
	int pass_time = 0;
	int wait_time = 0;
	bool paused = false;
	bool shotted = false;
	bool one_shot = false;
	std::function<void()> callback;
};

#endif // !_TIMER_H_
