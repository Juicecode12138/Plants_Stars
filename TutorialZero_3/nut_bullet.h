#ifndef _NUT_BULLET_H_
#define _NUT_BULLET_H_

#include "bullet.h"

extern Atlas atlas_nut_explode;

extern Camera main_camera;

// ����ӵ������ڽ�ɫ�Ƿ�Χ�Ա�
// �������������ӵ�����Ҫ��Ⱦ��ըǰ��Ч��
class NutBullet : public Bullet
{
public:
	NutBullet()
	{
		size.x = 288, size.y = 288;

		damage = 0;

		animation_explode.set_atlas(&atlas_nut_explode);
		animation_explode.set_interval(100);
		animation_explode.set_loop(false);
		animation_explode.set_callback([&]() { can_remove = true; });
	}

	~NutBullet() = default;

	// �ӵ�����ײ��������տ��Ĵ����չ�ը�������Ǿ����ཻ��⣬���Ը��������е���
	bool check_collision(const Vector2& position, const Vector2& size)
	{
		bool is_collide_x = (max(this->position.x + this->size.x, position.x + size.x)
			- min(this->position.x, position.x) <= this->size.x + size.x);
		bool is_collide_y = (max(this->position.y + this->size.y, position.y + size.y)
			- min(this->position.y, position.y) <= this->size.y + size.y);

		return is_collide_x && is_collide_y;
	}

	void on_update(int delta)
	{
		animation_explode.on_update(delta);

		main_camera.shake(2.5f, 100);
	}

	void on_draw(const Camera& camera) const
	{
		animation_explode.on_draw(camera, (int)position.x, (int)position.y);

		Bullet::on_draw(camera);
	}

private:
	Animation animation_explode;

};

#endif // !_NUT_BULLET_H_
