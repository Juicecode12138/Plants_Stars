#ifndef _NUT_PLAYER_H_
#define _NUT_PLAYER_H_

#include "player.h"
#include "nut_bullet.h"

extern Atlas atlas_nut_idle_left;
extern Atlas atlas_nut_idle_right;
extern Atlas atlas_nut_run_left;
extern Atlas atlas_nut_run_right;
extern Atlas atlas_nut_attack_ex_left;
extern Atlas atlas_nut_attack_ex_right;
extern Atlas atlas_nut_die_left;
extern Atlas atlas_nut_die_right;

extern Camera main_camera;

/*
	�½�ɫ��< ��� >

	���Ը���ʱ�仺���ָ�����ֵ������ֵ
	��ͨ����Ϊ���泯�ķ���˲��һС�ξ���
	�ܵ�����������������
	���⹥���Ա���������Ѫ������������������
*/
class NutPlayer : public Player
{
public:
	NutPlayer(bool facing_right = true) : Player(facing_right)
	{
		animation_idle_left.set_atlas(&atlas_nut_idle_left);
		animation_idle_right.set_atlas(&atlas_nut_idle_right);
		animation_run_left.set_atlas(&atlas_nut_run_left);
		animation_run_right.set_atlas(&atlas_nut_run_right);
		animation_attack_ex_left.set_atlas(&atlas_nut_attack_ex_left);
		animation_attack_ex_right.set_atlas(&atlas_nut_attack_ex_right);
		animation_die_left.set_atlas(&atlas_nut_die_left);
		animation_die_right.set_atlas(&atlas_nut_die_right);

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
		animation_attack_ex_left.set_loop(false);
		animation_attack_ex_right.set_loop(false);

		animation_attack_ex_left.set_callback([&]() { on_explode(); });
		animation_attack_ex_right.set_callback([&]() { on_explode(); });

		size.x = 96, size.y = 96;

		attack_cd = 200;
	}

	~NutPlayer() = default;

	// ��ͨ����˲�Ƶ�ʵ�֣�ֻ��Ҫ��λ�ý����޸ļ���
	void on_attack() override
	{
		position.x += is_facing_right ? 150 : -150;

		mciSendString(_T("play nut_dash from 0"), NULL, 0, NULL);
	}

	void on_attack_ex() override
	{
		is_attacking_ex = true;

		is_facing_right ? animation_attack_ex_right.reset() : animation_attack_ex_left.reset();

		mciSendString(_T("play nut_explode from 0"), NULL, 0, NULL);
	}

	void set_hp(double val) override
	{
		if (val < hp) mp += (int)((hp - val) * 4);

		hp = val;
	}

	void on_update(int delta) override
	{
		Player::on_update(delta);

		if (hp > 0) hp += 0.002 * delta;
		if (hp >= 100) hp = 100;

		mp += 0.002 * delta;

		if (is_attacking_ex)
			main_camera.shake(2.5f, 100);
	}

private:
	// ���⹥���Ա����߼�Ϊ��������Χ���ɸ߶��˺��ӵ�
	// �ӵ�������ɵ��˺����ݽ�ɫ����ǰʣ��������̬��ֵ
	void on_explode()
	{
		is_attacking_ex = false;

		Bullet* bullet = new NutBullet();

		Vector2 bullet_position;
		const Vector2& bullet_size = bullet->get_size();
		bullet_position.x = position.x + (size.x - bullet_size.x) / 2;
		bullet_position.y = position.y + (size.y - bullet_size.y) / 2;

		bullet->set_position(bullet_position.x, bullet_position.y);

		int damage = (int)(hp / 2); hp -= damage;

		bullet->set_damage((int)(damage * 1.5));
		bullet->set_collide_target(id == PlayerID::P1 ? PlayerID::P2 : PlayerID::P1);

		bullet->set_callback([&]() { mp += 35; });

		bullet_list.push_back(bullet);
	}

};

#endif // !_NUT_PLAYER_H_
