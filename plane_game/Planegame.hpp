#pragma once
#include<windows.h>
#include<iostream>
#include<vector>
using	namespace std;

#define WINDOW_WIDTH	400							
#define WINDOW_HEIGHT	600							
#define WINDOW_TITLE	L"plane"
#define MENU_ITEM_WIDTH 150
#define MENU_ITEM_HEIGHT 60

#define TIMERID 10
#define GAMEFPS 25
#include<gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"Msimg32.lib")
using namespace Gdiplus;

typedef enum GAMESCENE//游戏状态
{
	SCENE_START,
	SCENE_PLAY,
	SCENE_OVER
}GAMESCENE;
enum NPCTYPE
{
	NPC_SMALL,
	NPC_MEDIUM,
	NPC_BIG
};
typedef struct MenuItem//菜单属性
{
	TCHAR ItemText[5];
	RectF Itemrect;
	bool isFocus;
}MenuItem;
typedef struct NPC//敌机属性
{
	int type;
	POINT pos;
	int restblood;
	int state;
}NPC;
typedef struct Bullet
{
	int type;
	POINT pos;
	int hurt;
	int speed;
}Bullet;

typedef struct Player
{
	POINT pos;
	HBITMAP hbmap;
	SIZE size;
}Player;
typedef struct NPCStaticInfo
{
	HBITMAP npcbitmap;
	SIZE size;
	int speed;
	int blood;
}NPCStaticInfo;
struct Award
{
	int type;
	int state;
	POINT pos;
	HBITMAP hbmap;
	SIZE size;
};
typedef struct ImgInfo
{
	HBITMAP hmap;
	SIZE size;
}ImgInfo;
enum AWARDTYPE
{
	ONE_BULLET,
	TWO_BULLET
};
GAMESCENE CurGamescene;//当前游戏状态
NPCTYPE npctype;
MenuItem startmenu[2];
MenuItem overmenu[2];
NPCStaticInfo npcstaticinfo[3];
NPC npc;
Bullet bullet;
AWARDTYPE awardtype;
Player player;
Award award;
vector <NPC> npcvector;
vector <Bullet> bulletvector;
vector<Award> awardvector;
int frameindex;
HBITMAP g_hBitmap_start, g_hBitmap_play, g_hBitmap_over;
POINT playerprePos = { 0,0 };
ImgInfo npcImginfo[3], playerImginfo, bulletinfo[2], awardinfo[2];
void LoadImg();
void MouseClick(HWND hwnd, int x, int y);
void GameInit();
void GamePaint(HDC hdc);
void ReleaseRes();
void DleImage();
void GameLogic();
void Menuclick(HWND hwnd, MenuItem menu[]);
void DrawStartOver(HDC hdc, HBITMAP hmap, MenuItem menu[]);
void DrawPlay(HDC hdc);
void DrawMenu(HDC hdc, LPCWSTR text, RectF rect, Color fontcolor);
void InitMenu();
void MouseCallback(int x,int y, MenuItem menu[]);
BOOL isPointInRect(POINT pt, RectF rectf);
void MenuCallback(int x, int y);
void UpdateNPCPos();//更新敌机位置
void ProduceNPC(int type);
void LoadImgInfo(Bitmap *bmap, TCHAR *imgpath, ImgInfo* ImgInfo);
void InitPlayer();
void UpdatePlayerPos(int x,int y);
bool isCollsionWithRect(Rect rect1, Rect rect2);
void CollisionDetection();
void ProduceBulet(int type);
void UpdateBulletPos();
void ProduceAward(int type);
void UpdateAwardPos();
void LoadImg()
{
	Bitmap *bmap = Bitmap::FromFile(L"img//first.png");
	bmap->GetHBITMAP(NULL, &g_hBitmap_start);

	bmap = Bitmap::FromFile(L"img//back.png");
	bmap->GetHBITMAP(NULL, &g_hBitmap_play);

	bmap = Bitmap::FromFile(L"img//over.png");
	bmap->GetHBITMAP(NULL, &g_hBitmap_over);

	for (size_t i = 0; i < 3; i++)
	{
		TCHAR imgpath[30];
		wsprintf(imgpath, L"img//npc%d.png", i + 1);
		LoadImgInfo(bmap, imgpath, &npcImginfo[i]);
	}
	TCHAR imgpath[30] = L"img//player.png";
	LoadImgInfo(bmap, imgpath, &playerImginfo);
	for (size_t i = 0; i < 2; i++)
	{
		TCHAR imgpath[30];
		wsprintf(imgpath, L"img//bullet%d.png", i + 1);
		LoadImgInfo(bmap, imgpath, &bulletinfo[i]);
	}
	for (size_t i = 0; i < 2; i++)
	{
		TCHAR imgpath[30];
		wsprintf(imgpath, L"img//award%d.png", i + 1);
		LoadImgInfo(bmap, imgpath, &awardinfo[i]);
	}
	delete bmap;
	bmap = NULL;
}
void LoadImgInfo(Bitmap *bmap, TCHAR *imgpath, ImgInfo* ImgInfo)
{

	bmap = Bitmap::FromFile(imgpath);
	bmap->GetHBITMAP(NULL, &ImgInfo->hmap);
	ImgInfo->size.cx = bmap->GetWidth();
	ImgInfo->size.cy = bmap->GetHeight();
}
void InitNpcStaticInfo()
{
	for (int i = 0; i < 3; i++)
	{
		npcstaticinfo[i].npcbitmap = npcImginfo[i].hmap;
		npcstaticinfo[i].size.cx = npcImginfo[i].size.cx / 4;
		npcstaticinfo[i].size.cy = npcImginfo[i].size.cy;
		npcstaticinfo[i].speed = (3 - i) * 2;
		npcstaticinfo[i].blood = (i + 1) * 20;
	}
}
void InitPlayer()
{
	player.hbmap = playerImginfo.hmap;
	player.pos.x = (WINDOW_WIDTH - playerImginfo.size.cx / 5) / 2;
	player.pos.y = 450;
	player.size.cx = playerImginfo.size.cx / 5;
	player.size.cy = playerImginfo.size.cy;
}
void GameInit()
{
	srand(GetTickCount());
	LoadImg();
	InitMenu();
	InitNpcStaticInfo();
	CurGamescene = SCENE_START;

}
void ProduceNPC(int type)
{
	npc.type = type;
	npc.pos.x = rand() % (WINDOW_WIDTH - npcstaticinfo[type].size.cx);
	npc.pos.y = -npcstaticinfo[type].size.cy;
	npc.restblood = npcstaticinfo[type].blood;
	npc.state = 0;
	npcvector.push_back(npc);
}
void ProduceBulet(int type)
{
	bullet.type = type;
	bullet.hurt = (type + 1) * 10;
	bullet.pos.x = (LONG)(player.pos.x + player.size.cx*0.5);
	bullet.pos.y = player.pos.y;
	bullet.speed = 10;
	bulletvector.push_back(bullet);
}
void ProduceAward(int type)
{
	award.type = type;
	award.hbmap = awardinfo[type].hmap;
	award.pos.x = rand() % (WINDOW_WIDTH - awardinfo[type].size.cx);
	award.pos.y = -awardinfo[type].size.cy;
	award.state = 0;
	awardvector.push_back(award);
}
void GameLogic()
{
	if (CurGamescene == SCENE_PLAY)
	{
		frameindex++;
		if (frameindex % 10 == 0)
		{
			ProduceBulet(awardtype);
		}
		if (frameindex % 45 == 0)
		{
			ProduceNPC(NPCTYPE::NPC_SMALL);
		}
		if (frameindex % 150 == 0)
		{
			ProduceNPC(NPCTYPE::NPC_MEDIUM);
		}
		if (frameindex % 300 == 0)
		{
			ProduceAward(1);
		}
		if (frameindex % 400 == 0)
		{
			ProduceNPC(NPCTYPE::NPC_BIG);
		}
		if (frameindex % 500 == 0)
		{
			awardtype = ONE_BULLET;
			frameindex = 0;
		}
		UpdateNPCPos();
		UpdateBulletPos();
		UpdateAwardPos();
		CollisionDetection();
	}

}
void CollisionDetection()
{
	Rect Playerrect;
	Playerrect.Width = (INT)(0.8*player.size.cx);
	Playerrect.Height = (INT)(0.8*player.size.cy);
	Playerrect.X = (INT)(player.pos.x + 0.1*player.size.cx);
	Playerrect.Y = (INT)(player.pos.y + 0.1*player.size.cy);
	vector<NPC>::iterator iter;
	vector<Bullet>::iterator iter2;
	vector<Award>::iterator iter3;
	for (iter3 = awardvector.begin(); iter3 != awardvector.end(); iter3++)
	{
		Rect Awardrect;
		Awardrect.X = (INT)(iter3->pos.x + 0.1*awardinfo[iter3->type].size.cx);
		Awardrect.Y = (INT)(iter3->pos.y + 0.1*awardinfo[iter3->type].size.cy);
		Awardrect.Width = (INT)(0.8*awardinfo[iter3->type].size.cx);
		Awardrect.Height = (INT)(0.8*awardinfo[iter3->type].size.cy);
		if (isCollsionWithRect(Playerrect, Awardrect))
		{
			awardtype = TWO_BULLET;
		}
	}
	for (iter = npcvector.begin(); iter != npcvector.end(); iter++)
	{
		Rect NPCrect;

		NPCrect.X = (INT)(iter->pos.x + 0.1*npcstaticinfo[iter->type].size.cx);
		NPCrect.Y = (INT)(iter->pos.y + 0.1*npcstaticinfo[iter->type].size.cy);
		NPCrect.Width = (INT)(0.8*npcstaticinfo[iter->type].size.cx);
		NPCrect.Height = (INT)(0.8*npcstaticinfo[iter->type].size.cy);
		if (isCollsionWithRect(Playerrect, NPCrect))
		{
			CurGamescene = SCENE_OVER;
			break;
		}
		for (iter2 = bulletvector.begin(); iter2 != bulletvector.end();)
		{
			Rect Bulletrect;
			Bulletrect.X = (INT)(iter2->pos.x);
			Bulletrect.Y = (INT)(iter2->pos.y);
			Bulletrect.Width = (INT)(bulletinfo[iter2->type].size.cx);
			Bulletrect.Height = (INT)(bulletinfo[iter2->type].size.cy);
			if (isCollsionWithRect(Bulletrect, NPCrect))
			{
				iter->restblood -= iter2->hurt;
				iter2=bulletvector.erase(iter2);
			}
			else
				iter2++;
		}
		
	}
}
bool isCollsionWithRect(Rect rect1, Rect rect2)
{
	if (rect1.X >= rect2.X && rect1.X >= rect2.X + rect2.Width) {
		return false;
	}
	else if (rect1.X <= rect2.X && rect1.X + rect1.Width <= rect2.X) {
		return false;
	}
	else if (rect1.Y >= rect2.Y && rect1.Y >= rect2.Y + rect2.Height) {
		return false;
	}
	else if (rect1.Y <= rect2.Y  && rect1.Y + rect1.Height <= rect2.Y) {
		return false;
	}
	return true;
}

