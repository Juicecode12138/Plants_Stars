#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "timer.h"
#include "scene.h"
#include "bullet.h"
#include "platform.h"
#include "status_bar.h"

#include <vector>

extern bool is_debug;

extern IMAGE img_sky;
extern IMAGE img_hills;
extern IMAGE img_platform_large;
extern IMAGE img_platform_small;

extern IMAGE img_1P_winnner;
extern IMAGE img_2P_winnner;
extern IMAGE img_winnner_bar;

extern Camera main_camera;
extern std::vector<Prop> prop_list;
extern std::vector<Bullet*> bullet_list;
extern std::vector<Platform> platform_list;

extern Player* player_1;
extern Player* player_2;

extern IMAGE* img_player_1_avatar;
extern IMAGE* img_player_2_avatar;

extern SceneManager scene_manager;

class GameScene : public Scene
{
public:
	GameScene() = default;
	~GameScene() = default;

	void on_enter() 
	{
		status_bar_1P.set_avatar(img_player_1_avatar);
		status_bar_2P.set_avatar(img_player_2_avatar);

		status_bar_1P.set_position(235, 625);
		status_bar_2P.set_position(675, 625);

		is_game_over = false;
		is_slide_out_started = false;

		player_1->set_position(200, 50);
		player_2->set_position(975, 50);

		pos_img_sky.x = (getwidth() - img_sky.getwidth()) / 2;
		pos_img_sky.y = (getheight() - img_sky.getheight()) / 2;

		pos_img_hills.x = (getwidth() - img_hills.getwidth()) / 2;
		pos_img_hills.y = (getheight() - img_hills.getheight()) / 2;

		pos_img_winner_bar.x = -img_winnner_bar.getwidth();
		pos_img_winner_bar.y = (getheight() - img_winnner_bar.getheight()) / 2;
		pos_x_img_winner_bar_dst = (getwidth() - img_winnner_bar.getwidth()) / 2;

		pos_img_winner_text.x = pos_img_winner_bar.x;
		pos_img_winner_text.y = (getheight() - img_1P_winnner.getheight()) / 2;
		pos_x_img_winner_text_dst = (getwidth() - img_1P_winnner.getwidth()) / 2;

		// 随机道具生成定时器初始化
		timer_prop_generator.set_wait_time(2500);
		timer_prop_generator.set_one_shot(false);
		timer_prop_generator.set_callback([&]()
			{
				// 每隔 2.5 秒尝试生成道具，有 25% 的概率生成
				if (rand() % 100 >= 75)
					prop_list.emplace_back();
			});

		timer_winnner_slide_in.restart();
		timer_winnner_slide_in.set_wait_time(2500);
		timer_winnner_slide_in.set_one_shot(true);
		timer_winnner_slide_in.set_callback([&]()
			{ 
				is_slide_out_started = true;
			});

		timer_winnner_slide_out.restart();
		timer_winnner_slide_out.set_wait_time(1000);
		timer_winnner_slide_out.set_one_shot(true);
		timer_winnner_slide_out.set_callback([&]()
			{
				scene_manager.switch_to(SceneManager::SceneType::Menu);
			});

		bullet_list.clear();
		platform_list.resize(4);

		Platform& large_platform = platform_list[0];
		large_platform.img = &img_platform_large;
		large_platform.render_positon.x = 122;
		large_platform.render_positon.y = 455;
		large_platform.shape.left = (float)large_platform.render_positon.x + 30;
		large_platform.shape.right = (float)large_platform.render_positon.x + img_platform_large.getwidth() - 30;
		large_platform.shape.y = (float)large_platform.render_positon.y + 60;

		Platform& small_platform_1= platform_list[1];
		small_platform_1.img = &img_platform_small;
		small_platform_1.render_positon.x = 175;
		small_platform_1.render_positon.y = 360;
		small_platform_1.shape.left = (float)small_platform_1.render_positon.x + 40;
		small_platform_1.shape.right = (float)small_platform_1.render_positon.x + img_platform_small.getwidth() - 40;
		small_platform_1.shape.y = (float)small_platform_1.render_positon.y + img_platform_small.getheight() / 2;

		Platform& small_platform_2 = platform_list[2];
		small_platform_2.img = &img_platform_small;
		small_platform_2.render_positon.x = 855;
		small_platform_2.render_positon.y = 360;
		small_platform_2.shape.left = (float)small_platform_2.render_positon.x + 40;
		small_platform_2.shape.right = (float)small_platform_2.render_positon.x + img_platform_small.getwidth() - 40;
		small_platform_2.shape.y = (float)small_platform_2.render_positon.y + img_platform_small.getheight() / 2;

		Platform& small_platform_3 = platform_list[3];
		small_platform_3.img = &img_platform_small;
		small_platform_3.render_positon.x = 515;
		small_platform_3.render_positon.y = 225;
		small_platform_3.shape.left = (float)small_platform_3.render_positon.x + 40;
		small_platform_3.shape.right = (float)small_platform_3.render_positon.x + img_platform_small.getwidth() - 40;
		small_platform_3.shape.y = (float)small_platform_3.render_positon.y + img_platform_small.getheight() / 2;

		mciSendString(_T("play bgm_game repeat from 0"), NULL, 0, NULL);
	}

