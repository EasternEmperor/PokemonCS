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
#define DODGE  10			//基础闪避率，敏捷型精灵闪避率 = DODGE + level，上限25% 
#define MDL    25			//闪避上限25% 
#define CRIT   10			//基础暴击几率，力量型精灵暴击几率 = CRIT + level * 2，上限40%
#define CHL    40			//暴击上限40% 
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
		int crit;				//代表暴击几率
		int dodge;				//代表闪避几率 
		
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
		virtual int battleOver(int enemyLv, int enemyHP);
		//设置等级
		void setLevel(int preLevel);
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
	//使用随机数来判断是否闪避：随机数落在dodge间则闪避 
	srand((int)time(0));		//产生随机种子
	//闪避 
	int pro = rand() % 100 + 1;
	if (pro <= this -> dodge + 20 && pro > 20) {
		cout << this -> name << "完美闪避了这次攻击！" << endl << endl;
	}
	//承受伤害 
	else {
		damage = damage * ((double)defense / (double)(50 + defense));	//伤害减免 
		HP = HP - damage;		//受伤后的血量变化 
		if (HP < 0)
			HP = 0;				//血量不能低于0 
		//显示伤害 
		cout << this -> name << "受到" << damage << "点伤害，";
		cout << this -> name << "生命值变为" << HP << endl;
		cout << endl;
	}
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
int Type::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "************************************************" << endl;
		cout << "               战斗胜利！经验增加：" << VEX * enemyLv << endl;
		cout << "************************************************" << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		cout << endl;
		
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "************************************************" << endl;
		cout << "               战斗失败！经验增加：" << FEX * enemyLv << endl;
		cout << "************************************************" << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		cout << endl;
		
		return 0;
	}
}
//设置等级 
void Type::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//属性直接成长 	
	if (type == "力量型") {
		aggressivity += PI * gradeDif;			//攻击力变化 
	}
	else {
		aggressivity += NPI * gradeDif;			//攻击力变化 
	}
	
	if (type == "防御型") {
		defense += PI * gradeDif;				//防御力变化 
	}
	else {
		defense += NPI * gradeDif;				//防御力变化 
	}
	
	if (type == "肉盾型") {
		HPMAX += LVG * gradeDif;				//生命值变化 
		HP = HPMAX;
	}
	else {	
		HPMAX += LVG * gradeDif;				//生命值变化 
		HP = HPMAX;
	}
	
	if (type == "敏捷型") {
		//若是升级 
		if (gradeDif >= 0) {
			cooldown *= pow(AIR, gradeDif);
			if (cooldown <= MAI)	//非敏捷型精灵攻击间隔上限为MAI 
				cooldown = MAI;
		}
		//若是降级 
		else {
			cooldown /= pow(AIR, gradeDif);
			if (cooldown <= MAI)	//非敏捷型精灵攻击间隔上限为MAI 
				cooldown = MAI;
		}
	}
	else {
		//若是升级 
		if (gradeDif >= 0) {
			cooldown *= pow(NAIR, gradeDif);
			if (cooldown <= NMAI)	//非敏捷型精灵攻击间隔上限为MAI 
				cooldown = NMAI;
		}
		//若是降级 
		else {
			cooldown /= pow(NAIR, gradeDif);
			if (cooldown <= NMAI)	//非敏捷型精灵攻击间隔上限为MAI 
				cooldown = NMAI;
		}
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
		virtual int battleOver(int enemyLv, int enemyHP);	
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
int Cat::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		return 0;
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
	
	//使用随机数来判断是否暴击：随机数落在crit间则暴击 
	srand((int)time(0));		//产生随机种子
	//暴击 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "此次攻击发生暴击！伤害加倍！" << endl << endl;
		damage *= 2;
	}
	
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
		virtual int battleOver(int enemyLv, int enemyHP);	 
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
int Ox::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		return 0;
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
	
	//使用随机数来判断是否暴击：随机数落在crit间则暴击 
	srand((int)time(0));		//产生随机种子
	//暴击 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "此次攻击发生暴击！伤害加倍！" << endl << endl;
		damage *= 2;
	}
	
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
		virtual int battleOver(int enemyLv, int enemyHP);	 
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
int Luoyin::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		return 0;
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
	
	//使用随机数来判断是否暴击：随机数落在0-crit间则暴击 
	srand((int)time(0));		//产生随机种子
	//暴击 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "此次攻击发生暴击！伤害加倍！" << endl << endl;
		damage *= 2;
	}
	
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
		virtual int battleOver(int enemyLv, int enemyHP);	 
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
int Odin::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		return 0;
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
	
	//使用随机数来判断是否暴击：随机数落在0-crit间则暴击 
	srand((int)time(0));		//产生随机种子
	//暴击 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "此次攻击发生暴击！伤害加倍！" << endl << endl;
		damage *= 2;
	}
	
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
		virtual int battleOver(int enemyLv, int enemyHP);	 
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
int Dragon::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		return 0;
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
	
	//使用随机数来判断是否暴击：随机数落在0-crit间则暴击 
	srand((int)time(0));		//产生随机种子
	//暴击 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "此次攻击发生暴击！伤害加倍！" << endl << endl;
		damage *= 2;
	}
	
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
		virtual int battleOver(int enemyLv, int enemyHP);	 
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
int Abu::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		return 0;
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
	
	//使用随机数来判断是否暴击：随机数落在0-crit间则暴击 
	srand((int)time(0));		//产生随机种子
	//暴击 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "此次攻击发生暴击！伤害加倍！" << endl << endl;
		damage *= 2;
	}
	
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
		virtual int battleOver(int enemyLv, int enemyHP);	 
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
int Lion::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		return 0;
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
	
	//使用随机数来判断是否暴击：随机数落在0-crit间则暴击 
	srand((int)time(0));		//产生随机种子
	//暴击 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "此次攻击发生暴击！伤害加倍！" << endl << endl;
		damage *= 2;
	}
	
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
		virtual int battleOver(int enemyLv, int enemyHP);	 
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
int Oriole::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//胜利 
		cout << "战斗胜利！经验增加" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//失败 
		cout << "战斗失败！经验增加" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//战斗结束后的生命值回复
		return 0;
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
	
	//使用随机数来判断是否暴击：随机数落在0-crit间则暴击 
	srand((int)time(0));		//产生随机种子
	//暴击 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "此次攻击发生暴击！伤害加倍！" << endl << endl;
		damage *= 2;
	}
	
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
class accout {
	public:
		char username[NAMELENGTH];	//用户名
		char password[NAMELENGTH];	//密码 
		int logCnt;					//记录登录次数 
		int onlineOrNot;			//1在线 0不在线 
		int spiritcnt;				//记录精灵个数 
		int lV15;					//15级精灵个数 
		