void UpdateNPCPos()
{
	vector<NPC>::iterator iter;
	for (iter = npcvector.begin(); iter != npcvector.end();)
	{
		iter->pos.y += npcstaticinfo[iter->type].speed;
		if (iter->restblood == 0)
		{
			(iter->state)++;
		}

		if (iter->pos.y > WINDOW_HEIGHT|| iter->state==3)
		{
			iter = npcvector.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}
void UpdateAwardPos()
{
	vector<Award>::iterator iter;
	for (iter  = awardvector.begin();  iter!=awardvector.end();)
	{
		iter->pos.y += 5;
		if (awardtype==TWO_BULLET)
		{
			iter = awardvector.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}
void UpdateBulletPos()
{
	vector<Bullet>::iterator iter;
	for (iter = bulletvector.begin(); iter != bulletvector.end();)
	{
		iter->pos.y -= bullet.speed;
		if (iter->pos.y < 0)
		{
			iter = bulletvector.erase(iter);
		}
		else
			iter++;
	}
}
void GamePaint(HDC hdc)
{

	switch (CurGamescene)
	{
	case SCENE_START:
		DrawStartOver(hdc, g_hBitmap_start, startmenu);

		break;
	case SCENE_PLAY:
		DrawPlay(hdc);
		break;
	case SCENE_OVER:
		DrawStartOver(hdc, g_hBitmap_over, overmenu);
		break;
	default:
		break;
	}
}
void DrawStartOver(HDC hdc, HBITMAP hmap, MenuItem menu[])
{
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hgmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);//
	SelectObject(hdcMem, hgmap);
	HDC hdcMemTemp = CreateCompatibleDC(hdc);
	//贴背景
	SelectObject(hdcMemTemp, hmap);
	BitBlt(hdcMem, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcMemTemp, 0, 0, SRCCOPY);
	//菜单

	for (size_t i = 0; i < 2; i++)
	{
		if (menu[i].isFocus)
		{
			DrawMenu(hdcMem, menu[i].ItemText, menu[i].Itemrect, Color::Gray);
		}
		else
			DrawMenu(hdcMem, menu[i].ItemText, menu[i].Itemrect, Color::White);
	}

	BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcMem, 0, 0, SRCCOPY);    //采用BitBlt函数贴图，参数设置为窗口大小  

																			  //ReleaseDC(hwnd, hdc);  //释放设备环境
	DeleteObject(hgmap);
	DeleteDC(hdcMem);
	DeleteDC(hdcMemTemp);

}
void DrawMenu(HDC hdc, LPCWSTR text, RectF rect, Color fontcolor)
{
	Graphics graph(hdc);
	FontFamily fontFamily(L"黑体");
	Font font(&fontFamily, 17, FontStyleBold, UnitPoint);
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	SolidBrush solidbrush(fontcolor);
	graph.SetTextRenderingHint(TextRenderingHintAntiAlias);
	graph.DrawString(text, (INT)-1, &font, rect, &format, &solidbrush);
	graph.ReleaseHDC(hdc);
}
void InitMenu()
{
	memset(startmenu, 0, sizeof(startmenu));
	memset(overmenu, 0, sizeof(overmenu));

	TCHAR startmenuText[2][5] = { L"开始游戏",L"退出游戏" };
	TCHAR overmenuText[2][5] = { L"重新开始",L"退出游戏" };


	for (size_t i = 0; i < 2; i++)
	{
		lstrcpy(startmenu[i].ItemText, startmenuText[i]);
		lstrcpy(overmenu[i].ItemText, overmenuText[i]);

		RectF ItemRect = { (WINDOW_WIDTH - MENU_ITEM_WIDTH) / 2,320.0f + (MENU_ITEM_HEIGHT + 50)*i,MENU_ITEM_WIDTH,MENU_ITEM_HEIGHT };
		startmenu[i].Itemrect = ItemRect;
		overmenu[i].Itemrect = ItemRect;

		startmenu[i].isFocus = FALSE;
		overmenu[i].isFocus = FALSE;

	}

}
void MenuCallback(int x, int y)
{
	switch (CurGamescene)
	{
	case SCENE_START:
		MouseCallback(x, y, startmenu);
		break;
	case SCENE_PLAY:

		break;
	case SCENE_OVER:
		MouseCallback(x, y, overmenu);
		break;
	default:
		break;
	}
}
void MouseCallback(int x, int y, MenuItem menu[])
{
	for (size_t i = 0; i < 2; i++)
	{
		if (isPointInRect({ x, y }, menu[i].Itemrect))
			menu[i].isFocus = TRUE;
		else
			menu[i].isFocus = FALSE;
	}
}
BOOL isPointInRect(POINT pt, RectF rectf)
{
	RECT rect;
	rect.left = (LONG)rectf.X;
	rect.top = (LONG)rectf.Y;
	rect.right = (LONG)(rectf.X + rectf.Width);
	rect.bottom = (LONG)(rectf.Y + rectf.Height);
	if (PtInRect(&rect, pt))
	{
		return TRUE;
	}

	return FALSE;
}
void DrawPlay(HDC hdc)
{
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hgmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);//
	SelectObject(hdcMem, hgmap);

	HDC hdcMemTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcMemTemp, g_hBitmap_play);
	BitBlt(hdcMem, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcMemTemp, 0, 0, SRCCOPY);

	//SelectObject(hdcMemTemp, npc);
	//BitBlt(hdcMem, 200, 0, 50, 40, hdcMemTemp, 0, 0, SRCCOPY);
	BLENDFUNCTION dc_bf;
	dc_bf.AlphaFormat = AC_SRC_ALPHA;
	dc_bf.BlendFlags = 0;
	dc_bf.BlendOp = AC_SRC_OVER;
	dc_bf.SourceConstantAlpha = 230;//图片透明度
	vector<NPC>::iterator iter;
	for (iter = npcvector.begin(); iter != npcvector.end(); iter++)
	{
		SelectObject(hdcMemTemp, npcstaticinfo[iter->type].npcbitmap);
		AlphaBlend(hdcMem, iter->pos.x, iter->pos.y, npcstaticinfo[iter->type].size.cx, npcstaticinfo[iter->type].size.cy, hdcMemTemp, (iter->state)*npcstaticinfo[iter->type].size.cx, 0, npcstaticinfo[iter->type].size.cx, npcstaticinfo[iter->type].size.cy, dc_bf);
	}
	vector<Bullet>::iterator iter2;
	for (iter2 = bulletvector.begin(); iter2 != bulletvector.end(); iter2++)
	{
		SelectObject(hdcMemTemp, bulletinfo[iter2->type].hmap);
		AlphaBlend(hdcMem, iter2->pos.x, iter2->pos.y, bulletinfo[iter2->type].size.cx, bulletinfo[iter2->type].size.cy, hdcMemTemp, 0, 0, bulletinfo[iter2->type].size.cx, bulletinfo[iter2->type].size.cy, dc_bf);
	}
	vector<Award>::iterator iter3;
	for (iter3 = awardvector.begin(); iter3 != awardvector.end(); iter3++)
	{
		SelectObject(hdcMemTemp, awardinfo[iter3->type].hmap);
		AlphaBlend(hdcMem, iter3->pos.x, iter3->pos.y, awardinfo[iter3->type].size.cx, awardinfo[iter3->type].size.cy, hdcMemTemp, 0, 0, awardinfo[iter3->type].size.cx, awardinfo[iter3->type].size.cy, dc_bf);
	}
	SelectObject(hdcMemTemp, player.hbmap);
	AlphaBlend(hdcMem, player.pos.x, player.pos.y, player.size.cx, player.size.cy, hdcMemTemp, 0, 0, player.size.cx, player.size.cy, dc_bf);


	BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcMem, 0, 0, SRCCOPY);

	DeleteObject(hgmap);
	DeleteDC(hdcMem);
	DeleteDC(hdcMemTemp);

}

