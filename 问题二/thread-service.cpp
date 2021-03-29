#include <stdio.h>
#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <WS2tcpip.h>
#include <Winsock2.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
#define USERNUM    50		//能保存的最大用户数 及 用户能拥有的最大精灵数 
#define NAMELENGTH 50		//用户名和密码最大长度 
#define EXMAX  4			//经验上限 = 4 * 等级 
#define NPI    10			//非主属性增加 = 10
#define PI	   20			//主属性增加 = 20
#define LMAX   15			//等级上限15 
#define VEX	   2			//胜利获得经验 = 2 * 敌方等级 
#define FEX    1			//失败获得经验 = 1 * 敌方等级 
#define AIR    0.9			//敏捷型精灵升级时攻击间隔减少10%
#define NAIR   0.96			//非敏捷型精灵升级时攻击间隔减少4%
#define MAI    1.0			//敏捷型精灵攻击间隔最小值0.4s 
#define NMAI   1.5			//非敏捷型精灵攻击间隔最小值1s 
#define LVG    50			//肉盾型精灵升级时生命值成长 
#define NLVG   25			//非肉盾型精灵升级时生命值成长 
#define CAT    50 * level + aggressivity * 2					//暴力战猫的攻击伤害 
#define OX     40 * level + aggressivity * 1.2 + HP * 0.08		//狂野金牛的攻击伤害 
#define LUOYIN 30 * level + HPMAX * 0.08 + defense * 0.8		//罗隐的攻击伤害 
#define ODIN   30 * level + HPMAX * 0.12						//奥丁的攻击伤害 
#define DRAGON 25 * level + defense * 0.8						//上古战龙的攻击伤害 
#define ABU    25 * level + defense * 0.4 + aggressivity * 0.8	//超能阿布的攻击伤害 
#define LION   40 * level + aggressivity * 0.4 + HPMAX * 0.04	//银翼狮王的攻击伤害 
#define ORIOLE 40 * level + aggressivity * 0.8 + defense * 0.4 	//自由黄灵的攻击伤害 
using namespace std;
const int PORT = 1234;
#define MaxClient 10
#define MaxBufSize 1024
#define _CRT_SECURE_NO_WARINGS

//基类-精灵
class Spirit {
	public:
		string type;			//宠物种类，标志主属性
		Spirit()	{};			//默认构造函数 
		Spirit(string type);	//构造函数
		~Spirit() 	{};			//析构函数 
		//纯虚函数-攻击方式：攻击时调用 
		virtual int attack() = 0;	
		//纯虚函数-伤害显示：被攻击时调用 
		virtual void injury(int damage) = 0;
		//打印精灵属性
		virtual void show() = 0; 
}; 
Spirit::Spirit(string type) {
	this -> type = type;
}

//派生1：类别 
class Type : public Spirit
{
	public:
		string name;			//精灵名字
		int aggressivity;		//攻击力
		int defense;			//防御力
		int HPMAX;				//满状态生命值
		int HP;					//战斗时生命值变化 
		double cooldown;		//攻击间隔 
		int level;				//等级 
		int EX;					//经验值 
		
		Type()	{};				//默认构造函数 
		Type(string type, string name);		//构造函数	
		~Type()	{};				//析构函数 
		//纯虚函数-攻击方式：攻击时调用 
		virtual int attack();	
		//纯虚函数-伤害显示：被攻击时调用 
		virtual void injury(int damage);
		//打印精灵属性
		virtual void show();	
		//升级函数
		virtual void levelUp(); 
		//战斗结束，判断输赢，增加经验值 
		virtual void battleOver(int enemyLv, int enemyHP);
};
Type::Type(string type, string name) : Spirit(type) {
	this -> level = 1;			//初始等级1 
	this -> EX = 0;				//初始经验0 
	this -> name = name;
	//力量型基础属性 
	if ((name == "暴力战猫") || (name == "狂野金牛")) {
		this -> aggressivity = 30;	
		this -> defense = 10;
		this -> HPMAX = 65;
		this -> HP = this -> HPMAX;
		this -> cooldown = 6;	
	} 
	//肉盾型 
	if ((name == "罗隐") || (name == "奥丁")) {
		this -> aggressivity = 20;	
		this -> defense = 13;
		this -> HPMAX = 105;
		this -> HP = this -> HPMAX;
		this -> cooldown = 8;
	}
	//防御型
	if ((name == "上古战龙") || (name == "超能阿布")) {
		this -> aggressivity = 20;	
		this -> defense = 20;
		this -> HPMAX = 90;
		this -> HP = this -> HPMAX;
		this -> cooldown = 8;
	} 
	//敏捷型
	if ((name == "银翼狮王") || (name == "自由黄灵")) {
		this -> aggressivity = 30;	
		this -> defense = 5;
		this -> HPMAX = 55;
		this -> HP = this -> HPMAX;
		this -> cooldown = 4;
	}
}
int Type::attack() {
	cout << this -> name << "发动攻击：" << endl;
	return 0;
}
//伤害显示函数，受到伤害时调用 
void Type::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//伤害减免 
	HP = HP - damage;		//受伤后的血量变化 
	if (HP < 0)
		HP = 0;				//血量不能低于0 
	//显示伤害 
	cout << this -> name << "受到" << damage << "点伤害，";
	cout << this -> name << "生命值变为" << HP << endl;
	cout << endl;
}
void Type::show() {
	cout << "**************************************************************" << endl;
	cout << "*                 " << this -> name << '\t' << "等级：" << this -> level;
	cout << "                      *" << endl;
	cout << "* 攻击力：" << aggressivity << '\t';
	cout << "防御力：" << defense << '\t';
	cout << "生命值：" << HPMAX << '\t';
	cout << "攻击间隔：" << setprecision(2) << cooldown;
	cout << "  *" << endl;
	cout << "**************************************************************" << endl;
	cout << endl;
}
//升级 
void Type::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//等级提升 
		EX %= (EXMAX * level);			//经验重置 
		cout << "************************************************" << endl;
		cout << this -> name << "等级提升至" << this -> level << "!" << endl;
		//属性提升
		cout << "属性变化：" << endl;
		
		if (type == "力量型") {
			cout << "攻击力：" << aggressivity;
			aggressivity += PI;			//攻击力变化 
			cout << " -> " << aggressivity << endl;
		}
		else {
			cout << "攻击力：" << aggressivity;
			aggressivity += NPI;			//攻击力变化 
			cout << " -> " << aggressivity << endl;
		}
		
		if (type == "防御型") {
			cout << "防御力：" << defense;
			defense += PI;				//防御力变化 
			cout << " -> " << defense << endl;
		}
		else {
			cout << "防御力：" << defense;
			defense += NPI;				//防御力变化 
			cout << " -> " << defense << endl;
		}
		
		if (type == "肉盾型") {
			cout << "生命值：" << HPMAX;	
			HPMAX += LVG;				//生命值变化 
			HP = HPMAX;
			cout << " -> " << HPMAX << endl;
		}
		else {
			cout << "生命值：" << HPMAX;	
			HPMAX += LVG;				//生命值变化 
			HP = HPMAX;
			cout << " -> " << HPMAX << endl;
		}
		
		if (type == "敏捷型") {
			cout << "攻击间隔：" << setprecision(2) << cooldown;
			cooldown *= AIR;
			if (cooldown <= MAI)	//非敏捷型精灵攻击间隔上限为NMAI 
				cooldown = MAI;
			cout << " -> " << setprecision(2) << cooldown << endl;
		}
		else {
			cout << "攻击间隔：" << setprecision(2) << cooldown;
			cooldown *= NAIR;
			if (cooldown <= NMAI)	//非敏捷型精灵攻击间隔上限为NMAI 
				cooldown = NMAI;
			cout << " -> " << setprecision(2) << cooldown << endl;
		}
		
		cout << "************************************************" << endl;
		cout << endl;
	}
}
//战斗结算函数 
void Type::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "************************************************" << endl;
		cout << "               战斗胜利！经验增加：" << VEX * enemyLv << endl;
		cout << "************************************************" << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "************************************************" << endl;
		cout << "               战斗失败！经验增加：" << FEX * enemyLv << endl;
		cout << "************************************************" << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		cout << endl;
	}
}

