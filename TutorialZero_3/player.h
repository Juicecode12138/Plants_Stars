#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "prop.h"
#include "timer.h"
#include "bullet.h"
#include "vector2.h"
#include "platform.h"
#include "particle.h"
#include "player_id.h"

#include <vector>
#include <graphics.h>

extern IMAGE img_1P_cursor;
extern IMAGE img_2P_cursor;

extern Atlas atlas_run_effect;
extern Atlas atlas_jump_effect;
extern Atlas atlas_land_effect;

extern std::vector<Prop> prop_list;
extern std::vector<Bullet*> bullet_list;
extern std::vector<Platform> platform_list;

extern bool is_debug;

class Player
{
public:
	Player(bool facing_right = true) : is_facing_right(facing_right)
	{
		animation_jump_effect.set_atlas(&atlas_jump_effect);
		animation_jump_effect.set_interval(25);
		animation_jump_effect.set_loop(false);
		animation_jump_effect.set_callback([&]()
			{
				is_jump_effect_visible = false;
			});

		animation_land_effect.set_atlas(&atlas_land_effect);
		animation_land_effect.set_interval(50);
		animation_land_effect.set_loop(false);
		animation_land_effect.set_callback([&]()
			{
				is_land_effect_visible = false;
			});

		current_animation = is_facing_right ? &animation_idle_right : &animation_idle_left;

		timer_invulnerable.set_wait_time(500);
		timer_invulnerable.set_one_shot(true);
		timer_invulnerable.set_callback([&]()
			{
				is_invulnerable = false;
			});

		timer_invulnerable_blink.set_wait_time(75);
		timer_invulnerable_blink.set_callback([&]()
			{
				is_showing_sketch_frame = !is_showing_sketch_frame;
			});

		timer_cursor_visibility.set_wait_time(2500);
		timer_cursor_visibility.set_one_shot(true);
		timer_cursor_visibility.set_callback([&]()
			{
				is_cursor_visible = false;
			});

		timer_run_effect_generation.set_wait_time(75);
		timer_run_effect_generation.set_callback([&]()
			{
				Vector2 particle_position;
				IMAGE* frame = atlas_run_effect.get_image(0);
				particle_position.x = position.x + (size.x - frame->getwidth()) / 2;
				particle_position.y = position.y + size.y - frame->getheight();
				particle_list.emplace_back(particle_position, &atlas_run_effect, 45);
			});

		timer_die_effect_generation.set_wait_time(35);
		timer_die_effect_generation.set_callback([&]()
			{
				Vector2 particle_position;
				IMAGE* frame = atlas_run_effect.get_image(0);
				particle_position.x = position.x + (size.x - frame->getwidth()) / 2;
				particle_position.y = position.y + size.y - frame->getheight();
				particle_list.emplace_back(particle_position, &atlas_run_effect, 150);
			});

		timer_attack_cd.set_wait_time(attack_cd);
		timer_attack_cd.set_one_shot(true);
		timer_attack_cd.set_callback([&]()
			{
				can_attack = true;
			});

		timer_buff_hurry.set_wait_time(5000);
		timer_buff_hurry.set_one_shot(true);
		timer_buff_hurry.set_callback([&]()
			{
				has_hurry_buff = false;
			});

		timer_buff_invisible.set_wait_time(3500);
		timer_buff_invisible.set_one_shot(true);
		timer_buff_invisible.set_callback([&]()
			{
				has_invisible_buff = false;
			});

		timer_buff_recover.set_wait_time(6000);
		timer_buff_recover.set_one_shot(true);
		timer_buff_recover.set_callback([&]()
			{
				has_recover_buff = false;
			});

		timer_buff_silence.set_wait_time(3000);
		timer_buff_silence.set_one_shot(true);
		timer_buff_silence.set_callback([&]()
			{
				has_silence_buff = false;
			});
	}

	~Player() = default;

	bool check_hurry_buff() const
	{
		return has_hurry_buff;
	}

	bool check_invisible_buff() const
	{
		return has_invisible_buff;
	}

	bool check_recover_buff() const
	{
		return has_recover_buff;
	}

	bool check_silence_buff() const
	{
		return has_silence_buff;
	}