		Type my_spirit;				//用户的精灵 
};

int main()
{
	cout << "------------客户端------------" << endl;
	
	//1 初始化
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	
	//2 创建套接字
	SOCKET clientSocket = {};
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == clientSocket) {
		cout << "套接字创建失败！" << endl;
	} 
	else {
		cout << "套接字创建成功！" << endl;
	}
	
	//3 绑定套接字：指定绑定的IP地址和端口号
	sockaddr_in socketAddr;
	socketAddr.sin_family = PF_INET;
	socketAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	socketAddr.sin_port = htons(1234);
	int cRes = connect(clientSocket, (SOCKADDR *)&socketAddr, sizeof(SOCKADDR));
	if (SOCKET_ERROR == cRes) {
		cout << "客户端：\t\t与服务器连接失败！" << endl;
	} 
	else {
		cout << "客户端：\t\t与服务器连接成功！" << endl;
	}
	
	while (true) {
		//4 发送请求
//		char sendBuf[1024] = "from Client:     Hello service!";
//		send(clientSocket, sendBuf, strlen(sendBuf), 0);
	
//		string s;
//		cout << "输入发送数据：";
//		getline(cin, s);		//getline：可输入空格，默认以换行符结束
//		send(clientSocket, (char *)s.c_str(), (int)s.length(), 0); 
		 
		//因为recv接收函数是阻塞函数，所以要加以判断
		//请求正确我才接收数据，否则不影响我继续请求
//		if (strcmp("获取版本信息", s.c_str()) == 0) {
//			char recvBuf[4024] = {};
//			int reLen = recv(clientSocket, recvBuf, 4024, 0);	//recv函数，等待接收数据 
//			cout << endl << recvBuf << endl << endl;
//		} 
		//5 发送/接收 数据
	//	char recvBuf[1024] = {};
	//	recv(clientSocket, recvBuf, 1024, 0);
	//	cout << "客户端接收数据：" << recvBuf << endl << endl;
	
		//接收到菜单 
LOGMENU:
		char recvBuf[4024] = {};
		recv(clientSocket, recvBuf, 4024, 0);
		system("pause");
		system("cls");			//清理无关输出 	
		cout << recvBuf << endl << endl;
		//用户发送选项请求
		cin >> recvBuf;
		send(clientSocket, recvBuf, 4024, 0);
		
		//1、注册
		if (recvBuf[0] == '1') {
			//接收到服务端注册提示
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
			//读取用户输入的用户名
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);
			
			recv(clientSocket, recvBuf, 4024, 0);
			//进行注册 
			if (recvBuf[0] == '0') {
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				
				cin >> recvBuf;		//用户密码
				send(clientSocket, recvBuf, 4024, 0);	//发送密码 
				//注册成功
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl; 
				
				goto LOGMENU;
			}
			//已被注册
			else {
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl << endl;
				goto LOGMENU;		//回到登录菜单 
			} 
		}
		//2、登录
		if (recvBuf[0] == '2') {
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
			//读取用户输入的用户名 
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);	//发送给服务器 
			
			recv(clientSocket, recvBuf, 4024, 0);
			//可登录则进行登录
			if (recvBuf[0] == '1') {
				recv(clientSocket, recvBuf, 4024, 0);	//提示登录
				cout << recvBuf; 
				cin >> recvBuf;			//读取用户输入的密码
				send(clientSocket, recvBuf, 4024, 0);	//发送密码
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl;
				//密码错误 
				if (strcmp(recvBuf, "登录成功！\n") != 0) {
					goto LOGMENU;
				} 
			}
			//未注册
			else {
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl;
				
				goto LOGMENU;
			} 
		} 
		//3、注销
		if (recvBuf[0] == '3') {
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
			//读取用户输入的用户名 
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);	//发送给服务器 
			
			recv(clientSocket, recvBuf, 4024, 0);
			//可注销则进行注销 
			if (recvBuf[0] == '1') {
				recv(clientSocket, recvBuf, 4024, 0);	//提示输入登录密码
				cout << recvBuf; 
				cin >> recvBuf;			//读取用户输入的密码
				send(clientSocket, recvBuf, 4024, 0);	//发送密码
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl;
				//无论注销成功与否，都返回登录菜单 
				goto LOGMENU; 
			} 
			//未注册
			else {
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl;
				
				goto LOGMENU;
			} 
		}  
		//4、退出
		if (recvBuf[0] == '4') {
			break;
		} 
		
		/*用户登录后*/
		//用户第一次登陆
		recv(clientSocket, recvBuf, 4024, 0);
		if (recvBuf[0] == '1') {
			cout << "你拥有以下三只初始精灵：" << endl;
			for (int i = 0; i < 3; i++) {
				char type[4024];
				recv(clientSocket, type, 4024, 0);
				//若传入有误，则请求重传 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "力量型") != 0 && strcmp(type, "肉盾型") != 0 && 
						strcmp(type, "防御型") != 0 && strcmp(type, "敏捷型") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//重新传入
						recv(clientSocket, type, 4024, 0);
					}
					//传入无误 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				char name[4024];
				recv(clientSocket, name, 4024, 0);
				//创建对象
				Type mirSpirit(type, name);
				mirSpirit.show();
			}
		} 
		
		
