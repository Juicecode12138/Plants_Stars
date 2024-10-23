#ifndef _BUBBLES_BULLET_EX_H_
#define _BUBBLES_BULLET_EX_H_

#include "bullet.h"

extern Atlas atlas_bubbles_ex;

// 开朗菇特殊攻击子弹，范围较小
// 子弹在动画播放结束后生命周期结束可以被移除
class BubblesBulletEx : public Bullet
{
public:
	BubblesBulletEx()
	{
		size.x = 384, size.y = 384;

		damage = 10;

		animation_bubbles_ex.set_atlas(&atlas_bubbles_ex);
		animation_bubbles_ex.set_interval(35);
		animation_bubbles_ex.set_loop(false);
		animation_bubbles_ex.set_callback([&]() { can_remove = true; });
	}

	~BubblesBulletEx() = default;

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
		animation_bubbles_ex.on_update(delta);

		main_camera.shake(5, 100);
	}

	void on_draw(const Camera& camera) const
	{
		animation_bubbles_ex.on_draw(camera, (int)position.x, (int)position.y);

		Bullet::on_draw(camera);
	}

private:
	Animation animation_bubbles_ex;

};

#endif // !_BUBBLES_BULLET_EX_H_