//派生2：具体精灵
//1.1-力量型，暴力战猫 
class Cat : public Type
{
	public:
		Cat(string type, string name);		//构造函数
		~Cat()	{};			//析构函数 
		//升级函数
		virtual void levelUp(); 
		//战斗结束，判断输赢，增加经验值 
		virtual void battleOver(int enemyLv, int enemyHP);	
		//纯虚函数-攻击方式：攻击时调用 
		virtual int attack();	
		//纯虚函数-伤害显示：被攻击时调用 
		virtual void injury(int damage);
		//打印精灵属性
		virtual void show(); 
		//设置对应等级 
		void setLevel(int prelevel);
};
Cat::Cat(string type, string name) : Type(type, name){
	this -> level = 1;		//初始等级为1
	this -> EX = 0;			//初始经验值为0
}
//升级 
void Cat::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//等级提升 
		EX %= (EXMAX * level);			//经验重置 
		cout << "暴力战猫等级提升至" << level << "!" << endl;
		//属性提升
		cout << "属性变化：" << endl;
		
		cout << "攻击力：" << aggressivity;
		aggressivity += PI;			//攻击力变化 
		cout << " -> " << aggressivity << endl;
		
		cout << "防御力：" << defense;
		defense += NPI;				//防御力变化 
		cout << " -> " << defense << endl;
		
		cout << "生命值：" << HPMAX;	
		HPMAX += NLVG;				//生命值变化 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "攻击间隔：" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//非敏捷型精灵攻击间隔上限为NMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//战斗结算函数 
