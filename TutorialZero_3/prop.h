#ifndef _PROP_H_
#define _PROP_H_

#include "util.h"
#include "camera.h"
#include "vector2.h"
#include "animation.h"

extern Atlas atlas_prop_blue;
extern Atlas atlas_prop_pink;
extern Atlas atlas_prop_yellow;

// ��� BUFF ������
class Prop
{
public:
	enum class Type
	{
		Hurry = 0,
		Invisible,
		Revover,
		Silence,
		Invalid
	};

public:
	Prop()
	{
		size.x = 40, size.y = 40;

		const int padding = 100;
		position.x = (float)(padding + rand() % (1280 - padding * 2));
		position.y = -size.y / 2;

		// �ڵ��߳�ʼ��ʱ�����Ǳ�Ϊ�丳ֵЧ������
		type = (Type)(rand() % (int)Type::Invalid);

		animation.set_loop(true);
		animation.set_interval(50);
		
		// ���ѡȡһ�ֶ���ͼ����ʾ
		// ��ʾЧ�������� BUFF Ч���޹�
		switch (rand() % 3)
		{
		case 0:
			animation.set_atlas(&atlas_prop_blue);
			break;
		case 1:
			animation.set_atlas(&atlas_prop_pink);
			break;
		case 2:
			animation.set_atlas(&atlas_prop_yellow);
			break;
		}
	}

	~Prop() = default;

	void on_update(int delta)
	{
		// �õ��߳�����ֱ�����˶�
		position.y += 0.15f * delta;
		animation.on_update(delta);

		// ���߷ɳ������²�ʱ�������
		if (position.y >= 720)
			make_invalid();
	}

	void on_draw(const Camera& camera)
	{
		animation.on_draw(camera, (int)(position.x - size.x / 2), (int)(position.y - size.y / 2));
	}

	bool can_remove() const
	{
		return !is_valid;
	}

	void make_invalid()
	{
		is_valid = false;
	}

	Type get_type() const
	{
		return type;
	}

	const Vector2& get_size() const
	{
		return size;
	}

	const Vector2& get_position() const
	{
		return position;
	}

	// �������������ײ��ʹ��������Ϊ���㣬���ӵ���ײ����߼���ͬ
	bool check_collision(const Vector2& position, const Vector2& size) const
	{
		return this->position.x + this->size.x / 2 >= position.x
			&& this->position.x + this->size.x / 2 <= position.x + size.x
			&& this->position.y + this->size.y / 2 >= position.y
			&& this->position.y + this->size.y / 2 <= position.y + size.y;
	}

private:
	Type type;
	Vector2 size;
	Vector2 position;
	Animation animation;
	bool is_valid = true;

};

#endif // !_PROP_H_