USERMENU:
		recv(clientSocket, recvBuf, 4024, 0);
		system("pause");
		system("cls");			//清理无关输出
		cout << recvBuf << endl << endl;
		//用户发送选项请求
		cin >> recvBuf;
		send(clientSocket, recvBuf, 4024, 0);
		
		//1、查看我的精灵
		if (recvBuf[0] == '1') {
			//接收用户精灵个数 
			recv(clientSocket, recvBuf, 4024, 0);
			int spiritcnt = atoi(recvBuf);
			for (int i = 0; i < spiritcnt; i++) {
				char type[4024];
				recv(clientSocket, type, 4024, 0);
				//若传入有误，则请求重传 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "力量型") != 0 && strcmp(type, "肉盾型") != 0 && 
						strcmp(type, "防御型") != 0 && strcmp(type, "敏捷型") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//重新传入
						recv(clientSocket, type, 4024, 0);
					}
					//传入无误 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				//名字 
				char name[4024];
				recv(clientSocket, name, 4024, 0);
				//等级 
				recv(clientSocket, recvBuf, 4024, 0);
				int level = atoi(recvBuf);
				//创建对象
				Type mirSpirit(type, name);
				mirSpirit.setLevel(level);
				//打印 
				mirSpirit.show();
			}
			
			goto USERMENU;
		} 
		
		//2、查看用户状态
		if (recvBuf[0] == '2') {
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
			recv(clientSocket, recvBuf, 4024, 0);	//接受用户名 
			//输出用户 
			while (strcmp(recvBuf, "") != 0) {
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);	//“——”
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);	//在线状态
				cout << recvBuf << endl; 
				recv(clientSocket, recvBuf, 4024, 0);	//接受用户名 / 结束符号"" 
			}
			
			cout << endl;
			goto USERMENU; 
		}
		
		//3、搜索用户
		if (recvBuf[0] == '3') {
			recv(clientSocket, recvBuf, 4024, 0);		//提示输入
			cout << recvBuf;
//			cout << "输入你要搜索的用户名："; 
			cin >> recvBuf;			//读取用户输入的用户名
			send(clientSocket, recvBuf, 4024, 0);
			//判断是否找到
			recv(clientSocket, recvBuf, 4024, 0);
			if (recvBuf[0] == '1') {	//查找到 
				/*输出用户在线状态*/
				recv(clientSocket, recvBuf, 4024, 0);	//接受用户名 
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);	//“——”
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);	//在线状态
				cout << recvBuf << endl; 
				
				/*用户荣誉*/
				//宠物数量
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl; 
				//宠物15级数量 
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl; 
				
				/*输出该用户拥有的精灵*/
				recv(clientSocket, recvBuf, 4024, 0);	//判断该用户是否有精灵 
				//若该用户没有精灵
				if (recvBuf[0] == '0') {
					cout << "该用户暂时未拥有精灵。" << endl;
				}
				//若该用户拥有精灵
				else {
					//接收用户精灵个数 
					recv(clientSocket, recvBuf, 4024, 0);
					int spiritcnt = atoi(recvBuf);
					for (int i = 0; i < spiritcnt; i++) {
						char type[4024];
						recv(clientSocket, type, 4024, 0);
						//若传入有误，则请求重传 
						char type_flag[4];
						strcpy(type_flag, "0");
						while (type_flag[0] != '1') {
							if (strcmp(type, "力量型") != 0 && strcmp(type, "肉盾型") != 0 && 
								strcmp(type, "防御型") != 0 && strcmp(type, "敏捷型") != 0) {
								send(clientSocket, type_flag, 4, 0);
								
								//重新传入
								recv(clientSocket, type, 4024, 0);
							}
							//传入无误 
							else {
								strcpy(type_flag, "1");
								send(clientSocket, type_flag, 4, 0);
							}
						}
						char name[4024];
						recv(clientSocket, name, 4024, 0);
						//创建对象
						Type mirSpirit(type, name);
						mirSpirit.show();
					}
				} 
			} 
			else {
				cout << "未查找到该用户！" << endl;
			}	
			
			cout << endl;
			goto USERMENU; 
		} 
		
		//5、退出登录
		if (recvBuf[0] == '5') {
			cout << "退出登录！" << endl;
			goto LOGMENU;
		} 
		
		//4、虚拟决斗 