void Cat::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		cout << endl;
	}
}
//攻击方式函数
int Cat::attack() {
	//显示攻击 
	cout << "暴力战猫的攻击：" << endl;
	cout << "暴力战猫使用了<连续突刺>" << endl;
	//计算伤害 
	int damage = 0;
	damage = CAT;		//技能伤害 
	return damage;
}
//伤害显示函数，受到伤害时调用 
void Cat::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//伤害减免 
	HP = HP - damage;		//受伤后的血量变化 
	if (HP < 0)
		HP = 0;				//血量不能低于0 
	//显示伤害 
	cout << "暴力战猫受到" << damage << "点伤害，";
	cout << "暴力战猫生命值变为" << HP << endl;
	cout << endl;
}
//打印属性 
void Cat::show() {
	cout << "暴力战猫" << '\t' << "等级：" << level << endl;
	cout << "攻击力：" << aggressivity << '\t';
	cout << "防御力：" << defense << '\t';
	cout << "生命值：" << HPMAX << '\t';
	cout << "攻击间隔：" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//设置等级 
void Cat::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//属性直接成长 
	aggressivity += PI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//若是升级，攻击间隔减少 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//若是降级，攻击间隔增加
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//注意攻击间隔上限
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//1.2-力量型，狂野金牛 
class Ox : public Type
{
	private:
		int level;			//等级 
		int EX;				//经验值 
	public:
		Ox(string type, string name);		//构造函数
		~Ox()	{};			//析构函数 
		//升级函数
		virtual void levelUp(); 
		//战斗结束，判断输赢，增加经验值 
		virtual void battleOver(int enemyLv, int enemyHP);	 
		//纯虚函数-攻击方式：攻击时调用 
		virtual int attack();	
		//纯虚函数-伤害显示：被攻击时调用 
		virtual void injury(int damage);
		//打印属性
		virtual void show(); 
		//设置对应等级 
		void setLevel(int prelevel);
};
Ox::Ox(string type, string name) : Type(type, name){
	this -> level = 1;		//初始等级为1
	this -> EX = 0;			//初始经验值为0
}
//升级 
void Ox::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//等级提升 
		EX %= (EXMAX * level);			//经验重置 
		cout << "狂野金牛等级提升至" << level << "!" << endl;
		//属性提升
		cout << "属性变化：" << endl;
		
		cout << "攻击力：" << aggressivity;
		aggressivity += PI;			//攻击力变化 
		cout << " -> " << aggressivity << endl;
		
		cout << "防御力：" << defense;
		defense += NPI;				//防御力变化 
		cout << " -> " << defense << endl;
		
		cout << "生命值：" << HPMAX;	
		HPMAX += NLVG;				//生命值变化 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "攻击间隔：" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//非敏捷型精灵攻击间隔上限为NMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//战斗结算函数 
void Ox::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		cout << endl;
	}
}
//攻击方式函数
int Ox::attack() {
	//显示攻击 
	cout << "狂野金牛的攻击：" << endl;
	cout << "狂野金牛使用了<勇猛精进>" << endl;
	//计算伤害 
	int damage = 0;
	damage = OX;		//基础伤害 
	return damage;
}
//伤害显示函数，受到伤害时调用 
void Ox::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//伤害减免 
	HP = HP - damage;		//受伤后的血量变化 
	if (HP < 0)
		HP = 0;				//血量不能低于0 
	//显示伤害 
	cout << "狂野金牛受到" << damage << "点伤害";
	cout << "狂野金牛生命值变为" << HP << endl;
	cout << endl;
}
//打印属性 
void Ox::show() {
	cout << "狂野金牛" << '\t' << "等级：" << level << endl;
	cout << "攻击力：" << aggressivity << '\t';
	cout << "防御力：" << defense << '\t';
	cout << "生命值：" << HPMAX << '\t';
	cout << "攻击间隔：" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//设置等级 
void Ox::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//属性直接成长 
	aggressivity += PI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//若是升级，攻击间隔减少 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//若是降级，攻击间隔增加
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//注意攻击间隔上限
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//2.1-肉盾型，罗隐
class Luoyin : public Type
{
	private:
		int level;			//等级 
		int EX;				//经验值 
	public:
		Luoyin(string type, string name);		//构造函数
		~Luoyin()	{};			//析构函数 
		//升级函数
		virtual void levelUp(); 
		//战斗结束，判断输赢，增加经验值 
		virtual void battleOver(int enemyLv, int enemyHP);	 
		//纯虚函数-攻击方式：攻击时调用 
		virtual int attack();	
		//纯虚函数-伤害显示：被攻击时调用 
		virtual void injury(int damage);
		//打印属性 
		virtual void show();
		//设置对应等级
		void setLevel(int preLevel); 
};
Luoyin::Luoyin(string type, string name) : Type(type, name){
	this -> level = 1;		//初始等级为1
	this -> EX = 0;			//初始经验值为0
}
//升级 
void Luoyin::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//等级提升 
		EX %= (EXMAX * level);			//经验重置 
		cout << "罗隐等级提升至" << level << "!" << endl;
		//属性提升
		cout << "属性变化：" << endl;
		
		cout << "攻击力：" << aggressivity;
		aggressivity += NPI;			//攻击力变化 
		cout << " -> " << aggressivity << endl;
		
		cout << "防御力：" << defense;
		defense += NPI;				//防御力变化 
		cout << " -> " << defense << endl;
		
		cout << "生命值：" << HPMAX;	
		HPMAX += LVG;				//生命值变化 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "攻击间隔：" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//非敏捷型精灵攻击间隔上限为NMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//战斗结算函数 
void Luoyin::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		cout << endl;
	}
}
//攻击方式函数
int Luoyin::attack() {
	//显示攻击 
	cout << "罗隐的攻击：" << endl;
	cout << "罗隐使用了<车裂>" << endl;
	//计算伤害 
	int damage = 0;
	damage = LUOYIN;		//基础伤害 
	return damage;
}
//伤害显示函数，受到伤害时调用 
void Luoyin::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//伤害减免 
	HP = HP - damage;		//受伤后的血量变化 
	if (HP < 0)
		HP = 0;				//血量不能低于0 
	//显示伤害 
	cout << "罗隐受到" << damage << "点伤害，";
	cout << "罗隐生命值变为" << HP << endl;
	cout << endl;
}
//打印属性 
void Luoyin::show() {
	cout << "罗隐" << '\t' << "等级：" << level << endl;
	cout << "攻击力：" << aggressivity << '\t';
	cout << "防御力：" << defense << '\t';
	cout << "生命值：" << HPMAX << '\t';
	cout << "攻击间隔：" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//设置等级 
void Luoyin::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//属性直接成长 
	aggressivity += NPI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += LVG * gradeDif;
	HP = HPMAX;
	//若是升级，攻击间隔减少 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//若是降级，攻击间隔增加
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//注意攻击间隔上限
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//2.2-肉盾型，奥丁
class Odin : public Type
{
	private:
		int level;			//等级 
		int EX;				//经验值 
	public:
		Odin(string type, string name);		//构造函数
		~Odin()	{};			//析构函数 
		//升级函数
		virtual void levelUp(); 
		//战斗结束，判断输赢，增加经验值 
		virtual void battleOver(int enemyLv, int enemyHP);	 
		//纯虚函数-攻击方式：攻击时调用 
		virtual int attack();	
		//纯虚函数-伤害显示：被攻击时调用 
		virtual void injury(int damage);
		//打印属性
		virtual void show(); 
		//设置对应等级
		void setLevel(int preLevel); 
};
Odin::Odin(string type, string name) : Type(type, name){
	this -> level = 1;		//初始等级为1
	this -> EX = 0;			//初始经验值为0
}
//升级 
void Odin::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//等级提升 
		EX %= (EXMAX * level);			//经验重置 
		cout << "奥丁等级提升至" << level << "!" << endl;
		//属性提升
		cout << "属性变化：" << endl;
		
		cout << "攻击力：" << aggressivity;
		aggressivity += NPI;			//攻击力变化 
		cout << " -> " << aggressivity << endl;
		
		cout << "防御力：" << defense;
		defense += NPI;				//防御力变化 
		cout << " -> " << defense << endl;
		
		cout << "生命值：" << HPMAX;	
		HPMAX += LVG;				//生命值变化 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "攻击间隔：" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//非敏捷型精灵攻击间隔上限为NMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//战斗结算函数 
void Odin::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		cout << endl;
	}
}
//攻击方式函数
int Odin::attack() {
	//显示攻击 
	cout << "奥丁的攻击：" << endl;
	cout << "奥丁使用了<光刺炮>" << endl;
	//计算伤害 
	int damage = 0;
	damage = ODIN;		//基础伤害 
	return damage;
}
//伤害显示函数，受到伤害时调用 
void Odin::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//伤害减免 
	HP = HP - damage;		//受伤后的血量变化 
	if (HP < 0)
		HP = 0;				//血量不能低于0 
	//显示伤害 
	cout << "奥丁受到" << damage << "点伤害，";
	cout << "奥丁生命值变为" << HP << endl;
	cout << endl;
}
//打印属性 
void Odin::show() {
	cout << "奥丁" << '\t' << "等级：" << level << endl;
	cout << "攻击力：" << aggressivity << '\t';
	cout << "防御力：" << defense << '\t';
	cout << "生命值：" << HPMAX << '\t';
	cout << "攻击间隔：" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//设置等级 
void Odin::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//属性直接成长 
	aggressivity += NPI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += LVG * gradeDif;
	HP = HPMAX;
	//若是升级，攻击间隔减少 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//若是降级，攻击间隔增加
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//注意攻击间隔上限
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//3.1-防御型，上古战龙
class Dragon : public Type
{
	private:
		int level;			//等级 
		int EX;				//经验值 
	public:
		Dragon(string type, string name);		//构造函数
		~Dragon()	{};			//析构函数 
		//升级函数
		virtual void levelUp(); 
		//战斗结束，判断输赢，增加经验值 
		virtual void battleOver(int enemyLv, int enemyHP);	 
		//纯虚函数-攻击方式：攻击时调用 
		virtual int attack();	
		//纯虚函数-伤害显示：被攻击时调用 
		virtual void injury(int damage);
		//打印属性
		virtual void show(); 
		//设置对应等级
		void setLevel(int preLevel); 
};
Dragon::Dragon(string type, string name) : Type(type, name){
	this -> level = 1;		//初始等级为1
	this -> EX = 0;			//初始经验值为0
}
//升级 
void Dragon::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//等级提升 
		EX %= (EXMAX * level);			//经验重置 
		cout << "上古战龙等级提升至" << level << "!" << endl;
		//属性提升
		cout << "属性变化：" << endl;
		
		cout << "攻击力：" << aggressivity;
		aggressivity += NPI;			//攻击力变化 
		cout << " -> " << aggressivity << endl;
		
		cout << "防御力：" << defense;
		defense += PI;				//防御力变化 
		cout << " -> " << defense << endl;
		
		cout << "生命值：" << HPMAX;	
		HPMAX += LVG;				//生命值变化 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "攻击间隔：" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//非敏捷型精灵攻击间隔上限为NMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//战斗结算函数 
void Dragon::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		cout << endl;
	}
}
//攻击方式函数
int Dragon::attack() {
	//显示攻击 
	cout << "上古战龙的攻击：" << endl;
	cout << "上古战龙使用了<传说力量>" << endl;
	//计算伤害 
	int damage = 0;
	damage = DRAGON;		//基础伤害 
	return damage;
}
//伤害显示函数，受到伤害时调用 
void Dragon::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//伤害减免 
	HP = HP - damage;		//受伤后的血量变化 
	if (HP < 0)
		HP = 0;				//血量不能低于0 
	//显示伤害 
	cout << "上古战龙受到" << damage << "点伤害，";
	cout << "上古战龙生命值变为" << HP << endl;
	cout << endl;
}
//打印属性 
void Dragon::show() {
	cout << "上古战龙" << '\t' << "等级：" << level << endl;
	cout << "攻击力：" << aggressivity << '\t';
	cout << "防御力：" << defense << '\t';
	cout << "生命值：" << HPMAX << '\t';
	cout << "攻击间隔：" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//设置等级 
void Dragon::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//属性直接成长 
	aggressivity += NPI * gradeDif;
	defense += PI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//若是升级，攻击间隔减少 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//若是降级，攻击间隔增加
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//注意攻击间隔上限
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//3.2-防御型，超能阿布
class Abu : public Type
{
	private:
		int level;			//等级 
		int EX;				//经验值 
	public:
		Abu(string type, string name);		//构造函数
		~Abu()	{};			//析构函数 
		//升级函数
		virtual void levelUp(); 
		//战斗结束，判断输赢，增加经验值 
		virtual void battleOver(int enemyLv, int enemyHP);	 
		//纯虚函数-攻击方式：攻击时调用 
		virtual int attack();	
		//纯虚函数-伤害显示：被攻击时调用 
		virtual void injury(int damage);
		//打印属性
		virtual void show(); 
		//设置对应等级
		void setLevel(int preLevel); 
};
Abu::Abu(string type, string name) : Type(type, name){
	this -> level = 1;		//初始等级为1
	this -> EX = 0;			//初始经验值为0
}
//升级 
void Abu::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//等级提升 
		EX %= (EXMAX * level);			//经验重置 
		cout << "超能阿布等级提升至" << level << "!" << endl;
		//属性提升
		cout << "属性变化：" << endl;
		
		cout << "攻击力：" << aggressivity;
		aggressivity += NPI;			//攻击力变化 
		cout << " -> " << aggressivity << endl;
		
		cout << "防御力：" << defense;
		defense += PI;				//防御力变化 
		cout << " -> " << defense << endl;
		
		cout << "生命值：" << HPMAX;	
		HPMAX += NLVG;				//生命值变化 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "攻击间隔：" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//非敏捷型精灵攻击间隔上限为NMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//战斗结算函数 
void Abu::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		cout << endl;
	}
}
//攻击方式函数
int Abu::attack() {
	//显示攻击 
	cout << "超能阿布的攻击：" << endl;
	cout << "超能阿布使用了<龙之爪牙>" << endl;
	//计算伤害 
	int damage = 0;
	damage = ABU;		//基础伤害 
	return damage;
}
//伤害显示函数，受到伤害时调用 
void Abu::injury(int damage) {
//	cout << ((double)defense / (double)(100 + defense)) << endl;
	damage = damage * ((double)defense / (double)(50 + defense));	//伤害减免 
	HP = HP - damage;		//受伤后的血量变化 
	if (HP < 0)
		HP = 0;				//血量不能低于0 
	//显示伤害 
	cout << "超能阿布受到" << damage << "点伤害，";
	cout << "超能阿布生命值变为" << HP << endl;
	cout << endl;
}
//打印属性 
void Abu::show() {
	cout << "超能阿布" << '\t' << "等级：" << level << endl;
	cout << "攻击力：" << aggressivity << '\t';
	cout << "防御力：" << defense << '\t';
	cout << "生命值：" << HPMAX << '\t';
	cout << "攻击间隔：" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//设置等级 
void Abu::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//属性直接成长 
	aggressivity += NPI * gradeDif;
	defense += PI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//若是升级，攻击间隔减少 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//若是降级，攻击间隔增加
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//注意攻击间隔上限
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//4.1-敏捷型，银翼狮王
class Lion : public Type
{
	private:
		int level;			//等级 
		int EX;				//经验值 
	public:
		Lion(string type, string name);		//构造函数
		~Lion()	{};			//析构函数 
		//升级函数
		virtual void levelUp(); 
		//战斗结束，判断输赢，增加经验值 
		virtual void battleOver(int enemyLv, int enemyHP);	 
		//纯虚函数-攻击方式：攻击时调用 
		virtual int attack();	
		//纯虚函数-伤害显示：被攻击时调用 
		virtual void injury(int damage);
		//打印属性
		virtual void show(); 
		//设置对应等级
		void setLevel(int preLevel); 
};
Lion::Lion(string type, string name) : Type(type, name){
	this -> level = 1;		//初始等级为1
	this -> EX = 0;			//初始经验值为0
}
//升级 
void Lion::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//等级提升 
		EX %= (EXMAX * level);			//经验重置 
		cout << "银翼狮王等级提升至" << level << "!" << endl;
		//属性提升
		cout << "属性变化：" << endl;
		
		cout << "攻击力：" << aggressivity;
		aggressivity += NPI;			//攻击力变化 
		cout << " -> " << aggressivity << endl;
		
		cout << "防御力：" << defense;
		defense += NPI;				//防御力变化 
		cout << " -> " << defense << endl;
		
		cout << "生命值：" << HPMAX;	
		HPMAX += NLVG;				//生命值变化 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "攻击间隔：" << setprecision(2) << cooldown;
		cooldown *= AIR;
		if (cooldown <= MAI)	//非敏捷型精灵攻击间隔上限为MAI 
			cooldown = MAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//战斗结算函数 
void Lion::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		cout << endl;
	}
}
//攻击方式函数
int Lion::attack() {
	//显示攻击 
	cout << "银翼狮王的攻击：" << endl;
	cout << "银翼狮王使用了<沙尘狂风>" << endl;
	//计算伤害 
	int damage = 0;
	damage = LION;		//基础伤害 
	return damage;
}
//伤害显示函数，受到伤害时调用 
void Lion::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//伤害减免 
	HP = HP - damage;		//受伤后的血量变化 
	if (HP < 0)
		HP = 0;				//血量不能低于0 
	//显示伤害 
	cout << "银翼狮王受到" << damage << "点伤害，";
	cout << "银翼狮王生命值变为" << HP << endl;
	cout << endl;
}
//打印属性 
void Lion::show() {
	cout << "银翼狮王" << '\t' << "等级：" << level << endl;
	cout << "攻击力：" << aggressivity << '\t';
	cout << "防御力：" << defense << '\t';
	cout << "生命值：" << HPMAX << '\t';
	cout << "攻击间隔：" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//设置等级 
void Lion::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//属性直接成长 
	aggressivity += NPI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//若是升级，攻击间隔减少 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//若是降级，攻击间隔增加
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//注意攻击间隔上限
	if (cooldown <= MAI)
		cooldown = MAI;
}

