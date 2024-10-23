#ifndef _BUBBLES_BULLET_H_
#define _BUBBLES_BULLET_H_

#include "bullet.h"

extern Atlas atlas_bubbles;

// 开朗菇普通攻击子弹，范围较小
// 子弹在动画播放结束后生命周期结束可以被移除
class BubblesBullet : public Bullet
{
public:
	BubblesBullet()
	{
		size.x = 192, size.y = 192;

		damage = 10;

		animation_bubbles.set_atlas(&atlas_bubbles);
		animation_bubbles.set_interval(50);
		animation_bubbles.set_loop(false);
		animation_bubbles.set_callback([&]() { can_remove = true; });
	}

	~BubblesBullet() = default;

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
		animation_bubbles.on_update(delta);
	}

	void on_draw(const Camera& camera) const
	{
		animation_bubbles.on_draw(camera, (int)position.x, (int)position.y);

		Bullet::on_draw(camera);
	}

private:
	Animation animation_bubbles;

};

#endif // !_BUBBLES_BULLET_H_
