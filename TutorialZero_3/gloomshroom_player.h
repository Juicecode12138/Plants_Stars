#ifndef _GLOOMSHROOM_PLAYER_H_
#define _GLOOMSHROOM_PLAYER_H_

#include "player.h"
#include "bubbles_bullet.h"
#include "bubbles_bullet_ex.h"

extern Atlas atlas_gloomshroom_idle_left;
extern Atlas atlas_gloomshroom_idle_right;
extern Atlas atlas_gloomshroom_run_left;
extern Atlas atlas_gloomshroom_run_right;
extern Atlas atlas_gloomshroom_attack_ex_left;
extern Atlas atlas_gloomshroom_attack_ex_right;
extern Atlas atlas_gloomshroom_die_left;
extern Atlas atlas_gloomshroom_die_right;

extern Camera main_camera;

/*
	新角色：< 开朗菇 >

	普通攻击为小范围伤害
	特殊攻击为持续的大范围伤害
	技能实现思路依然是在自身位置处生成不同尺寸的子弹
*/
class GloomshroomPlayer : public Player
{
public:
	GloomshroomPlayer(bool facing_right = true) : Player(facing_right)
	{
		animation_idle_left.set_atlas(&atlas_gloomshroom_idle_left);
		animation_idle_right.set_atlas(&atlas_gloomshroom_idle_right);
		animation_run_left.set_atlas(&atlas_gloomshroom_run_left);
		animation_run_right.set_atlas(&atlas_gloomshroom_run_right);
		animation_attack_ex_left.set_atlas(&atlas_gloomshroom_attack_ex_left);
		animation_attack_ex_right.set_atlas(&atlas_gloomshroom_attack_ex_right);
		animation_die_left.set_atlas(&atlas_gloomshroom_die_left);
		animation_die_right.set_atlas(&atlas_gloomshroom_die_right);

		animation_idle_left.set_interval(75);
		animation_idle_right.set_interval(75);
		animation_run_left.set_interval(75);
		animation_run_right.set_interval(75);
		animation_attack_ex_left.set_interval(75);
		animation_attack_ex_right.set_interval(75);
		animation_die_left.set_interval(150);
		animation_die_right.set_interval(150);

		animation_die_left.set_loop(false);
		animation_die_right.set_loop(false);

		timer_attack_ex.set_wait_time(attack_ex_duration);
		timer_attack_ex.set_one_shot(true);
		timer_attack_ex.set_callback([&]()
			{
				is_attacking_ex = false;
			});

		timer_spwan_bubbles_ex.set_wait_time(250);
		timer_spwan_bubbles_ex.set_callback([&]()
			{
				spawn_bubbles_bullet(new BubblesBulletEx());
			});

		size.x = 96, size.y = 96;

		attack_cd = 100;
	}

	~GloomshroomPlayer() = default;

	void on_attack() override
	{
		spawn_bubbles_bullet(new BubblesBullet());

		mciSendString(_T("play bubbles_shot from 0"), NULL, 0, NULL);
	}

	void on_attack_ex() override
	{
		is_attacking_ex = true;
		timer_attack_ex.restart();

		is_facing_right ? animation_attack_ex_right.reset() : animation_attack_ex_left.reset();

		mciSendString(_T("play bubbles_shot_ex from 0"), NULL, 0, NULL);
	}

	void on_update(int delta) override
	{
		Player::on_update(delta);

		if (is_attacking_ex)
		{
			main_camera.shake(5, 100);
			timer_attack_ex.on_update(delta);
			timer_spwan_bubbles_ex.on_update(delta);
		}
	}

private:
	const int attack_ex_duration = 2500;

private:
	Timer timer_attack_ex;
	Timer timer_spwan_bubbles_ex;

private:
	void spawn_bubbles_bullet(Bullet* bullet)
	{
		Vector2 bullet_position;
		const Vector2& bullet_size = bullet->get_size();
		bullet_position.x = position.x + (size.x - bullet_size.x) / 2;
		bullet_position.y = position.y + (size.y - bullet_size.y) / 2;

		bullet->set_position(bullet_position.x, bullet_position.y);

		bullet->set_collide_target(id == PlayerID::P1 ? PlayerID::P2 : PlayerID::P1);

		bullet->set_callback([&]() { mp += 20; });

		bullet_list.push_back(bullet);
	}

};

#endif // !_GLOOMSHROOM_PLAYER_H_