//4.2-敏捷型，自由黄灵
class Oriole : public Type
{
	private:
		int level;			//等级 
		int EX;				//经验值 
	public:
		Oriole(string type, string name);		//构造函数
		~Oriole()	{};			//析构函数 
		//升级函数
		virtual void levelUp(); 
		//战斗结束，判断输赢，增加经验值 
		virtual void battleOver(int enemyLv, int enemyHP);	 
		//纯虚函数-攻击方式：攻击时调用 
		virtual int attack();	
		//纯虚函数-伤害显示：被攻击时调用 
		virtual void injury(int damage);
		//打印精灵属性 
		virtual void show();
		//设置对应等级
		void setLevel(int preLevel); 
};
Oriole::Oriole(string type, string name) : Type(type, name){
	this -> level = 1;		//初始等级为1
	this -> EX = 0;			//初始经验值为0
}
//升级 
void Oriole::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//等级提升 
		EX %= (EXMAX * level);			//经验重置 
		cout << "自由黄灵等级提升至" << level << "!" << endl;
		//属性提升
		cout << "属性变化：" << endl;
		
		cout << "攻击力：" << aggressivity;
		aggressivity += NPI;			//攻击力变化 
		cout << " -> " << aggressivity << endl;
		
		cout << "防御力：" << defense;
		defense += NPI;				//防御力变化 
		cout << " -> " << defense << endl;
		
		cout << "生命值：" << HPMAX;	
		HPMAX += NLVG;				//生命值变化 
		HP = HPMAX;
		cout << " -> " << HPMAX << endl;
		
		cout << "攻击间隔：" << setprecision(2) << cooldown;
		cooldown *= AIR;
		if (cooldown <= MAI)	//敏捷型精灵攻击间隔上限为MAI 
			cooldown = MAI;
		cout << " -> " << setprecision(2) << cooldown;
	}
}
//战斗结算函数 
void Oriole::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
	}
}
//攻击方式函数
int Oriole::attack() {
	//显示攻击 
	cout << "自由黄灵的攻击：" << endl;
	cout << "自由黄灵使用了<魔力旋风>" << endl;
	//计算伤害 
	int damage = 0;
	damage = ORIOLE;		//基础伤害 
	return damage;
}
//伤害显示函数，受到伤害时调用 
void Oriole::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//伤害减免 
	HP = HP - damage;		//受伤后的血量变化 
	if (HP < 0)
		HP = 0;				//血量不能低于0 
	//显示伤害 
	cout << "自由黄灵受到" << damage << "点伤害，";
	cout << "自由黄灵生命值变为" << HP << endl;
	cout << endl;
}
//打印属性 
void Oriole::show() {
	cout << "自由黄灵" << '\t' << "等级：" << level << endl;
	cout << "攻击力：" << aggressivity << '\t';
	cout << "防御力：" << defense << '\t';
	cout << "生命值：" << HPMAX << '\t';
	cout << "攻击间隔：" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//设置等级 
void Oriole::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//属性直接成长 
	aggressivity += NPI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//若是升级，攻击间隔减少 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//若是降级，攻击间隔增加
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//注意攻击间隔上限
	if (cooldown <= MAI)
		cooldown = MAI;
}