void ReleaseRes()
{
	DleImage();

}
void DleImage()
{
	DeleteObject(g_hBitmap_start);
	DeleteObject(g_hBitmap_play);
	DeleteObject(g_hBitmap_over);
	for (int i = 0; i < 3; i++)
	{
		DeleteObject(npcImginfo[i].hmap);
	}
	for (int i = 0; i < 2; i++)
	{
		DeleteObject(bulletinfo[i].hmap);
		DeleteObject(awardinfo[i].hmap);
	}
}
void MouseClick(HWND hwnd, int x, int y)
{
	switch (CurGamescene)
	{
	case SCENE_START:
		Menuclick(hwnd, startmenu);
		break;
	case SCENE_PLAY:
	{
		playerprePos.x = x;
		playerprePos.y = y;

	}
	break;
	case SCENE_OVER:
		Menuclick(hwnd, overmenu);
		break;
	default:
		break;
	}
}
void Menuclick(HWND hwnd, MenuItem menu[])
{

	if (menu[0].isFocus)
	{
		InitPlayer();
		npcvector.clear();
		bulletvector.clear();
		frameindex = 0;
		CurGamescene = SCENE_PLAY;
	}
	if (menu[1].isFocus)
	{
		SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
	}
}
void UpdatePlayerPos(int x, int y)
{
	playerprePos.x = player.pos.x + player.size.cx / 2;
	playerprePos.y = player.pos.y + player.size.cy / 2;
	if (CurGamescene == SCENE_PLAY)
	{
		int dx = x - playerprePos.x;
		int dy = y - playerprePos.y;
		if (playerprePos.x + dx<player.size.cx / 4 || playerprePos.x + dx>WINDOW_WIDTH - player.size.cx/4)
		{
			dx = 0;
		}
		if (playerprePos.y + dy<player.size.cy / 2 || playerprePos.y + dy>WINDOW_HEIGHT - player.size.cy / 2)
		{
			dy = 0;
		}
		player.pos.x += dx;
		player.pos.y += dy;
		playerprePos.x = x;
		playerprePos.y = y;
	}

}
 