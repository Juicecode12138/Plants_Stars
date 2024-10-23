#ifndef _STATUS_BAR_H_
#define _STATUS_BAR_H_

#include "util.h"

extern IMAGE img_buff_icon_hurry;
extern IMAGE img_buff_icon_invisible;
extern IMAGE img_buff_icon_recover;
extern IMAGE img_buff_icon_silence;

class StatusBar
{
public:
	StatusBar() = default;
	~StatusBar() = default;

	void set_avatar(IMAGE* img)
	{
		img_avatar = img;
	}

	void set_position(int x, int y)
	{
		position.x = x, position.y = y;
	}

	void set_hp(int val)
	{
		hp = val;
	}

	void set_mp(int val)
	{
		mp = val;
	}

	void set_hurry_buff(bool flag)
	{
		has_hurry_buff = flag;
	}

	void set_invisible_buff(bool flag)
	{
		has_invisible_buff = flag;
	}

	void set_recover_buff(bool flag)
	{
		has_recover_buff = flag;
	}

	void set_silence_buff(bool flag)
	{
		has_silence_buff = flag;
	}

	void on_draw()
	{
		int icon_cursor_pos_x = position.x + 100;	// BUFF 图标渲染水平位置指针
		const int icon_step_x = 60 + 15;			// 图标水平摆放步长
		const int icon_pos_y = position.y - 60;		// 图标渲染竖直位置

		// 根据当前状态栏记录的 BUFF 状态渲染对应的图标
		if (has_hurry_buff)
		{
			putimage_alpha(icon_cursor_pos_x, icon_pos_y, &img_buff_icon_hurry);
			icon_cursor_pos_x += icon_step_x;	// 向右排列摆放
		}
		if (has_invisible_buff)
		{
			putimage_alpha(icon_cursor_pos_x, icon_pos_y, &img_buff_icon_invisible);
			icon_cursor_pos_x += icon_step_x;
		}
		if (has_recover_buff)
		{
			putimage_alpha(icon_cursor_pos_x, icon_pos_y, &img_buff_icon_recover);
			icon_cursor_pos_x += icon_step_x;
		}
		if (has_silence_buff)
		{
			putimage_alpha(icon_cursor_pos_x, icon_pos_y, &img_buff_icon_silence);
			icon_cursor_pos_x += icon_step_x;
		}

		putimage_alpha(position.x, position.y, img_avatar);	// 渲染玩家头像

		setfillcolor(RGB(5, 5, 5));
		solidroundrect(position.x + 100, position.y + 10, position.x + 100 + width + 3 * 2, position.y + 36, 8, 8);
		solidroundrect(position.x + 100, position.y + 45, position.x + 100 + width + 3 * 2, position.y + 71, 8, 8);
		setfillcolor(RGB(67, 47, 47));
		solidroundrect(position.x + 100, position.y + 10, position.x + 100 + width + 3, position.y + 33, 8, 8);
		solidroundrect(position.x + 100, position.y + 45, position.x + 100 + width + 3, position.y + 68, 8, 8);

		float hp_bar_width = width * max(0, hp) / 100.0f;
		float mp_bar_width = width * min(100, mp) / 100.0f;
		setfillcolor(RGB(197, 61, 67));
		solidroundrect(position.x + 100, position.y + 10, position.x + 100 + (int)hp_bar_width + 3, position.y + 33, 8, 8);
		setfillcolor(RGB(83, 131, 195));
		solidroundrect(position.x + 100, position.y + 45, position.x + 100 + (int)mp_bar_width + 3, position.y + 68, 8, 8);
	}

private:
	const int width = 275;

private:
	int hp = 0;
	int mp = 0;
	POINT position = { 0 };
	IMAGE* img_avatar = nullptr;
	bool has_hurry_buff = false;
	bool has_invisible_buff = false;
	bool has_recover_buff = false;
	bool has_silence_buff = false;

};

#endif // !_STATUS_BAR_H_