//用户账号 
typedef struct accout {
	char username[NAMELENGTH];	//用户名
	char password[NAMELENGTH];	//密码 
	int logCnt;					//记录登录次数 
	int onlineOrNot;			//1在线 0不在线 
	
	Type my_spirit[USERNUM];	//用户的精灵 
}accout;
accout users[USERNUM];	//用户结构数组
int usercnt = 0;		//记录用户个数 

//服务线程
DWORD WINAPI ServerThread(LPVOID lpParameter){
	SOCKET *ClientSocket=(SOCKET*)lpParameter;
	int receByt=0;
	char RecvBuf[MaxBufSize];
	char SendBuf[MaxBufSize];
	//记录用户注册序号 
	int logSrNo = 0;
	while(1){
		/*登录界面菜单*/ 
LOGMENU:
		char recvBuf[4024] = {};
		char logMenu[4024] = {"\n************************************\n\
*             1、注册              *\n\
*             2、登录              *\n\
*             3、注销              *\n\
*             4、退出              *\n\
************************************\n\
          请输入你的选择：\n"};
		int sLen = send(*ClientSocket, logMenu, strlen(logMenu), 0);
		cout << logMenu << endl; 
		//用户输入选项
		int reLen = recv(*ClientSocket, recvBuf, 4024, 0);	//阻塞函数，等待接收数据
		
		if (SOCKET_ERROR == sLen) {
			cout << "服务器发送数据失败！" << endl;
		} 
		else {
			cout << recvBuf << endl;
			//1、注册 
			if (recvBuf[0] == '1') {
				strcpy(recvBuf, "***************注册*****************\n请输入要注册的用户名：");
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				
				recv(*ClientSocket, recvBuf, 4024, 0);		//收到客户端发来的用户名
				cout << recvBuf << endl;		//公屏输出用户名 
				int username_flag = 0;			//0代表未被注册，1代表已被注册 
				for (int i = 0; i < usercnt; i++) {
					if (strcmp(recvBuf, users[i].username) == 0)
						username_flag = 1;
				}
				//告诉客户端该用户名注册与否 
				char flagBuf[4];
				if (username_flag == 1) {
					strcpy(flagBuf, "1");
				}
				else {
					strcpy(flagBuf, "0");
				}
				send(*ClientSocket, flagBuf, 4, 0);
				//未被注册则进行注册 
				if (username_flag == 0) {
					strcpy(users[usercnt].username, recvBuf);	//保存用户名 
					
					strcpy(recvBuf, "请输入你的密码：");
					send(*ClientSocket, recvBuf, 4024, 0);			//提示输入密码 
					cout << recvBuf;
					
					//收到客户端发来的密码
					recv(*ClientSocket, recvBuf, 4024, 0); 
					strcpy(users[usercnt++].password, recvBuf);	//保存密码 
					
//					for (int i = 0; i < usercnt; i++) {
//						cout << users[i].username;
//						cout << "：" << users[i].password;
//					}
					
					cout << recvBuf << endl;		//公屏输出密码 
					
					strcpy(recvBuf, "注册成功！\n");
					users[usercnt].logCnt = 0;		//登录次数置0 
					users[usercnt].onlineOrNot = 0;	//不在线 
					send(*ClientSocket, recvBuf, 4024, 0);
					cout << recvBuf << endl;
					
					goto LOGMENU;
				}
				//已被注册则返回菜单并提示
				else {
					strcpy(recvBuf, "非常抱歉，该用户名已被注册，请重新选择。");
					send(*ClientSocket, recvBuf, 4024, 0);		//发送重名提示
					cout << recvBuf << endl;
					
					goto LOGMENU;
				}
			}
			//2、登录
			if (recvBuf[0] == '2') {
				strcpy(recvBuf, "***************登录*****************\n请输入用户名：");
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				
				recv(*ClientSocket, recvBuf, 4024, 0);		//收到客户端发来的用户名
				cout << recvBuf << endl;		//公屏输出用户名 
				int username_flag = 0;			//0代表未被注册，1代表已被注册 
				for (int i = 0; i < usercnt; i++) {
					if (strcmp(recvBuf, users[i].username) == 0) {
						username_flag = 1;
						logSrNo = i;			//记录用户在数组的序号 
						break;
					}
				}
				
				//是否可登录发送给客户端 
				char flagBuf[4];
				if (username_flag == 1) {
					strcpy(flagBuf, "1");
				}
				else {
					strcpy(flagBuf, "0");
				}
				send(*ClientSocket, flagBuf, 4, 0);
				
				if (username_flag == 1) {
					strcpy(recvBuf, "请输入密码：");
					send(*ClientSocket, recvBuf, 4024, 0);			//提示输入密码 
					cout << recvBuf;
					
					//收到客户端发来的密码
					recv(*ClientSocket, recvBuf, 4024, 0); 
					cout << recvBuf << endl;
					
					//密码正确 
					if (strcmp(recvBuf, users[logSrNo].password) == 0) {
						//目前不在线则登录成功 
						if (users[logSrNo].onlineOrNot == 0) {		
							strcpy(recvBuf, "登录成功！\n");
							users[logSrNo].onlineOrNot = 1;		//置1表示在线 
							cout << recvBuf << endl;
							send(*ClientSocket, recvBuf, 4024, 0);
							//登录次数+1 
							users[logSrNo].logCnt++;
						}
						//若在线则登录失败
						else {
							strcpy(recvBuf, "当前账号已被登录，请稍候再尝试！\n");
							cout << recvBuf << endl;
							send(*ClientSocket, recvBuf, 4024, 0); 
							
							goto LOGMENU;
						} 
					}
					//密码错误 
					else {
						strcpy(recvBuf, "用户名或密码错误，请重新选择！\n");
						cout << recvBuf << endl;
						send(*ClientSocket, recvBuf, 4024, 0);
						
						goto LOGMENU;
					}
				}
				//未注册则返回菜单并提示
				else {
					strcpy(recvBuf, "非常抱歉，该用户名未注册，请重新选择。");
					send(*ClientSocket, recvBuf, 4024, 0);		//发送重名提示
					cout << recvBuf << endl;
					
					goto LOGMENU;
				}
			}
			//3、注销
			if (recvBuf[0] == '3') {
				strcpy(recvBuf, "请输入要注销的用户名：");
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl;
				
				recv(*ClientSocket, recvBuf, 4024, 0);	//接收到用户名
				cout << recvBuf << endl;		//公屏输出用户名 
				int username_flag = 0;			//0代表未被注册，1代表已被注册 
				int srNo = 0;
				for (int i = 0; i < usercnt; i++) {
					if (strcmp(recvBuf, users[i].username) == 0) {
						username_flag = 1;
						srNo = i;			//记录用户在数组的序号 
						break;
					}
				}
				
				//是否可注销发送给客户端 
				char flagBuf[4];
				if (username_flag == 1) {
					strcpy(flagBuf, "1");
				}
				else {
					strcpy(flagBuf, "0");
				}
				send(*ClientSocket, flagBuf, 4, 0);
				
				if (username_flag == 1) {
					strcpy(recvBuf, "请输入登录密码：");
					send(*ClientSocket, recvBuf, 4024, 0);			//提示输入密码 
					cout << recvBuf;
					
					//收到客户端发来的密码
					recv(*ClientSocket, recvBuf, 4024, 0); 
					cout << recvBuf << endl;
					
					//密码正确 
					if (strcmp(recvBuf, users[srNo].password) == 0) {
						//注销账号
						if (srNo != usercnt) {
							for (int i = srNo; i < usercnt - 1; i++) {
								strcpy(users[srNo].password, users[srNo + 1].password);
								strcpy(users[srNo].username, users[srNo + 1].username);
							}
							usercnt--;
						}
						else 
							usercnt--;
						
						strcpy(recvBuf, "注销成功！\n");
						cout << recvBuf << endl;
						send(*ClientSocket, recvBuf, 4024, 0);
						
						goto LOGMENU;		//返回登录菜单 
					}
					//密码错误 
					else {
						strcpy(recvBuf, "用户名或密码错误，请重新选择！\n");
						cout << recvBuf << endl;
						send(*ClientSocket, recvBuf, 4024, 0);
						
						goto LOGMENU;
					}
				}
				//用户名未注册 
				else {
					strcpy(recvBuf, "非常抱歉，该用户名未注册，请重新选择。");
					cout << recvBuf << endl;
					send(*ClientSocket, recvBuf, 4024, 0);
					
					goto LOGMENU;
				}
			} 
			//4、退出 
			if (recvBuf[0] == '4') {
				break;
			}
			
			/*用户登录*/
			//服务端告知客户端该用户为第几次登陆 
			char logCnt[4];
			if (users[logSrNo].logCnt == 1)
				strcpy(logCnt, "1");
			else 
				strcpy(logCnt, "0");
			send(*ClientSocket, logCnt, 4, 0);
			
			//用户注册后第一次登陆，服务器向用户分配精灵
			if (users[logSrNo].logCnt == 1) {
				
				srand((int)time(0));		//产生随机种子
				//数字对应八种精灵，以此分配 
				int flag[8] = {0, 0, 0, 0, 0, 0, 0, 0};	//为防止重复给用户精灵，判断是否重复 
				for (int i = 0; i < 3; i++) {
					switch ((rand() % 8 + 1)) {
						case 1:	{
								if (flag[0] == 0) {
									Type s1("力量型", "暴力战猫");
									users[logSrNo].my_spirit[i] = s1;
									flag[0] = 1;
									break;	
								}
								else {
									i--;		//重新选择，i回退1 
									break;
								}
						}
				//				mine.my_spirit.show();
						case 2:	{
								if (flag[1] == 0) {
									Type s2("力量型", "狂野金牛");
									users[logSrNo].my_spirit[i] = s2;
									flag[1] = 1;
									break;
								}
								else {
									i--;		//重新选择，i回退1 
									break;
								}
						}
				//				mine.my_spirit.show();
						case 3:	{
								if (flag[2] == 0) {
									Type s3("肉盾型", "罗隐");
									users[logSrNo].my_spirit[i] = s3;
									break;
								}
								else {
									i--;		//重新选择，i回退1 
									break;
								}
						}
						case 4:	{
								if (flag[3] == 0) {
									Type s4("肉盾型", "奥丁");
									users[logSrNo].my_spirit[i] = s4;
									flag[3] = 1;
									break;
								}
								else {
									i--;		//重新选择，i回退1 
									break;
								}
						}
						case 5:	{
								if (flag[4] == 0) {
									Type s5("防御型", "上古战龙");
									users[logSrNo].my_spirit[i] = s5;
									flag[4] = 1;
									break;
								}
								else {
									i--;
									break;
								}
						}
						case 6:	{
								if (flag[5] == 0) {
									Type s6("防御型", "超能阿布");
									users[logSrNo].my_spirit[i] = s6;
									flag[5] = 1;
									break;
								}
								else {
									i--;
									break;
								}
						}
						case 7:	{
								if (flag[6] == 0) {
									Type s7("敏捷型", "银翼狮王");
									users[logSrNo].my_spirit[i] = s7;
									flag[6] = 1;
									break;
								}
								else {
									i--;
									break;
								}
						}
						case 8:	{
								if (flag[7] == 0) {
									Type s8("敏捷型", "自由黄灵");
									users[logSrNo].my_spirit[i] = s8;
									flag[7] = 1;
									break;
								}
								else {
									i--;
									break;
								}
						}
				}
			}
				
				//发送三种精灵到客户端
				for (int i = 0; i < 3; i++) {
					strcpy(recvBuf, users[logSrNo].my_spirit[i].type.c_str());
					send(*ClientSocket, recvBuf, 4024, 0);
					//若传出有误
					char type_flag[4];
					recv(*ClientSocket, type_flag, 4, 0);		//收到客户端反馈
					while (type_flag[0] != '1') {
						strcpy(recvBuf, users[logSrNo].my_spirit[i].type.c_str());
						send(*ClientSocket, recvBuf, 4024, 0);
						recv(*ClientSocket, type_flag, 4, 0);
					}
					
					strcpy(recvBuf, users[logSrNo].my_spirit[i].name.c_str());
					send(*ClientSocket, recvBuf, 4024, 0);
				}
				//服务器打印 
				users[logSrNo].my_spirit[0].show();
				users[logSrNo].my_spirit[1].show();
				users[logSrNo].my_spirit[2].show();
			}
			
			//用户菜单
USERMENU:
			char userMenu[4024] = {"\n************************************\n\
*         1、查看我的精灵          *\n\
*         2、查看所有用户状态      *\n\
*         3、搜索用户              *\n\
*         4、退出登录              *\n\
************************************\n\
	    请输入你的选择：\n"};
	        send(*ClientSocket, userMenu, 4024, 0);
			cout << userMenu << endl; 
			//用户输入选项
			recv(*ClientSocket, recvBuf, 4024, 0);	//阻塞函数，等待接收数据
			cout << recvBuf << endl;		//输出用户选项
			
			//1、查看我的精灵
			if (recvBuf[0] == '1') {
				//发送三种精灵到客户端
				for (int i = 0; i < 3; i++) {
					strcpy(recvBuf, users[logSrNo].my_spirit[i].type.c_str());
					send(*ClientSocket, recvBuf, 4024, 0);
					//若传出有误
					char type_flag[4];
					recv(*ClientSocket, type_flag, 4, 0);		//收到客户端反馈
					while (type_flag[0] != '1') {
						strcpy(recvBuf, users[logSrNo].my_spirit[i].type.c_str());
						send(*ClientSocket, recvBuf, 4024, 0);
						recv(*ClientSocket, type_flag, 4, 0);
					}
					strcpy(recvBuf, users[logSrNo].my_spirit[i].name.c_str());
					send(*ClientSocket, recvBuf, 4024, 0);
				}
				//服务器打印 
				users[logSrNo].my_spirit[0].show();
				users[logSrNo].my_spirit[1].show();
				users[logSrNo].my_spirit[2].show();
				
				goto USERMENU;
			}
			
			//2、查看所有用户状态 
			if (recvBuf[0] == '2') {
				strcpy(recvBuf, "用户列表：\n");	
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				for (int i = 0; i < usercnt; i++) {
					strcpy(recvBuf, users[i].username);		//用户名 
					send(*ClientSocket, recvBuf, 4024, 0);
					cout << recvBuf << "——";
					strcpy(recvBuf, "——");
					send(*ClientSocket, recvBuf, 4024, 0);
					//在线状态 
					if (users[i].onlineOrNot == 1) {
						strcpy(recvBuf, "在线");			//输出在线 
						send(*ClientSocket, recvBuf, 4024, 0);
						cout << recvBuf << endl;
					}
					else {
						strcpy(recvBuf, "离线");			//输出离线 
						send(*ClientSocket, recvBuf, 4024, 0);
						cout << recvBuf << endl;
					}
				}
				strcpy(recvBuf, "");
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << endl;
				
				goto USERMENU;
			}
			
			//3、搜索用户
			if (recvBuf[0] == '3') {
				strcpy(recvBuf, "输入你要搜索的用户名：");
				int found_flag = 0;			//1找到，0未找到 
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				recv(*ClientSocket, recvBuf, 4024, 0);		//接收客户端输入的用户名 
				cout << recvBuf << endl;
				//查找该用户 
				for (int i = 0; i < usercnt; i++) {
					if (strcmp(recvBuf, users[i].username) == 0) {	//匹配上 
						found_flag = 1;
						strcpy(recvBuf, "1");		
						send(*ClientSocket, recvBuf, 4024, 0);		//若找到，告知客户端 
						cout << recvBuf << endl;
						
						/*输出用户在线状态*/
						strcpy(recvBuf, users[i].username);			//用户名 
						send(*ClientSocket, recvBuf, 4024, 0);
						cout << recvBuf << "——";
						strcpy(recvBuf, "——");
						send(*ClientSocket, recvBuf, 4024, 0);
						//在线状态 
						if (users[i].onlineOrNot == 1) {
							strcpy(recvBuf, "在线");			//输出在线 
							send(*ClientSocket, recvBuf, 4024, 0);
							cout << recvBuf << endl;
						}
						else {
							strcpy(recvBuf, "离线");			//输出离线 
							send(*ClientSocket, recvBuf, 4024, 0);
							cout << recvBuf << endl;
						}
						
						/*输出该用户拥有的精灵*/
						//若该用户仅注册而未登陆过，即没有领取系统送的三只精灵
						if (users[i].logCnt == 0) {
							strcpy(recvBuf, "0");
							send(*ClientSocket, recvBuf, 4024, 0);	//告知客户端无精灵
						} 
						//若该用户拥有精灵 
						else {
							strcpy(recvBuf, "1");
							send(*ClientSocket, recvBuf, 4024, 0);	//告知客户端有精灵
							//发送三种精灵到客户端
							for (int j = 0; j < 3; j++) {
								strcpy(recvBuf, users[i].my_spirit[j].type.c_str());
								send(*ClientSocket, recvBuf, 4024, 0);
								//若传出有误
								char type_flag[4];
								recv(*ClientSocket, type_flag, 4, 0);		//收到客户端反馈
								while (type_flag[0] != '1') {
									strcpy(recvBuf, users[i].my_spirit[j].type.c_str());
									send(*ClientSocket, recvBuf, 4024, 0);
									recv(*ClientSocket, type_flag, 4, 0);
								}
								strcpy(recvBuf, users[i].my_spirit[j].name.c_str());
								send(*ClientSocket, recvBuf, 4024, 0);
							}
							//服务器打印 
							users[i].my_spirit[0].show();
							users[i].my_spirit[1].show();
							users[i].my_spirit[2].show();
						}
					}
				}
				//未找到 
				if (found_flag == 0) {
					strcpy(recvBuf, "0");		
					send(*ClientSocket, recvBuf, 4024, 0);		//若未找到，告知客户端 
				}
				
				cout << endl;
				goto USERMENU; 
			}
			
			//4、退出登录
			if (recvBuf[0] == '4') {
				cout << "退出登录！" << endl;
				users[logSrNo].onlineOrNot = 0;
				goto LOGMENU;
			} 
			 
//			cout << "请求命令：   " << endl;
//			if (strcmp("cls", recvBuf) == 0) {
//				//服务端执行命令
//				system(recvBuf); 
//			}
//			else if (strcmp("获取版本信息", recvBuf) == 0) {
//				//返回数据
//				string verData = "Version: 1.0.1\nAuthor: Primer\nReleaseData: 2020-9-3";
//				int sLen = send(recvClientSocket, (char*)verData.c_str(), verData.length(), 0); 
//			} 
//			else if (strcmp("exit", recvBuf) == 0) {
//				cout << endl << "退出服务器" << endl;
//				break;
//			}
//			else {
//				cout << "\t不正确..." << endl;
//			}
//			cout << endl;
		}
	}//while
	closesocket(*ClientSocket);
	free(ClientSocket);
	return 0;
}