	virtual void on_update(int delta)
	{
		int direction = is_right_key_down - is_left_key_down;

		if (direction != 0)
		{
			if (!is_attacking_ex)
				is_facing_right = direction > 0;
			current_animation = is_facing_right ? &animation_run_right : &animation_run_left;
			on_run(direction * (has_hurry_buff ? run_velocity + run_velocity_increase : run_velocity) * delta);
		}
		else
		{
			current_animation = is_facing_right ? &animation_idle_right : &animation_idle_left;
			timer_run_effect_generation.pause();
		}

		if (has_recover_buff) hp += 0.01 * delta;
		if (hp >= 100) hp = 100;

		if (is_attacking_ex)
			current_animation = is_facing_right ? &animation_attack_ex_right : &animation_attack_ex_left;

		if (hp <= 0)
			current_animation = last_hurt_direction.x < 0 ? &animation_die_left : &animation_die_right;

		current_animation->on_update(delta);
		animation_jump_effect.on_update(delta);
		animation_land_effect.on_update(delta);

		timer_attack_cd.on_update(delta);
		timer_invulnerable.on_update(delta);
		timer_invulnerable_blink.on_update(delta);
		timer_cursor_visibility.on_update(delta);
		timer_run_effect_generation.on_update(delta);

		timer_buff_hurry.on_update(delta);
		timer_buff_invisible.on_update(delta);
		timer_buff_recover.on_update(delta);
		timer_buff_silence.on_update(delta);

		if (hp <= 0)
			timer_die_effect_generation.on_update(delta);

		if (is_showing_sketch_frame)
			sketch_image(current_animation->get_frame(), &img_sketch);

		particle_list.erase(std::remove_if(
			particle_list.begin(), particle_list.end(),
			[](const Particle& particle)
			{
				return !particle.check_valid();
			}),
			particle_list.end());
		for (Particle& particle : particle_list)
			particle.on_update(delta);

		move_and_collide(delta);
	}

	virtual void on_draw(const Camera& camera)
	{
		if (is_jump_effect_visible)
			animation_jump_effect.on_draw(camera, (int)position_jump_effect.x, (int)position_jump_effect.y);

		if (is_land_effect_visible)
			animation_land_effect.on_draw(camera, (int)position_land_effect.x, (int)position_land_effect.y);

		for (const Particle& particle : particle_list)
			particle.on_draw(camera);

		// 在渲染角色动画时，首先检查是否隐身
		// 只有不具备隐身效果时才可以渲染自身动画
		if (!has_invisible_buff)
		{
			if (hp > 0 && is_invulnerable && is_showing_sketch_frame)
				putimage_alpha(camera, (int)position.x, (int)position.y, &img_sketch);
			else
				current_animation->on_draw(camera, (int)position.x, (int)position.y);
		}

		if (is_cursor_visible)
		{
			switch (id)
			{
			case PlayerID::P1:
				putimage_alpha(camera, (int)(position.x + (size.x - img_1P_cursor.getwidth()) / 2),
					(int)(position.y - img_1P_cursor.getheight()), &img_1P_cursor);
				break;
			case PlayerID::P2:
				putimage_alpha(camera, (int)(position.x + (size.x - img_2P_cursor.getwidth()) / 2),
					(int)(position.y - img_2P_cursor.getheight()), &img_2P_cursor);
				break;
			}
		}

		if (is_debug)
		{
			setlinecolor(RGB(0, 125, 255));
			rectangle((int)position.x, (int)position.y, (int)(position.x + size.x), (int)(position.y + size.y));
		}
	}