	void on_update(int delta)
	{
		main_camera.on_update(delta);

		player_1->on_update(delta);
		player_2->on_update(delta);

		bullet_list.erase(std::remove_if(
			bullet_list.begin(), bullet_list.end(),
			[](const Bullet* bullet)
			{
				bool deletable = bullet->check_can_remove();
				if (deletable) delete bullet;
				return deletable;
			}),
			bullet_list.end());
		for (Bullet* bullet : bullet_list)
			bullet->on_update(delta);

		prop_list.erase(std::remove_if(
			prop_list.begin(), prop_list.end(),
			[](const Prop& prop) { return prop.can_remove(); }),
			prop_list.end());
		for (Prop& prop : prop_list)
			prop.on_update(delta);

		const Vector2& position_player_1 = player_1->get_postion();
		const Vector2& position_player_2 = player_2->get_postion();
		if (position_player_1.y >= getheight())
			player_1->set_hp(0);
		if (position_player_2.y >= getheight())
			player_2->set_hp(0);

		if (player_1->get_hp() <= 0 || player_2->get_hp() <= 0)
		{
			if (!is_game_over)
			{
				mciSendString(_T("stop bgm_game"), NULL, 0, NULL);
				mciSendString(_T("play ui_win from 0"), NULL, 0, NULL);
			}

			is_game_over = true;
		}

		status_bar_1P.set_hp((int)player_1->get_hp());
		status_bar_1P.set_mp((int)player_1->get_mp());
		status_bar_1P.set_hurry_buff(player_1->check_hurry_buff());
		status_bar_1P.set_invisible_buff(player_1->check_invisible_buff());
		status_bar_1P.set_recover_buff(player_1->check_recover_buff());
		status_bar_1P.set_silence_buff(player_1->check_silence_buff());

		status_bar_2P.set_hp((int)player_2->get_hp());
		status_bar_2P.set_mp((int)player_2->get_mp());
		status_bar_2P.set_hurry_buff(player_2->check_hurry_buff());
		status_bar_2P.set_invisible_buff(player_2->check_invisible_buff());
		status_bar_2P.set_recover_buff(player_2->check_recover_buff());
		status_bar_2P.set_silence_buff(player_2->check_silence_buff());

		if (!is_game_over)
			timer_prop_generator.on_update(delta);
		else
		{
			pos_img_winner_bar.x += (int)(speed_winnner_bar * delta);
			pos_img_winner_text.x += (int)(speed_winnner_text * delta);

			if (!is_slide_out_started)
			{
				timer_winnner_slide_in.on_update(delta);
				if (pos_img_winner_bar.x > pos_x_img_winner_bar_dst)
					pos_img_winner_bar.x = pos_x_img_winner_bar_dst;
				if (pos_img_winner_text.x > pos_x_img_winner_text_dst)
					pos_img_winner_text.x = pos_x_img_winner_text_dst;
			}
			else
				timer_winnner_slide_out.on_update(delta);
		}
	}

	void on_draw(const Camera& camera) 
	{
		putimage_alpha(camera, pos_img_sky.x, pos_img_sky.y, &img_sky);
		putimage_alpha(camera, pos_img_hills.x, pos_img_hills.y, &img_hills);

		for (const Platform& platform : platform_list)
			platform.on_draw(camera);

		player_1->on_draw(camera);
		player_2->on_draw(camera);

		for (const Bullet* bullet : bullet_list)
			bullet->on_draw(camera);
		for (Prop& prop : prop_list)
			prop.on_draw(camera);

		if (is_debug)
		{
			settextcolor(RGB(255, 0, 0));
			outtextxy(15, 15, _T("已开启调试模式，按 'Q' 键关闭"));
		}

		if (is_game_over)
		{
			putimage_alpha(pos_img_winner_bar.x, pos_img_winner_bar.y, &img_winnner_bar);
			putimage_alpha(pos_img_winner_text.x, pos_img_winner_text.y,
				player_1->get_hp() > 0 ? &img_1P_winnner : &img_2P_winnner);
		}
		else
		{
			status_bar_1P.on_draw();
			status_bar_2P.on_draw();
		}
	}

	void on_input(const ExMessage& msg)
	{
		player_1->on_input(msg);
		player_2->on_input(msg);

		switch (msg.message)
		{
		case WM_KEYUP:
			// 'Q'
			if (msg.vkcode == 0x51)
				is_debug = !is_debug;
			break;
		default:
			break;
		}
	}
	
	void on_exit()
	{
		delete player_1; player_1 = nullptr;
		delete player_2; player_2 = nullptr;

		is_debug = false;

		main_camera.reset();
	}

private:
	StatusBar status_bar_1P;
	StatusBar status_bar_2P;
	bool is_game_over = false;
	bool is_slide_out_started = false;
	POINT pos_img_sky = { 0 };
	POINT pos_img_hills = { 0 };
	POINT pos_img_winner_bar = { 0 };
	POINT pos_img_winner_text = { 0 };
	int pos_x_img_winner_bar_dst = 0;
	int pos_x_img_winner_text_dst = 0;
	float speed_winnner_bar = 3.0f;
	float speed_winnner_text = 1.5f;
	Timer timer_prop_generator;
	Timer timer_winnner_slide_in;
	Timer timer_winnner_slide_out;

};

#endif // !_GAME_SCENE_H_