FIGHT:
		recv(clientSocket, recvBuf, 4024, 0);
		system("pause");
		system("cls");			//清理无关输出
		cout << recvBuf << endl << endl;
		//用户发送选项请求
		cin >> recvBuf;
		send(clientSocket, recvBuf, 4024, 0);
		
		//升级赛 
		if (recvBuf[0] == '1') {
			//给出对战精灵列表 - 客户端输出 
			system("pause");
			system("cls");
			cout << "************************************************************" << endl;
			cout << "1.暴力战猫" << endl;
			cout << "2.狂野金牛" << endl;
			cout << "3.罗隐" << endl;
			cout << "4.奥丁" << endl;
			cout << "5.上古战龙" << endl;
			cout << "6.超能阿布" << endl;
			cout << "7.银翼狮王" << endl;
			cout << "8.自由黄灵" << endl;
			cout << "************************************************************" << endl;
			cout << "输入对应序号选择你的对手吧：" << endl; 
			//用户发送选项请求
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);
			//创建敌方精灵对象 
			Type allSpirits[8];
			Type s1("力量型", "暴力战猫");
			allSpirits[0] = s1;
			Type s2("力量型", "狂野金牛");
			allSpirits[1] = s2;
			Type s3("肉盾型", "罗隐");
			allSpirits[2] = s3;
			Type s4("肉盾型", "奥丁");
			allSpirits[3] = s4;
			Type s5("防御型", "上古战龙");
			allSpirits[4] = s5;
			Type s6("防御型", "超能阿布");
			allSpirits[5] = s6;
			Type s7("敏捷型", "银翼狮王");
			allSpirits[6] = s7;
			Type s8("敏捷型", "自由黄灵");
			allSpirits[7] = s8;
			int choice = recvBuf[0] - '0';			//对战精灵序号 
			
			cout << "输入你想挑战的等级：" << endl;
			//用户发送选项请求
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);
			int enemyLv;		//对战精灵等级
			if (recvBuf[1] != '\0')
				enemyLv = (recvBuf[0] - '0') * 10 + recvBuf[1] - '0';
			else 
				enemyLv = recvBuf[0] - '0';
			allSpirits[choice - 1].setLevel(enemyLv);
			accout enemy;
			enemy.my_spirit = allSpirits[choice - 1];		//保存敌方精灵 
			
			/*用户挑选出战精灵*/
			//输出用户精灵 
			system("pause");
			system("cls");
			char type[4024];
			recv(clientSocket, type, 4024, 0);	//接收第一只精灵的type 
			while (type[0] != '0') {
				//若传入有误，则请求重传 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "力量型") != 0 && strcmp(type, "肉盾型") != 0 && 
						strcmp(type, "防御型") != 0 && strcmp(type, "敏捷型") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//重新传入
						recv(clientSocket, type, 4024, 0);
					}
					//传入无误 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				char name[4024];
				recv(clientSocket, name, 4024, 0);
				//创建对象
				Type mirSpirit(type, name);
				mirSpirit.show();
				
				recv(clientSocket, type, 4024, 0);	//接受用户名 / 结束信息'0'
			}
			//请求输入 
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
SELECTWARSPIRIT1:
			cin >> recvBuf;			//用户选择的出战精灵序号 
			send(clientSocket, recvBuf, 4024, 0);
			recv(clientSocket, recvBuf, 4024, 0);	//接受判断：用户是否拥有该序号精灵
			accout mine;			//保存用户出战精灵 
			if (recvBuf[0] == '1') {		//用户拥有该精灵 
				char type[4024];
				recv(clientSocket, type, 4024, 0);
				//若传入有误，则请求重传 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "力量型") != 0 && strcmp(type, "肉盾型") != 0 && 
						strcmp(type, "防御型") != 0 && strcmp(type, "敏捷型") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//重新传入
						recv(clientSocket, type, 4024, 0);
					}
					//传入无误 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				char name[4024];
				recv(clientSocket, name, 4024, 0);	//名字 
				char level[4024];
				int mirLevel;
				recv(clientSocket, level, 4024, 0);	//等级
				if (level[1] != '\0')
					mirLevel = (level[0] - '0') * 10 + level[1] - '0';
				else 
					mirLevel = level[0] - '0';
				//创建对象
				Type mirSpirit(type, name);
				mirSpirit.setLevel(mirLevel);
				mine.my_spirit = mirSpirit;
				mirSpirit.show();
			} 
			else {
				cout << "你尚未拥有该序号上的精灵，请重新输入序号：";
				goto SELECTWARSPIRIT1;
			}
			
			//战斗阶段
			system("pause");
				system("cls");
				cout << "************************************************************" << endl;
				cout << "*                        战斗开始！                        *" << endl;
				cout << "*     我方：" << mine.my_spirit.name;
				cout << "                    敌方：" << enemy.my_spirit.name;
				cout << "     *" << endl;
				cout << "************************************************************" << endl;
				//由于精灵类的多样性，下列判断应当调用的精灵攻击函数 
				
				//声明进行战斗的我方精灵 
				Cat Vcat("力量型", "暴力战猫");
				Ox Wox("力量型", "狂野金牛");
				Luoyin LY("肉盾型", "罗隐");
				Odin OD("肉盾型", "奥丁");
				Dragon ABdragon("防御型", "上古战龙");
				Abu SPabu("防御型", "超能阿布");
				Lion SWlion("敏捷型", "银翼狮王");
				Oriole Foriole("敏捷型", "自由黄灵");
				//声明进行战斗的敌方精灵 
				Cat EVcat("力量型", "暴力战猫");
				Ox EWox("力量型", "狂野金牛");
				Luoyin ELY("肉盾型", "罗隐");
				Odin EOD("肉盾型", "奥丁");
				Dragon EABdragon("防御型", "上古战龙");
				Abu ESPabu("防御型", "超能阿布");
				Lion ESWlion("敏捷型", "银翼狮王");
				Oriole EForiole("敏捷型", "自由黄灵");
				
				int Mdamage = 0, Edamage = 0;		//分别代表我方精灵的伤害，敌方精灵的伤害 
				/*判断先手*/
				//我方先手 
				if (enemy.my_spirit.cooldown >= mine.my_spirit.cooldown) {	
					/*我方精灵的攻击*/
					if (enemy.my_spirit.name == "暴力战猫") {
						Vcat.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = Vcat.attack();
						
						//敌方精灵受击函数
						allSpirits[choice].injury(Mdamage); 
					}
					if (mine.my_spirit.name == "狂野金牛") {
						Wox.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = Wox.attack();
						
						//敌方精灵受击函数
						allSpirits[choice].injury(Mdamage); 
					}
					if (mine.my_spirit.name == "罗隐") {
						LY.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = LY.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "奥丁") {
						OD.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = OD.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "上古战龙") {
						ABdragon.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = ABdragon.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "超能阿布") {
						SPabu.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = SPabu.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "银翼狮王") {
						SWlion.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = SWlion.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "自由黄灵") {
						Foriole.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = Foriole.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					
					/*敌方精灵的攻击*/
					if (enemy.my_spirit.name == "暴力战猫") {
						EVcat.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = EVcat.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "狂野金牛") {
						EWox.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = EWox.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "罗隐") {
						ELY.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = ELY.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "奥丁") {
						EOD.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = EOD.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "上古战龙") {
						EABdragon.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = EABdragon.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "超能阿布") {
						ESPabu.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = ESPabu.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "银翼狮王") {
						ESWlion.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = ESWlion.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "自由黄灵") {
						EForiole.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = EForiole.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
				}
				//敌方先手
				if (enemy.my_spirit.cooldown < mine.my_spirit.cooldown) {	
					/*敌方精灵的攻击*/
					if (enemy.my_spirit.name == "暴力战猫") {
						EVcat.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = EVcat.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "狂野金牛") {
						EWox.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = EWox.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "罗隐") {
						ELY.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = ELY.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "奥丁") {
						EOD.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = EOD.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "上古战龙") {
						EABdragon.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = ABdragon.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "超能阿布") {
						ESPabu.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = ESPabu.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "银翼狮王") {
						ESWlion.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = ESWlion.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "自由黄灵") {
						EForiole.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = EForiole.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					
					/*我方精灵的攻击*/
					if (mine.my_spirit.name == "暴力战猫") {
						Vcat.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = Vcat.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "狂野金牛") {
						Wox.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = Wox.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "罗隐") {
						LY.setLevel(mine.my_spirit.level);		//等级升至一致
						LY.injury(Edamage);			//伤害计算 
						//我方精灵的攻击函数 
						Mdamage = LY.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "奥丁") {
						OD.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = OD.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "上古战龙") {
						ABdragon.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = ABdragon.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "超能阿布") {
						SPabu.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = SPabu.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "银翼狮王") {
						SWlion.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = SWlion.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "自由黄灵") {
						Foriole.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = Foriole.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
				}
				
				//后续战斗 
				DWORD time_start, time_end;			//获取时间 
				/* 获取开始时间 */
				time_start = GetTickCount(); 		//从操作系统启动经过的毫秒数
				double Time = 0;	//双方冷却时间 
				double pre_mineTime = 0, pre_enemyTime = 0;	//记忆上一次攻击的时间 
				while (enemy.my_spirit.HP != 0 && mine.my_spirit.HP != 0) {
					/*计时部分，用于判断攻击时间 */
					time_end = GetTickCount();				//获取结束时间
					Time = (time_end - time_start) / 1000.0;//得到经过的时间	
		//			cout << time_end - time_start << endl;
		//			cout << Time << endl;
		//			cout << pre_mineTime << "：" << Time << endl;
		//			cout << pre_enemyTime << "：" << Time << endl;
					 
					//我方冷却完毕，发动下一次攻击 
					if (mine.my_spirit.HP != 0) {
						if ((int)Time != 0 && (int)Time != (int)pre_mineTime
								&& ((int)Time % (int)mine.my_spirit.cooldown) == 0) {
							/*我方精灵的攻击*/
							//记忆攻击时间 
							pre_mineTime = Time;
							if (mine.my_spirit.name == "暴力战猫") {
								Vcat.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = Vcat.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "狂野金牛") {
								Wox.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = Wox.attack();			//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "罗隐") {
								LY.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = LY.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "奥丁") {
								OD.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = OD.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "上古战龙") {
								ABdragon.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = ABdragon.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "超能阿布") {
								SPabu.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = SPabu.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "银翼狮王") {
								SWlion.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = SWlion.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "自由黄灵") {
								Foriole.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = Foriole.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
						}
					}
					//敌方冷却完毕，发动下一次攻击 
					if (enemy.my_spirit.HP != 0) {
						if ((int)Time != 0 && (int)Time != (int)pre_enemyTime 
								&& ((int)Time % (int)enemy.my_spirit.cooldown) == 0) {
							/*敌方精灵的攻击*/
							//记忆攻击时间 
							pre_enemyTime = Time;
							if (enemy.my_spirit.name == "暴力战猫") {
								EVcat.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = EVcat.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "狂野金牛") {
								EWox.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = EWox.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "罗隐") {
								ELY.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = ELY.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "奥丁") {
								EOD.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = EOD.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "上古战龙") {
								EABdragon.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = EABdragon.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "超能阿布") {
								ESPabu.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = ESPabu.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "银翼狮王") {
								ESWlion.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = ESWlion.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "自由黄灵") {
								EForiole.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = EForiole.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
						}
					}
					
				}
				int wOl = mine.my_spirit.battleOver(enemy.my_spirit.level, enemy.my_spirit.HP);	//记录胜负 
				mine.my_spirit.levelUp(); 
				if (wOl == 1) {		//战斗胜利 
					strcpy(recvBuf, "1");
					send(clientSocket, recvBuf, 4024, 0);
					itoa(enemyLv, recvBuf, 10);
					send(clientSocket, recvBuf, 4024, 0);	//敌方等级传去服务器
				}
				else {				//战斗失败 
					strcpy(recvBuf, "0");	
					send(clientSocket, recvBuf, 4024, 0);
					itoa(enemyLv, recvBuf, 10);
					send(clientSocket, recvBuf, 4024, 0);	//敌方等级传入 
				}
				
				goto FIGHT;
		}
		
		//决斗赛 
		if (recvBuf[0] == '2') {
			//给出对战精灵列表 - 客户端输出 
			system("pause");
			system("cls");
			cout << "************************************************************" << endl;
			cout << "1.暴力战猫" << endl;
			cout << "2.狂野金牛" << endl;
			cout << "3.罗隐" << endl;
			cout << "4.奥丁" << endl;
			cout << "5.上古战龙" << endl;
			cout << "6.超能阿布" << endl;
			cout << "7.银翼狮王" << endl;
			cout << "8.自由黄灵" << endl;
			cout << "************************************************************" << endl;
			cout << "输入对应序号选择你的对手吧：" << endl; 
			//用户发送选项请求
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);
			//创建敌方精灵对象 
			Type allSpirits[8];
			Type s1("力量型", "暴力战猫");
			allSpirits[0] = s1;
			Type s2("力量型", "狂野金牛");
			allSpirits[1] = s2;
			Type s3("肉盾型", "罗隐");
			allSpirits[2] = s3;
			Type s4("肉盾型", "奥丁");
			allSpirits[3] = s4;
			Type s5("防御型", "上古战龙");
			allSpirits[4] = s5;
			Type s6("防御型", "超能阿布");
			allSpirits[5] = s6;
			Type s7("敏捷型", "银翼狮王");
			allSpirits[6] = s7;
			Type s8("敏捷型", "自由黄灵");
			allSpirits[7] = s8;
			int choice = recvBuf[0] - '0';			//对战精灵序号 
			
			cout << "输入你想挑战的等级：" << endl;
			//用户发送选项请求
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);
			int enemyLv;		//对战精灵等级
			if (recvBuf[1] != '\0')
				enemyLv = (recvBuf[0] - '0') * 10 + recvBuf[1] - '0';
			else 
				enemyLv = recvBuf[0] - '0';
			allSpirits[choice - 1].setLevel(enemyLv);
			accout enemy;
			enemy.my_spirit = allSpirits[choice - 1];		//保存敌方精灵 
			
			/*用户挑选出战精灵*/
			//输出用户精灵 
			system("pause");
			system("cls");
			char type[4024];
			recv(clientSocket, type, 4024, 0);	//接收第一只精灵的type 
			while (type[0] != '0') {
				//若传入有误，则请求重传 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "力量型") != 0 && strcmp(type, "肉盾型") != 0 && 
						strcmp(type, "防御型") != 0 && strcmp(type, "敏捷型") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//重新传入
						recv(clientSocket, type, 4024, 0);
					}
					//传入无误 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				char name[4024];
				recv(clientSocket, name, 4024, 0);
				//创建对象
				Type mirSpirit(type, name);
				mirSpirit.show();
				
				recv(clientSocket, type, 4024, 0);	//接受用户名 / 结束信息'0'
			}
			//请求输入 
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
SELECTWARSPIRIT2:
			cin >> recvBuf;			//用户选择的出战精灵序号 
			send(clientSocket, recvBuf, 4024, 0);
			recv(clientSocket, recvBuf, 4024, 0);	//接受判断：用户是否拥有该序号精灵
			accout mine;			//保存用户出战精灵 
			if (recvBuf[0] == '1') {		//用户拥有该精灵 
				char type[4024];
				recv(clientSocket, type, 4024, 0);
				//若传入有误，则请求重传 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "力量型") != 0 && strcmp(type, "肉盾型") != 0 && 
						strcmp(type, "防御型") != 0 && strcmp(type, "敏捷型") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//重新传入
						recv(clientSocket, type, 4024, 0);
					}
					//传入无误 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				char name[4024];
				recv(clientSocket, name, 4024, 0);	//名字 
				char level[4024];
				int mirLevel;
				recv(clientSocket, level, 4024, 0);	//等级
				if (level[1] != '\0')
					mirLevel = (level[0] - '0') * 10 + level[1] - '0';
				else 
					mirLevel = level[0] - '0';
				//创建对象
				Type mirSpirit(type, name);
				mirSpirit.setLevel(mirLevel);
				mine.my_spirit = mirSpirit;
				mirSpirit.show();
			} 
			else {
				cout << "你尚未拥有该序号上的精灵，请重新输入序号：";
				goto SELECTWARSPIRIT2;
			}
			
			//战斗阶段
			system("pause");
				system("cls");
				cout << "************************************************************" << endl;
				cout << "*                        战斗开始！                        *" << endl;
				cout << "*     我方：" << mine.my_spirit.name;
				cout << "                    敌方：" << enemy.my_spirit.name;
				cout << "     *" << endl;
				cout << "************************************************************" << endl;
				//由于精灵类的多样性，下列判断应当调用的精灵攻击函数 
				
				//声明进行战斗的我方精灵 
				Cat Vcat("力量型", "暴力战猫");
				Ox Wox("力量型", "狂野金牛");
				Luoyin LY("肉盾型", "罗隐");
				Odin OD("肉盾型", "奥丁");
				Dragon ABdragon("防御型", "上古战龙");
				Abu SPabu("防御型", "超能阿布");
				Lion SWlion("敏捷型", "银翼狮王");
				Oriole Foriole("敏捷型", "自由黄灵");
				//声明进行战斗的敌方精灵 
				Cat EVcat("力量型", "暴力战猫");
				Ox EWox("力量型", "狂野金牛");
				Luoyin ELY("肉盾型", "罗隐");
				Odin EOD("肉盾型", "奥丁");
				Dragon EABdragon("防御型", "上古战龙");
				Abu ESPabu("防御型", "超能阿布");
				Lion ESWlion("敏捷型", "银翼狮王");
				Oriole EForiole("敏捷型", "自由黄灵");
				
				int Mdamage = 0, Edamage = 0;		//分别代表我方精灵的伤害，敌方精灵的伤害 
				/*判断先手*/
				//我方先手 
				if (enemy.my_spirit.cooldown >= mine.my_spirit.cooldown) {	
					/*我方精灵的攻击*/
					if (enemy.my_spirit.name == "暴力战猫") {
						Vcat.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = Vcat.attack();
						
						//敌方精灵受击函数
						allSpirits[choice].injury(Mdamage); 
					}
					if (mine.my_spirit.name == "狂野金牛") {
						Wox.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = Wox.attack();
						
						//敌方精灵受击函数
						allSpirits[choice].injury(Mdamage); 
					}
					if (mine.my_spirit.name == "罗隐") {
						LY.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = LY.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "奥丁") {
						OD.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = OD.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "上古战龙") {
						ABdragon.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = ABdragon.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "超能阿布") {
						SPabu.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = SPabu.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "银翼狮王") {
						SWlion.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = SWlion.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "自由黄灵") {
						Foriole.setLevel(mine.my_spirit.level);		//等级升至一致
						Mdamage = Foriole.attack();
						
						//敌方精灵受击函数
						enemy.my_spirit.injury(Mdamage); 
					}
					
					/*敌方精灵的攻击*/
					if (enemy.my_spirit.name == "暴力战猫") {
						EVcat.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = EVcat.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "狂野金牛") {
						EWox.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = EWox.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "罗隐") {
						ELY.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = ELY.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "奥丁") {
						EOD.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = EOD.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "上古战龙") {
						EABdragon.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = EABdragon.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "超能阿布") {
						ESPabu.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = ESPabu.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "银翼狮王") {
						ESWlion.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = ESWlion.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "自由黄灵") {
						EForiole.setLevel(enemy.my_spirit.level);		//等级升至一致
						//敌方精灵的攻击函数 
						Edamage = EForiole.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
				}
				//敌方先手
				if (enemy.my_spirit.cooldown < mine.my_spirit.cooldown) {	
					/*敌方精灵的攻击*/
					if (enemy.my_spirit.name == "暴力战猫") {
						EVcat.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = EVcat.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "狂野金牛") {
						EWox.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = EWox.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "罗隐") {
						ELY.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = ELY.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "奥丁") {
						EOD.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = EOD.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "上古战龙") {
						EABdragon.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = ABdragon.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "超能阿布") {
						ESPabu.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = ESPabu.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "银翼狮王") {
						ESWlion.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = ESWlion.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "自由黄灵") {
						EForiole.setLevel(enemy.my_spirit.level);		//等级升至一致
						Edamage = EForiole.attack();
						
						//我方精灵的受击函数
						mine.my_spirit.injury(Edamage);  
					}
					
					/*我方精灵的攻击*/
					if (mine.my_spirit.name == "暴力战猫") {
						Vcat.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = Vcat.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "狂野金牛") {
						Wox.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = Wox.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "罗隐") {
						LY.setLevel(mine.my_spirit.level);		//等级升至一致
						LY.injury(Edamage);			//伤害计算 
						//我方精灵的攻击函数 
						Mdamage = LY.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "奥丁") {
						OD.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = OD.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "上古战龙") {
						ABdragon.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = ABdragon.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "超能阿布") {
						SPabu.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = SPabu.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "银翼狮王") {
						SWlion.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = SWlion.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "自由黄灵") {
						Foriole.setLevel(mine.my_spirit.level);		//等级升至一致
						//我方精灵的攻击函数 
						Mdamage = Foriole.attack();
						
						//敌方精灵的受击函数
						enemy.my_spirit.injury(Mdamage);  
					}
				}
				
				//后续战斗 
				DWORD time_start, time_end;			//获取时间 
				/* 获取开始时间 */
				time_start = GetTickCount(); 		//从操作系统启动经过的毫秒数
				double Time = 0;	//双方冷却时间 
				double pre_mineTime = 0, pre_enemyTime = 0;	//记忆上一次攻击的时间 
				while (enemy.my_spirit.HP != 0 && mine.my_spirit.HP != 0) {
					/*计时部分，用于判断攻击时间 */
					time_end = GetTickCount();				//获取结束时间
					Time = (time_end - time_start) / 1000.0;//得到经过的时间	
		//			cout << time_end - time_start << endl;
		//			cout << Time << endl;
		//			cout << pre_mineTime << "：" << Time << endl;
		//			cout << pre_enemyTime << "：" << Time << endl;
					 
					//我方冷却完毕，发动下一次攻击 
					if (mine.my_spirit.HP != 0) {
						if ((int)Time != 0 && (int)Time != (int)pre_mineTime
								&& ((int)Time % (int)mine.my_spirit.cooldown) == 0) {
							/*我方精灵的攻击*/
							//记忆攻击时间 
							pre_mineTime = Time;
							if (mine.my_spirit.name == "暴力战猫") {
								Vcat.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = Vcat.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "狂野金牛") {
								Wox.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = Wox.attack();			//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "罗隐") {
								LY.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = LY.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "奥丁") {
								OD.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = OD.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "上古战龙") {
								ABdragon.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = ABdragon.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "超能阿布") {
								SPabu.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = SPabu.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "银翼狮王") {
								SWlion.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = SWlion.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "自由黄灵") {
								Foriole.setLevel(mine.my_spirit.level);		//等级升至一致
								Mdamage = Foriole.attack();		//我方精灵攻击 
							
								//敌方精灵的受击函数
								enemy.my_spirit.injury(Mdamage);  
							}
						}
					}
					//敌方冷却完毕，发动下一次攻击 
					if (enemy.my_spirit.HP != 0) {
						if ((int)Time != 0 && (int)Time != (int)pre_enemyTime 
								&& ((int)Time % (int)enemy.my_spirit.cooldown) == 0) {
							/*敌方精灵的攻击*/
							//记忆攻击时间 
							pre_enemyTime = Time;
							if (enemy.my_spirit.name == "暴力战猫") {
								EVcat.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = EVcat.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "狂野金牛") {
								EWox.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = EWox.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "罗隐") {
								ELY.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = ELY.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "奥丁") {
								EOD.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = EOD.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "上古战龙") {
								EABdragon.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = EABdragon.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "超能阿布") {
								ESPabu.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = ESPabu.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "银翼狮王") {
								ESWlion.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = ESWlion.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "自由黄灵") {
								EForiole.setLevel(enemy.my_spirit.level);		//等级升至一致
								Edamage = EForiole.attack();		//敌方精灵攻击 
							
								//我方精灵的受击函数
								mine.my_spirit.injury(Edamage);  
							}
						}
					}
					
				}
				int wOl = mine.my_spirit.battleOver(enemy.my_spirit.level, enemy.my_spirit.HP);	//记录胜负 
				mine.my_spirit.levelUp(); 
				if (wOl == 1) {		//战斗胜利 
					strcpy(recvBuf, "1");
					send(clientSocket, recvBuf, 4024, 0);
					itoa(enemyLv, recvBuf, 10);
					send(clientSocket, recvBuf, 4024, 0);	//敌方等级传去服务器
					//获得敌方精灵，将敌方精灵信息传入服务器
					send(clientSocket, enemy.my_spirit.type.c_str(), 4024, 0);	//精灵类型 
					send(clientSocket, enemy.my_spirit.name.c_str(), 4024, 0);	//精灵名字 
					
					cout << "获得精灵：" << enemy.my_spirit.name << endl;
					enemy.my_spirit.show();
					
//					recv(clientSocket, name, 4024, 0);	//名字 
				}
				else {
					strcpy(recvBuf, "0");
					send(clientSocket, recvBuf, 4024, 0);
					itoa(enemyLv, recvBuf, 10);
					send(clientSocket, recvBuf, 4024, 0);	//敌方等级传入 
					
					//打印我方三只精灵，由用户选择失去
					cout << "\n************************************\n";
					cout << "请你从以下精灵选择一只失去：" << endl;
					recv(clientSocket, recvBuf, 4024, 0);	//循环次数
					int ret = 1;
					int loopcnt = recvBuf[0] - '0';
					for (int i = 0; i < loopcnt; i++) {
						char type[4024];
						recv(clientSocket, type, 4024, 0);
						//若传入有误，则请求重传 
						char type_flag[4];
						strcpy(type_flag, "0");
						while (type_flag[0] != '1') {
							if (strcmp(type, "力量型") != 0 && strcmp(type, "肉盾型") != 0 && 
								strcmp(type, "防御型") != 0 && strcmp(type, "敏捷型") != 0) {
								send(clientSocket, type_flag, 4, 0);
								
								//重新传入
								recv(clientSocket, type, 4024, 0);
							}
							//传入无误 
							else {
								strcpy(type_flag, "1");
								send(clientSocket, type_flag, 4, 0);
							}
						}
						char name[4024];
						recv(clientSocket, name, 4024, 0);	//名字 
						cout << ret++ << "、" << name << endl;
					} 
					cout << "\n************************************\n";
					
					cout << "请输入你要失去的精灵的序号：";
					cin >> recvBuf;
					send(clientSocket, recvBuf, 4024, 0);		//序号传入服务器 
					recv(clientSocket, recvBuf, 4024, 0);		//接收精灵名字
					cout << "\n************************************\n";
					cout << "你失去了精灵：" << recvBuf << endl;
				}
				
				//若用户没有精灵了则系统随机赠送一只精灵 
				recv(clientSocket, recvBuf, 4024, 0);	//判断用户是否拥有精灵了 
				if (recvBuf[0] == '0') {
					cout << "系统赠送一只初始精灵：" << endl;
					char type[4024];
					recv(clientSocket, type, 4024, 0);
					//若传入有误，则请求重传 
					char type_flag[4];
					strcpy(type_flag, "0");
					while (type_flag[0] != '1') {
						if (strcmp(type, "力量型") != 0 && strcmp(type, "肉盾型") != 0 && 
							strcmp(type, "防御型") != 0 && strcmp(type, "敏捷型") != 0) {
							send(clientSocket, type_flag, 4, 0);
							
							//重新传入
							recv(clientSocket, type, 4024, 0);
						}
						//传入无误 
						else {
							strcpy(type_flag, "1");
							send(clientSocket, type_flag, 4, 0);
						}
					}
					char name[4024];
					recv(clientSocket, name, 4024, 0);
					//创建对象
					Type mirSpirit(type, name);
					mirSpirit.show();
				}
				
				goto FIGHT;
		}

		//退出决斗 
		if (recvBuf[0] == '3') {
			goto USERMENU;
		}		
	}
	
		
	
	//6 关闭socket
	closesocket(clientSocket);
	
	//7 终止
	WSACleanup();
	
	cout << "客户端退出" << endl;
	cin.get();
	return 0; 
} 