	virtual void on_input(const ExMessage& msg)
	{
		if (hp <= 0) return;

		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (id)
			{
			case PlayerID::P1:
				switch (msg.vkcode)
				{
					// 'A'
				case 0x41:
					is_left_key_down = true;
					break;
					// 'D'
				case 0x44:
					is_right_key_down = true;
					break;
					// 'W'
				case 0x57:
					on_jump();
					break;
					// 'F'
				case 0x46:
					// 这里添加了对沉默效果的检查
					// 只有角色没有沉默才可以攻击，后续同理
					if (can_attack && !has_silence_buff)
					{
						on_attack();
						can_attack = false;
						timer_attack_cd.restart();
					}
					break;
					// 'G'
				case 0x47:
					if (mp >= 100 && !has_silence_buff)
					{
						on_attack_ex();
						mp = 0;
					}
					break;
				}
				break;
			case PlayerID::P2:
				switch (msg.vkcode)
				{
					// '←'
				case VK_LEFT:
					is_left_key_down = true;
					break;
					// '→'
				case VK_RIGHT:
					is_right_key_down = true;
					break;
					// '↑'
				case VK_UP:
					on_jump();
					break;
					// '.'
				case VK_OEM_PERIOD:
					if (can_attack && !has_silence_buff)
					{
						on_attack();
						can_attack = false;
						timer_attack_cd.restart();
					}
					break;
					// '/'
				case VK_OEM_2:
					if (mp >= 100 && !has_silence_buff)
					{
						on_attack_ex();
						mp = 0;
					}
					break;
				}
				break;
			}
			break;
		case WM_KEYUP:
			switch (id)
			{
			case PlayerID::P1:
				switch (msg.vkcode)
				{
					// 'A'
				case 0x41:
					is_left_key_down = false;
					break;
					// 'D'
				case 0x44:
					is_right_key_down = false;
					break;
				}
				break;
			case PlayerID::P2:
				switch (msg.vkcode)
				{
					// '←'
				case VK_LEFT:
					is_left_key_down = false;
					break;
					// '→'
				case VK_RIGHT:
					is_right_key_down = false;
					break;
				}
				break;
			}
			break;
		}
	}

	virtual void on_run(float dir_x)
	{
		if (is_attacking_ex)
			return;

		position.x += dir_x;
		timer_run_effect_generation.resume();
	}

	virtual void on_jump()
	{
		if (velocity.y != 0 || is_attacking_ex)
			return;

		velocity.y += jump_velocity;
		is_jump_effect_visible = true;
		animation_jump_effect.reset();

		IMAGE* effect_frame = animation_jump_effect.get_frame();
		position_jump_effect.x = position.x + (size.x - effect_frame->getwidth()) / 2;
		position_jump_effect.y = position.y + size.y - effect_frame->getheight();
	}

	virtual void on_land()
	{
		is_land_effect_visible = true;
		animation_land_effect.reset();

		IMAGE* effect_frame = animation_land_effect.get_frame();
		position_land_effect.x = position.x + (size.x - effect_frame->getwidth()) / 2;
		position_land_effect.y = position.y + size.y - effect_frame->getheight();
	}

	virtual void on_attack() { }
	virtual void on_attack_ex() { }

	virtual void set_hp(double val)
	{
		hp = val;
	}

	double get_hp() const
	{
		return hp;
	}

	double get_mp() const
	{
		return mp;
	}

	void set_id(PlayerID id)
	{
		this->id = id;
	}

	void set_position(float x, float y)
	{
		position.x = x, position.y = y;
	}

	const Vector2& get_postion() const
	{
		return position;
	}

	const Vector2& get_size() const
	{
		return size;
	}

protected:
	const float gravity = 1.6e-3f;
	const float run_velocity = 0.55f;
	const float jump_velocity = -0.85f;
	const float run_velocity_increase = 0.35f;

protected:
	double mp = 0;
	double hp = 100;
	int attack_cd = 500;
	IMAGE img_sketch;
	PlayerID id = PlayerID::P1;
	Vector2 size;
	Vector2 position;
	Vector2 velocity;
	Vector2 position_jump_effect;
	Vector2 position_land_effect;
	Vector2 last_hurt_direction;
	bool can_attack = true;
	bool is_facing_right = true;
	bool is_cursor_visible = true;
	bool is_jump_effect_visible = false;
	bool is_land_effect_visible = false;
	bool is_left_key_down = false;
	bool is_right_key_down = false;
	bool is_attacking_ex = false;
	bool is_invulnerable = false;
	bool is_showing_sketch_frame = false;
	bool has_hurry_buff = false;
	bool has_invisible_buff = false;
	bool has_recover_buff = false;
	bool has_silence_buff = false;
	Animation animation_idle_left;
	Animation animation_idle_right;
	Animation animation_run_left;
	Animation animation_run_right;
	Animation animation_attack_ex_left;
	Animation animation_attack_ex_right;
	Animation animation_die_left;
	Animation animation_die_right;
	Animation animation_jump_effect;
	Animation animation_land_effect;
	Animation* current_animation = nullptr;
	Timer timer_attack_cd;
	Timer timer_invulnerable;
	Timer timer_invulnerable_blink;
	Timer timer_cursor_visibility;
	Timer timer_run_effect_generation;
	Timer timer_die_effect_generation;
	Timer timer_buff_hurry;
	Timer timer_buff_invisible;
	Timer timer_buff_recover;
	Timer timer_buff_silence;
	std::vector<Particle> particle_list;