int main(){
	WORD myVersionRequest;
	WSAData wsd;
	myVersionRequest=MAKEWORD(2,2);
	int err;
	err=WSAStartup(myVersionRequest,&wsd);
	if (!err)
    {
    	printf("已打开套接字\n");            
    }
    else
    {
        //进一步绑定套接字
        printf("嵌套字未打开!");
    }//创建套接字，socket前的一些检查工作，包括服务的启动,通过WSACleanup的返回值来确定socket协议是否启动
	SOCKET ListenSocket = socket(AF_INET,SOCK_STREAM,0);//创建了可识别套接字
	SOCKADDR_IN ListenAddr;
	ListenAddr.sin_family=AF_INET;
	ListenAddr.sin_addr.S_un.S_addr=INADDR_ANY;//表示填入本机ip地址 
	ListenAddr.sin_port=htons(PORT);//绑定端口
	//要绑定的参数
	int n;
	n=bind(ListenSocket,(LPSOCKADDR)&ListenAddr,sizeof(ListenAddr));//绑定完成
	if(n==SOCKET_ERROR){
		printf("端口绑定失败！\n");
		return -1;
	}
	else{
		printf("端口绑定成功：%d\n",PORT);
	}
	
	int l =listen(ListenSocket,20);//其中第二个参数代表能够接收的最多的连接数
	printf("服务端准备就绪，等待连接请求\n");
	
	while(1){
		//循环接收客户端连接请求并创建服务线程
		SOCKET *ClientSocket = new SOCKET;
		ClientSocket=(SOCKET*)malloc(sizeof(SOCKET));
		//接收客户端连接请求
		int SockAddrlen = sizeof(sockaddr);
		*ClientSocket = accept(ListenSocket,0,0);
		printf("一个客户端已连接到服务器，socket是：%d\n",*ClientSocket);
		CreateThread(NULL,0,&ServerThread,ClientSocket,0,NULL);
	}//while
	closesocket(ListenSocket);
	WSACleanup();
	return(0);
}//main

