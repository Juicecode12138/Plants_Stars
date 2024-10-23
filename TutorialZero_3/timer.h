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
			// ��ǰ��¼״̬���Ӷ�����ȷ���ڻص����������ö�ʱ��
			// �Ӷ�ʵ�ֻ��ڻص�������ѭ����ʱ
			bool can_shot = (!one_shot || (one_shot && !shotted));
			shotted = true;
			if (can_shot && callback)
				callback();
			// ����������Ѷ�ʱ����ʵ�ֱ�Ϊ "����" ��˼·
			// ÿ��ʱ�䵽��ʱ������һ���ȴ�ʱ��
			// �������Ը��õ�ƽ���ʱ���
			// �ں��Ч����ȷ����δ�������
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