protected:
	void make_invulnerable()
	{
		is_invulnerable = true;
		timer_invulnerable.restart();
	}

	// BUFF 效果的设置与受击无敌的思路一致
	// 设置标志位，并重置对应定时器，定时器结束后取消效果

	// 设置角色疾跑 BUFF 效果
	void make_buff_hurry()
	{
		has_hurry_buff = true;
		timer_buff_hurry.restart();
	}

	// 设置角色隐身 BUFF 效果
	void make_buff_invisible()
	{
		has_invisible_buff = true;
		timer_buff_invisible.restart();
	}

	// 设置角色恢复 BUFF 效果
	void make_buff_recover()
	{
		has_recover_buff = true;
		timer_buff_recover.restart();
	}

	// 设置角色沉默 BUFF 效果
	void make_buff_silence()
	{
		has_silence_buff = true;
		timer_buff_silence.restart();
	}

	void move_and_collide(int delta)
	{
		// 在物理碰撞检测时，我们在尝试修改速度和位置前记录了上一帧的数据
		// 避免后续回退时因为浮点精度导致的极小概率出现的角色与平台碰撞失效
		float last_velocity_y = velocity.y;
		float last_foot_pos_y = position.y + size.y;

		velocity.y += gravity * delta;
		position += velocity * (float)delta;

		if (hp <= 0)
			return;

		if (velocity.y > 0)
		{
			for (const Platform& platform : platform_list)
			{
				const Platform::CollisionShape& shape = platform.shape;
				bool is_collide_x = (max(position.x + size.x, shape.right) - min(position.x, shape.left)
					<= size.x + (shape.right - shape.left));
				bool is_collide_y = (shape.y >= position.y && shape.y <= position.y + size.y);
				if (is_collide_x && is_collide_y)
				{
					float delta_pos_y = velocity.y * delta;
					if (last_foot_pos_y <= shape.y)
					{
						position.y = shape.y - size.y;
						velocity.y = 0;

						if (last_velocity_y != 0)
							on_land();

						break;
					}
				}
			}
		}

		for (Bullet* bullet : bullet_list)
		{
			if (!bullet->get_valid() || bullet->get_collide_target() != id)
				continue;

			if (!is_invulnerable && bullet->check_collision(position, size))
			{
				make_invulnerable();
				bullet->on_collide();
				bullet->set_valid(false);
				set_hp(get_hp() - bullet->get_damage());
				last_hurt_direction = bullet->get_position() - position;
				if (hp <= 0)
				{
					velocity.x = last_hurt_direction.x < 0 ? 0.35f : -0.35f;
					velocity.y = -1.0f;
				}
			}
		}

		// 检查道具与角色的碰撞
		for (Prop& prop : prop_list)
		{
			if (!prop.can_remove() && prop.check_collision(position, size))
			{
				// 如果发生了碰撞
				// 则根据道具的类型启动不同的 BUFF 效果
				switch (prop.get_type())
				{
				case Prop::Type::Hurry:
					make_buff_hurry();
					break;
				case Prop::Type::Invisible:
					make_buff_invisible();
					break;
				case Prop::Type::Revover:
					make_buff_recover();
					break;
				case Prop::Type::Silence:
					make_buff_silence();
					break;
				}

				prop.make_invalid();
			}
		}
	}
};

#endif // !_PLAYER_H_
