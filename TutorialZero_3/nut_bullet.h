#ifndef _NUT_BULLET_H_
#define _NUT_BULLET_H_

#include "bullet.h"

extern Atlas atlas_nut_explode;

extern Camera main_camera;

// 奸果子弹，由于角色是范围自爆
// 所以这个虚拟的子弹不需要渲染爆炸前的效果
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

	// 子弹的碰撞检测与龙日葵的大型日光炸弹很像，是矩形相交检测，所以更容易命中敌人
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
