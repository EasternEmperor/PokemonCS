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
#define USERNUM    50		//�ܱ��������û��� �� �û���ӵ�е�������� 
#define NAMELENGTH 50		//�û�����������󳤶� 
#define EXMAX  4			//�������� = 4 * �ȼ� 
#define NPI    10			//������������ = 10
#define PI	   20			//���������� = 20
#define LMAX   15			//�ȼ�����15 
#define VEX	   2			//ʤ����þ��� = 2 * �з��ȼ� 
#define FEX    1			//ʧ�ܻ�þ��� = 1 * �з��ȼ� 
#define AIR    0.9			//�����;�������ʱ�����������10%
#define NAIR   0.96			//�������;�������ʱ�����������4%
#define MAI    1.0			//�����;��鹥�������Сֵ0.4s 
#define NMAI   1.5			//�������;��鹥�������Сֵ1s 
#define LVG    50			//����;�������ʱ����ֵ�ɳ� 
#define NLVG   25			//������;�������ʱ����ֵ�ɳ� 
#define DODGE  10			//���������ʣ������;��������� = DODGE + level������25% 
#define MDL    25			//��������25% 
#define CRIT   10			//�����������ʣ������;��鱩������ = CRIT + level * 2������40%
#define CHL    40			//��������40% 
#define CAT    50 * level + aggressivity * 2					//����սè�Ĺ����˺� 
#define OX     40 * level + aggressivity * 1.2 + HP * 0.08		//��Ұ��ţ�Ĺ����˺� 
#define LUOYIN 30 * level + HPMAX * 0.08 + defense * 0.8		//�����Ĺ����˺� 
#define ODIN   30 * level + HPMAX * 0.12						//�¶��Ĺ����˺� 
#define DRAGON 25 * level + defense * 0.8						//�Ϲ�ս���Ĺ����˺� 
#define ABU    25 * level + defense * 0.4 + aggressivity * 0.8	//���ܰ����Ĺ����˺� 
#define LION   40 * level + aggressivity * 0.4 + HPMAX * 0.04	//����ʨ���Ĺ����˺� 
#define ORIOLE 40 * level + aggressivity * 0.8 + defense * 0.4 	//���ɻ���Ĺ����˺� 
using namespace std;
const int PORT = 1234;
#define MaxClient 10
#define MaxBufSize 1024
#define _CRT_SECURE_NO_WARINGS

//����-����
class Spirit {
	public:
		string type;			//�������࣬��־������
		Spirit()	{};			//Ĭ�Ϲ��캯�� 
		Spirit(string type);	//���캯��
		~Spirit() 	{};			//�������� 
		//���麯��-������ʽ������ʱ���� 
		virtual int attack() = 0;	
		//���麯��-�˺���ʾ��������ʱ���� 
		virtual void injury(int damage) = 0;
		//��ӡ��������
		virtual void show() = 0; 
}; 
Spirit::Spirit(string type) {
	this -> type = type;
}

//����1����� 
class Type : public Spirit
{
	public:
		string name;			//��������
		int aggressivity;		//������
		int defense;			//������
		int HPMAX;				//��״̬����ֵ
		int HP;					//ս��ʱ����ֵ�仯 
		double cooldown;		//������� 
		int level;				//�ȼ� 
		int EX;					//����ֵ 
		int crit;				//����������
		int dodge;				//�������ܼ��� 
		
		Type()	{};				//Ĭ�Ϲ��캯�� 
		Type(string type, string name);		//���캯��	
		~Type()	{};				//�������� 
		//���麯��-������ʽ������ʱ���� 
		virtual int attack();	
		//���麯��-�˺���ʾ��������ʱ���� 
		virtual void injury(int damage);
		//��ӡ��������
		virtual void show();	
		//��������
		virtual void levelUp(); 
		//ս���������ж���Ӯ�����Ӿ���ֵ 
		virtual int battleOver(int enemyLv, int enemyHP);
		//���õȼ�
		void setLevel(int preLevel);
};
Type::Type(string type, string name) : Spirit(type) {
	this -> level = 1;			//��ʼ�ȼ�1 
	this -> EX = 0;				//��ʼ����0 
	this -> name = name;
	//�����ͻ������� 
	if ((name == "����սè") || (name == "��Ұ��ţ")) {
		this -> aggressivity = 30;	
		this -> defense = 10;
		this -> HPMAX = 65;
		this -> HP = this -> HPMAX;
		this -> cooldown = 6;	
	} 
	//����� 
	if ((name == "����") || (name == "�¶�")) {
		this -> aggressivity = 20;	
		this -> defense = 13;
		this -> HPMAX = 105;
		this -> HP = this -> HPMAX;
		this -> cooldown = 8;
	}
	//������
	if ((name == "�Ϲ�ս��") || (name == "���ܰ���")) {
		this -> aggressivity = 20;	
		this -> defense = 20;
		this -> HPMAX = 90;
		this -> HP = this -> HPMAX;
		this -> cooldown = 8;
	} 
	//������
	if ((name == "����ʨ��") || (name == "���ɻ���")) {
		this -> aggressivity = 30;	
		this -> defense = 5;
		this -> HPMAX = 55;
		this -> HP = this -> HPMAX;
		this -> cooldown = 4;
	}
}
int Type::attack() {
	cout << this -> name << "����������" << endl;
	return 0;
}
//�˺���ʾ�������ܵ��˺�ʱ���� 
void Type::injury(int damage) {
	//ʹ����������ж��Ƿ����ܣ����������dodge�������� 
	srand((int)time(0));		//�����������
	//���� 
	int pro = rand() % 100 + 1;
	if (pro <= this -> dodge + 20 && pro > 20) {
		cout << this -> name << "������������ι�����" << endl << endl;
	}
	//�����˺� 
	else {
		damage = damage * ((double)defense / (double)(50 + defense));	//�˺����� 
		HP = HP - damage;		//���˺��Ѫ���仯 
		if (HP < 0)
			HP = 0;				//Ѫ�����ܵ���0 
		//��ʾ�˺� 
		cout << this -> name << "�ܵ�" << damage << "���˺���";
		cout << this -> name << "����ֵ��Ϊ" << HP << endl;
		cout << endl;
	}
}
void Type::show() {
	cout << "**************************************************************" << endl;
	cout << "*                 " << this -> name << '\t' << "�ȼ���" << this -> level;
	cout << "                      *" << endl;
	cout << "* ��������" << aggressivity << '\t';
	cout << "��������" << defense << '\t';
	cout << "����ֵ��" << HPMAX << '\t';
	cout << "���������" << setprecision(2) << cooldown;
	cout << "  *" << endl;
	cout << "**************************************************************" << endl;
	cout << endl;
}
//���� 
void Type::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//�ȼ����� 
		EX %= (EXMAX * level);			//�������� 
		cout << "************************************************" << endl;
		cout << this -> name << "�ȼ�������" << this -> level << "!" << endl;
		//��������
		cout << "���Ա仯��" << endl;
		
		if (type == "������") {
			cout << "��������" << aggressivity;
			aggressivity += PI;			//�������仯 
			cout << " -> " << aggressivity << endl;
		}
		else {
			cout << "��������" << aggressivity;
			aggressivity += NPI;			//�������仯 
			cout << " -> " << aggressivity << endl;
		}
		
		if (type == "������") {
			cout << "��������" << defense;
			defense += PI;				//�������仯 
			cout << " -> " << defense << endl;
		}
		else {
			cout << "��������" << defense;
			defense += NPI;				//�������仯 
			cout << " -> " << defense << endl;
		}
		
		if (type == "�����") {
			cout << "����ֵ��" << HPMAX;	
			HPMAX += LVG;				//����ֵ�仯 
			HP = HPMAX;
			cout << " -> " << HPMAX << endl;
		}
		else {
			cout << "����ֵ��" << HPMAX;	
			HPMAX += LVG;				//����ֵ�仯 
			HP = HPMAX;
			cout << " -> " << HPMAX << endl;
		}
		
		if (type == "������") {
			cout << "���������" << setprecision(2) << cooldown;
			cooldown *= AIR;
			if (cooldown <= MAI)	//�������;��鹥���������ΪNMAI 
				cooldown = MAI;
			cout << " -> " << setprecision(2) << cooldown << endl;
		}
		else {
			cout << "���������" << setprecision(2) << cooldown;
			cooldown *= NAIR;
			if (cooldown <= NMAI)	//�������;��鹥���������ΪNMAI 
				cooldown = NMAI;
			cout << " -> " << setprecision(2) << cooldown << endl;
		}
		
		cout << "************************************************" << endl;
		cout << endl;
	}
}
//ս�����㺯�� 
int Type::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "************************************************" << endl;
		cout << "               ս��ʤ�����������ӣ�" << VEX * enemyLv << endl;
		cout << "************************************************" << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		cout << endl;
		
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "************************************************" << endl;
		cout << "               ս��ʧ�ܣ��������ӣ�" << FEX * enemyLv << endl;
		cout << "************************************************" << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		cout << endl;
		
		return 0;
	}
}
//���õȼ� 
void Type::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//����ֱ�ӳɳ� 	
	if (type == "������") {
		aggressivity += PI * gradeDif;			//�������仯 
	}
	else {
		aggressivity += NPI * gradeDif;			//�������仯 
	}
	
	if (type == "������") {
		defense += PI * gradeDif;				//�������仯 
	}
	else {
		defense += NPI * gradeDif;				//�������仯 
	}
	
	if (type == "�����") {
		HPMAX += LVG * gradeDif;				//����ֵ�仯 
		HP = HPMAX;
	}
	else {	
		HPMAX += LVG * gradeDif;				//����ֵ�仯 
		HP = HPMAX;
	}
	
	if (type == "������") {
		//�������� 
		if (gradeDif >= 0) {
			cooldown *= pow(AIR, gradeDif);
			if (cooldown <= MAI)	//�������;��鹥���������ΪMAI 
				cooldown = MAI;
		}
		//���ǽ��� 
		else {
			cooldown /= pow(AIR, gradeDif);
			if (cooldown <= MAI)	//�������;��鹥���������ΪMAI 
				cooldown = MAI;
		}
	}
	else {
		//�������� 
		if (gradeDif >= 0) {
			cooldown *= pow(NAIR, gradeDif);
			if (cooldown <= NMAI)	//�������;��鹥���������ΪMAI 
				cooldown = NMAI;
		}
		//���ǽ��� 
		else {
			cooldown /= pow(NAIR, gradeDif);
			if (cooldown <= NMAI)	//�������;��鹥���������ΪMAI 
				cooldown = NMAI;
		}
	}
}


//����2�����徫��
//1.1-�����ͣ�����սè 
class Cat : public Type
{
	public:
		Cat(string type, string name);		//���캯��
		~Cat()	{};			//�������� 
		//��������
		virtual void levelUp(); 
		//ս���������ж���Ӯ�����Ӿ���ֵ 
		virtual int battleOver(int enemyLv, int enemyHP);	
		//���麯��-������ʽ������ʱ���� 
		virtual int attack();	
		//���麯��-�˺���ʾ��������ʱ���� 
		virtual void injury(int damage);
		//��ӡ��������
		virtual void show(); 
		//���ö�Ӧ�ȼ� 
		void setLevel(int prelevel);
};
Cat::Cat(string type, string name) : Type(type, name){
	this -> level = 1;		//��ʼ�ȼ�Ϊ1
	this -> EX = 0;			//��ʼ����ֵΪ0
}
//���� 
void Cat::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//�ȼ����� 
		EX %= (EXMAX * level);			//�������� 
		cout << "����սè�ȼ�������" << level << "!" << endl;
		//��������
		cout << "���Ա仯��" << endl;
		
		cout << "��������" << aggressivity;
		aggressivity += PI;			//�������仯 
		cout << " -> " << aggressivity << endl;
		
		cout << "��������" << defense;
		defense += NPI;				//�������仯 
		cout << " -> " << defense << endl;
		
		cout << "����ֵ��" << HPMAX;	
		HPMAX += NLVG;				//����ֵ�仯 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "���������" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//�������;��鹥���������ΪNMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//ս�����㺯�� 
int Cat::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		return 0;
	}
}
//������ʽ����
int Cat::attack() {
	//��ʾ���� 
	cout << "����սè�Ĺ�����" << endl;
	cout << "����սèʹ����<����ͻ��>" << endl;
	//�����˺� 
	int damage = 0;
	damage = CAT;		//�����˺� 
	
	//ʹ����������ж��Ƿ񱩻������������crit���򱩻� 
	srand((int)time(0));		//�����������
	//���� 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "�˴ι��������������˺��ӱ���" << endl << endl;
		damage *= 2;
	}
	
	return damage;
}
//�˺���ʾ�������ܵ��˺�ʱ���� 
void Cat::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//�˺����� 
	HP = HP - damage;		//���˺��Ѫ���仯 
	if (HP < 0)
		HP = 0;				//Ѫ�����ܵ���0 
	//��ʾ�˺� 
	cout << "����սè�ܵ�" << damage << "���˺���";
	cout << "����սè����ֵ��Ϊ" << HP << endl;
	cout << endl;
}
//��ӡ���� 
void Cat::show() {
	cout << "����սè" << '\t' << "�ȼ���" << level << endl;
	cout << "��������" << aggressivity << '\t';
	cout << "��������" << defense << '\t';
	cout << "����ֵ��" << HPMAX << '\t';
	cout << "���������" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//���õȼ� 
void Cat::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//����ֱ�ӳɳ� 
	aggressivity += PI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//��������������������� 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//���ǽ����������������
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//ע�⹥���������
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//1.2-�����ͣ���Ұ��ţ 
class Ox : public Type
{
	private:
		int level;			//�ȼ� 
		int EX;				//����ֵ 
	public:
		Ox(string type, string name);		//���캯��
		~Ox()	{};			//�������� 
		//��������
		virtual void levelUp(); 
		//ս���������ж���Ӯ�����Ӿ���ֵ 
		virtual int battleOver(int enemyLv, int enemyHP);	 
		//���麯��-������ʽ������ʱ���� 
		virtual int attack();	
		//���麯��-�˺���ʾ��������ʱ���� 
		virtual void injury(int damage);
		//��ӡ����
		virtual void show(); 
		//���ö�Ӧ�ȼ� 
		void setLevel(int prelevel);
};
Ox::Ox(string type, string name) : Type(type, name){
	this -> level = 1;		//��ʼ�ȼ�Ϊ1
	this -> EX = 0;			//��ʼ����ֵΪ0
}
//���� 
void Ox::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//�ȼ����� 
		EX %= (EXMAX * level);			//�������� 
		cout << "��Ұ��ţ�ȼ�������" << level << "!" << endl;
		//��������
		cout << "���Ա仯��" << endl;
		
		cout << "��������" << aggressivity;
		aggressivity += PI;			//�������仯 
		cout << " -> " << aggressivity << endl;
		
		cout << "��������" << defense;
		defense += NPI;				//�������仯 
		cout << " -> " << defense << endl;
		
		cout << "����ֵ��" << HPMAX;	
		HPMAX += NLVG;				//����ֵ�仯 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "���������" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//�������;��鹥���������ΪNMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//ս�����㺯�� 
int Ox::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		return 0;
	}
}
//������ʽ����
int Ox::attack() {
	//��ʾ���� 
	cout << "��Ұ��ţ�Ĺ�����" << endl;
	cout << "��Ұ��ţʹ����<���;���>" << endl;
	//�����˺� 
	int damage = 0;
	damage = OX;		//�����˺� 
	
	//ʹ����������ж��Ƿ񱩻������������crit���򱩻� 
	srand((int)time(0));		//�����������
	//���� 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "�˴ι��������������˺��ӱ���" << endl << endl;
		damage *= 2;
	}
	
	return damage;
}
//�˺���ʾ�������ܵ��˺�ʱ���� 
void Ox::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//�˺����� 
	HP = HP - damage;		//���˺��Ѫ���仯 
	if (HP < 0)
		HP = 0;				//Ѫ�����ܵ���0 
	//��ʾ�˺� 
	cout << "��Ұ��ţ�ܵ�" << damage << "���˺�";
	cout << "��Ұ��ţ����ֵ��Ϊ" << HP << endl;
	cout << endl;
}
//��ӡ���� 
void Ox::show() {
	cout << "��Ұ��ţ" << '\t' << "�ȼ���" << level << endl;
	cout << "��������" << aggressivity << '\t';
	cout << "��������" << defense << '\t';
	cout << "����ֵ��" << HPMAX << '\t';
	cout << "���������" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//���õȼ� 
void Ox::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//����ֱ�ӳɳ� 
	aggressivity += PI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//��������������������� 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//���ǽ����������������
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//ע�⹥���������
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//2.1-����ͣ�����
class Luoyin : public Type
{
	private:
		int level;			//�ȼ� 
		int EX;				//����ֵ 
	public:
		Luoyin(string type, string name);		//���캯��
		~Luoyin()	{};			//�������� 
		//��������
		virtual void levelUp(); 
		//ս���������ж���Ӯ�����Ӿ���ֵ 
		virtual int battleOver(int enemyLv, int enemyHP);	 
		//���麯��-������ʽ������ʱ���� 
		virtual int attack();	
		//���麯��-�˺���ʾ��������ʱ���� 
		virtual void injury(int damage);
		//��ӡ���� 
		virtual void show();
		//���ö�Ӧ�ȼ�
		void setLevel(int preLevel); 
};
Luoyin::Luoyin(string type, string name) : Type(type, name){
	this -> level = 1;		//��ʼ�ȼ�Ϊ1
	this -> EX = 0;			//��ʼ����ֵΪ0
}
//���� 
void Luoyin::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//�ȼ����� 
		EX %= (EXMAX * level);			//�������� 
		cout << "�����ȼ�������" << level << "!" << endl;
		//��������
		cout << "���Ա仯��" << endl;
		
		cout << "��������" << aggressivity;
		aggressivity += NPI;			//�������仯 
		cout << " -> " << aggressivity << endl;
		
		cout << "��������" << defense;
		defense += NPI;				//�������仯 
		cout << " -> " << defense << endl;
		
		cout << "����ֵ��" << HPMAX;	
		HPMAX += LVG;				//����ֵ�仯 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "���������" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//�������;��鹥���������ΪNMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//ս�����㺯�� 
int Luoyin::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		return 0;
	}
}
//������ʽ����
int Luoyin::attack() {
	//��ʾ���� 
	cout << "�����Ĺ�����" << endl;
	cout << "����ʹ����<����>" << endl;
	//�����˺� 
	int damage = 0;
	damage = LUOYIN;		//�����˺� 
	
	//ʹ����������ж��Ƿ񱩻������������0-crit���򱩻� 
	srand((int)time(0));		//�����������
	//���� 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "�˴ι��������������˺��ӱ���" << endl << endl;
		damage *= 2;
	}
	
	return damage;
}
//�˺���ʾ�������ܵ��˺�ʱ���� 
void Luoyin::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//�˺����� 
	HP = HP - damage;		//���˺��Ѫ���仯 
	if (HP < 0)
		HP = 0;				//Ѫ�����ܵ���0 
	//��ʾ�˺� 
	cout << "�����ܵ�" << damage << "���˺���";
	cout << "��������ֵ��Ϊ" << HP << endl;
	cout << endl;
}
//��ӡ���� 
void Luoyin::show() {
	cout << "����" << '\t' << "�ȼ���" << level << endl;
	cout << "��������" << aggressivity << '\t';
	cout << "��������" << defense << '\t';
	cout << "����ֵ��" << HPMAX << '\t';
	cout << "���������" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//���õȼ� 
void Luoyin::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//����ֱ�ӳɳ� 
	aggressivity += NPI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += LVG * gradeDif;
	HP = HPMAX;
	//��������������������� 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//���ǽ����������������
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//ע�⹥���������
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//2.2-����ͣ��¶�
class Odin : public Type
{
	private:
		int level;			//�ȼ� 
		int EX;				//����ֵ 
	public:
		Odin(string type, string name);		//���캯��
		~Odin()	{};			//�������� 
		//��������
		virtual void levelUp(); 
		//ս���������ж���Ӯ�����Ӿ���ֵ 
		virtual int battleOver(int enemyLv, int enemyHP);	 
		//���麯��-������ʽ������ʱ���� 
		virtual int attack();	
		//���麯��-�˺���ʾ��������ʱ���� 
		virtual void injury(int damage);
		//��ӡ����
		virtual void show(); 
		//���ö�Ӧ�ȼ�
		void setLevel(int preLevel); 
};
Odin::Odin(string type, string name) : Type(type, name){
	this -> level = 1;		//��ʼ�ȼ�Ϊ1
	this -> EX = 0;			//��ʼ����ֵΪ0
}
//���� 
void Odin::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//�ȼ����� 
		EX %= (EXMAX * level);			//�������� 
		cout << "�¶��ȼ�������" << level << "!" << endl;
		//��������
		cout << "���Ա仯��" << endl;
		
		cout << "��������" << aggressivity;
		aggressivity += NPI;			//�������仯 
		cout << " -> " << aggressivity << endl;
		
		cout << "��������" << defense;
		defense += NPI;				//�������仯 
		cout << " -> " << defense << endl;
		
		cout << "����ֵ��" << HPMAX;	
		HPMAX += LVG;				//����ֵ�仯 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "���������" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//�������;��鹥���������ΪNMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//ս�����㺯�� 
int Odin::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		return 0;
	}
}
//������ʽ����
int Odin::attack() {
	//��ʾ���� 
	cout << "�¶��Ĺ�����" << endl;
	cout << "�¶�ʹ����<�����>" << endl;
	//�����˺� 
	int damage = 0;
	damage = ODIN;		//�����˺� 
	
	//ʹ����������ж��Ƿ񱩻������������0-crit���򱩻� 
	srand((int)time(0));		//�����������
	//���� 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "�˴ι��������������˺��ӱ���" << endl << endl;
		damage *= 2;
	}
	
	return damage;
}
//�˺���ʾ�������ܵ��˺�ʱ���� 
void Odin::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//�˺����� 
	HP = HP - damage;		//���˺��Ѫ���仯 
	if (HP < 0)
		HP = 0;				//Ѫ�����ܵ���0 
	//��ʾ�˺� 
	cout << "�¶��ܵ�" << damage << "���˺���";
	cout << "�¶�����ֵ��Ϊ" << HP << endl;
	cout << endl;
}
//��ӡ���� 
void Odin::show() {
	cout << "�¶�" << '\t' << "�ȼ���" << level << endl;
	cout << "��������" << aggressivity << '\t';
	cout << "��������" << defense << '\t';
	cout << "����ֵ��" << HPMAX << '\t';
	cout << "���������" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//���õȼ� 
void Odin::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//����ֱ�ӳɳ� 
	aggressivity += NPI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += LVG * gradeDif;
	HP = HPMAX;
	//��������������������� 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//���ǽ����������������
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//ע�⹥���������
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//3.1-�����ͣ��Ϲ�ս��
class Dragon : public Type
{
	private:
		int level;			//�ȼ� 
		int EX;				//����ֵ 
	public:
		Dragon(string type, string name);		//���캯��
		~Dragon()	{};			//�������� 
		//��������
		virtual void levelUp(); 
		//ս���������ж���Ӯ�����Ӿ���ֵ 
		virtual int battleOver(int enemyLv, int enemyHP);	 
		//���麯��-������ʽ������ʱ���� 
		virtual int attack();	
		//���麯��-�˺���ʾ��������ʱ���� 
		virtual void injury(int damage);
		//��ӡ����
		virtual void show(); 
		//���ö�Ӧ�ȼ�
		void setLevel(int preLevel); 
};
Dragon::Dragon(string type, string name) : Type(type, name){
	this -> level = 1;		//��ʼ�ȼ�Ϊ1
	this -> EX = 0;			//��ʼ����ֵΪ0
}
//���� 
void Dragon::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//�ȼ����� 
		EX %= (EXMAX * level);			//�������� 
		cout << "�Ϲ�ս���ȼ�������" << level << "!" << endl;
		//��������
		cout << "���Ա仯��" << endl;
		
		cout << "��������" << aggressivity;
		aggressivity += NPI;			//�������仯 
		cout << " -> " << aggressivity << endl;
		
		cout << "��������" << defense;
		defense += PI;				//�������仯 
		cout << " -> " << defense << endl;
		
		cout << "����ֵ��" << HPMAX;	
		HPMAX += LVG;				//����ֵ�仯 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "���������" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//�������;��鹥���������ΪNMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//ս�����㺯�� 
int Dragon::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		return 0;
	}
}
//������ʽ����
int Dragon::attack() {
	//��ʾ���� 
	cout << "�Ϲ�ս���Ĺ�����" << endl;
	cout << "�Ϲ�ս��ʹ����<��˵����>" << endl;
	//�����˺� 
	int damage = 0;
	damage = DRAGON;		//�����˺� 
	
	//ʹ����������ж��Ƿ񱩻������������0-crit���򱩻� 
	srand((int)time(0));		//�����������
	//���� 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "�˴ι��������������˺��ӱ���" << endl << endl;
		damage *= 2;
	}
	
	return damage;
}
//�˺���ʾ�������ܵ��˺�ʱ���� 
void Dragon::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//�˺����� 
	HP = HP - damage;		//���˺��Ѫ���仯 
	if (HP < 0)
		HP = 0;				//Ѫ�����ܵ���0 
	//��ʾ�˺� 
	cout << "�Ϲ�ս���ܵ�" << damage << "���˺���";
	cout << "�Ϲ�ս������ֵ��Ϊ" << HP << endl;
	cout << endl;
}
//��ӡ���� 
void Dragon::show() {
	cout << "�Ϲ�ս��" << '\t' << "�ȼ���" << level << endl;
	cout << "��������" << aggressivity << '\t';
	cout << "��������" << defense << '\t';
	cout << "����ֵ��" << HPMAX << '\t';
	cout << "���������" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//���õȼ� 
void Dragon::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//����ֱ�ӳɳ� 
	aggressivity += NPI * gradeDif;
	defense += PI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//��������������������� 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//���ǽ����������������
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//ע�⹥���������
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//3.2-�����ͣ����ܰ���
class Abu : public Type
{
	private:
		int level;			//�ȼ� 
		int EX;				//����ֵ 
	public:
		Abu(string type, string name);		//���캯��
		~Abu()	{};			//�������� 
		//��������
		virtual void levelUp(); 
		//ս���������ж���Ӯ�����Ӿ���ֵ 
		virtual int battleOver(int enemyLv, int enemyHP);	 
		//���麯��-������ʽ������ʱ���� 
		virtual int attack();	
		//���麯��-�˺���ʾ��������ʱ���� 
		virtual void injury(int damage);
		//��ӡ����
		virtual void show(); 
		//���ö�Ӧ�ȼ�
		void setLevel(int preLevel); 
};
Abu::Abu(string type, string name) : Type(type, name){
	this -> level = 1;		//��ʼ�ȼ�Ϊ1
	this -> EX = 0;			//��ʼ����ֵΪ0
}
//���� 
void Abu::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//�ȼ����� 
		EX %= (EXMAX * level);			//�������� 
		cout << "���ܰ����ȼ�������" << level << "!" << endl;
		//��������
		cout << "���Ա仯��" << endl;
		
		cout << "��������" << aggressivity;
		aggressivity += NPI;			//�������仯 
		cout << " -> " << aggressivity << endl;
		
		cout << "��������" << defense;
		defense += PI;				//�������仯 
		cout << " -> " << defense << endl;
		
		cout << "����ֵ��" << HPMAX;	
		HPMAX += NLVG;				//����ֵ�仯 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "���������" << setprecision(2) << cooldown;
		cooldown *= NAIR;
		if (cooldown <= NMAI)	//�������;��鹥���������ΪNMAI 
			cooldown = NMAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//ս�����㺯�� 
int Abu::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		return 0;
	}
}
//������ʽ����
int Abu::attack() {
	//��ʾ���� 
	cout << "���ܰ����Ĺ�����" << endl;
	cout << "���ܰ���ʹ����<��֮צ��>" << endl;
	//�����˺� 
	int damage = 0;
	damage = ABU;		//�����˺� 
	
	//ʹ����������ж��Ƿ񱩻������������0-crit���򱩻� 
	srand((int)time(0));		//�����������
	//���� 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "�˴ι��������������˺��ӱ���" << endl << endl;
		damage *= 2;
	}
	
	return damage;
}
//�˺���ʾ�������ܵ��˺�ʱ���� 
void Abu::injury(int damage) {
//	cout << ((double)defense / (double)(100 + defense)) << endl;
	damage = damage * ((double)defense / (double)(50 + defense));	//�˺����� 
	HP = HP - damage;		//���˺��Ѫ���仯 
	if (HP < 0)
		HP = 0;				//Ѫ�����ܵ���0 
	//��ʾ�˺� 
	cout << "���ܰ����ܵ�" << damage << "���˺���";
	cout << "���ܰ�������ֵ��Ϊ" << HP << endl;
	cout << endl;
}
//��ӡ���� 
void Abu::show() {
	cout << "���ܰ���" << '\t' << "�ȼ���" << level << endl;
	cout << "��������" << aggressivity << '\t';
	cout << "��������" << defense << '\t';
	cout << "����ֵ��" << HPMAX << '\t';
	cout << "���������" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//���õȼ� 
void Abu::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//����ֱ�ӳɳ� 
	aggressivity += NPI * gradeDif;
	defense += PI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//��������������������� 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//���ǽ����������������
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//ע�⹥���������
	if (cooldown <= NMAI)
		cooldown = NMAI;
}

//4.1-�����ͣ�����ʨ��
class Lion : public Type
{
	private:
		int level;			//�ȼ� 
		int EX;				//����ֵ 
	public:
		Lion(string type, string name);		//���캯��
		~Lion()	{};			//�������� 
		//��������
		virtual void levelUp(); 
		//ս���������ж���Ӯ�����Ӿ���ֵ 
		virtual int battleOver(int enemyLv, int enemyHP);	 
		//���麯��-������ʽ������ʱ���� 
		virtual int attack();	
		//���麯��-�˺���ʾ��������ʱ���� 
		virtual void injury(int damage);
		//��ӡ����
		virtual void show(); 
		//���ö�Ӧ�ȼ�
		void setLevel(int preLevel); 
};
Lion::Lion(string type, string name) : Type(type, name){
	this -> level = 1;		//��ʼ�ȼ�Ϊ1
	this -> EX = 0;			//��ʼ����ֵΪ0
}
//���� 
void Lion::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//�ȼ����� 
		EX %= (EXMAX * level);			//�������� 
		cout << "����ʨ���ȼ�������" << level << "!" << endl;
		//��������
		cout << "���Ա仯��" << endl;
		
		cout << "��������" << aggressivity;
		aggressivity += NPI;			//�������仯 
		cout << " -> " << aggressivity << endl;
		
		cout << "��������" << defense;
		defense += NPI;				//�������仯 
		cout << " -> " << defense << endl;
		
		cout << "����ֵ��" << HPMAX;	
		HPMAX += NLVG;				//����ֵ�仯 
		HP = HPMAX;
		cout << " -> " << HPMAX;
		
		cout << "���������" << setprecision(2) << cooldown;
		cooldown *= AIR;
		if (cooldown <= MAI)	//�������;��鹥���������ΪMAI 
			cooldown = MAI;
		cout << " -> " << setprecision(2) << cooldown;
		
		cout << endl;
	}
}
//ս�����㺯�� 
int Lion::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		return 0;
	}
}
//������ʽ����
int Lion::attack() {
	//��ʾ���� 
	cout << "����ʨ���Ĺ�����" << endl;
	cout << "����ʨ��ʹ����<ɳ�����>" << endl;
	//�����˺� 
	int damage = 0;
	damage = LION;		//�����˺� 
	
	//ʹ����������ж��Ƿ񱩻������������0-crit���򱩻� 
	srand((int)time(0));		//�����������
	//���� 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "�˴ι��������������˺��ӱ���" << endl << endl;
		damage *= 2;
	}
	
	return damage;
}
//�˺���ʾ�������ܵ��˺�ʱ���� 
void Lion::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//�˺����� 
	HP = HP - damage;		//���˺��Ѫ���仯 
	if (HP < 0)
		HP = 0;				//Ѫ�����ܵ���0 
	//��ʾ�˺� 
	cout << "����ʨ���ܵ�" << damage << "���˺���";
	cout << "����ʨ������ֵ��Ϊ" << HP << endl;
	cout << endl;
}
//��ӡ���� 
void Lion::show() {
	cout << "����ʨ��" << '\t' << "�ȼ���" << level << endl;
	cout << "��������" << aggressivity << '\t';
	cout << "��������" << defense << '\t';
	cout << "����ֵ��" << HPMAX << '\t';
	cout << "���������" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//���õȼ� 
void Lion::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//����ֱ�ӳɳ� 
	aggressivity += NPI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//��������������������� 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//���ǽ����������������
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//ע�⹥���������
	if (cooldown <= MAI)
		cooldown = MAI;
}

//4.2-�����ͣ����ɻ���
class Oriole : public Type
{
	private:
		int level;			//�ȼ� 
		int EX;				//����ֵ 
	public:
		Oriole(string type, string name);		//���캯��
		~Oriole()	{};			//�������� 
		//��������
		virtual void levelUp(); 
		//ս���������ж���Ӯ�����Ӿ���ֵ 
		virtual int battleOver(int enemyLv, int enemyHP);	 
		//���麯��-������ʽ������ʱ���� 
		virtual int attack();	
		//���麯��-�˺���ʾ��������ʱ���� 
		virtual void injury(int damage);
		//��ӡ�������� 
		virtual void show();
		//���ö�Ӧ�ȼ�
		void setLevel(int preLevel); 
};
Oriole::Oriole(string type, string name) : Type(type, name){
	this -> level = 1;		//��ʼ�ȼ�Ϊ1
	this -> EX = 0;			//��ʼ����ֵΪ0
}
//���� 
void Oriole::levelUp() {
	if (EX >= EXMAX * level) {
		level += EX / (EXMAX * level);	//�ȼ����� 
		EX %= (EXMAX * level);			//�������� 
		cout << "���ɻ���ȼ�������" << level << "!" << endl;
		//��������
		cout << "���Ա仯��" << endl;
		
		cout << "��������" << aggressivity;
		aggressivity += NPI;			//�������仯 
		cout << " -> " << aggressivity << endl;
		
		cout << "��������" << defense;
		defense += NPI;				//�������仯 
		cout << " -> " << defense << endl;
		
		cout << "����ֵ��" << HPMAX;	
		HPMAX += NLVG;				//����ֵ�仯 
		HP = HPMAX;
		cout << " -> " << HPMAX << endl;
		
		cout << "���������" << setprecision(2) << cooldown;
		cooldown *= AIR;
		if (cooldown <= MAI)	//�����;��鹥���������ΪMAI 
			cooldown = MAI;
		cout << " -> " << setprecision(2) << cooldown;
	}
}
//ս�����㺯�� 
int Oriole::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		return 1;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		return 0;
	}
}
//������ʽ����
int Oriole::attack() {
	//��ʾ���� 
	cout << "���ɻ���Ĺ�����" << endl;
	cout << "���ɻ���ʹ����<ħ������>" << endl;
	//�����˺� 
	int damage = 0;
	damage = ORIOLE;		//�����˺� 
	
	//ʹ����������ж��Ƿ񱩻������������0-crit���򱩻� 
	srand((int)time(0));		//�����������
	//���� 
	int pro = rand() % 100 + 1;
	if (pro <= this -> crit + 50 && pro > 50) {
		cout << this -> name << "�˴ι��������������˺��ӱ���" << endl << endl;
		damage *= 2;
	}
	
	return damage;
}
//�˺���ʾ�������ܵ��˺�ʱ���� 
void Oriole::injury(int damage) {
	damage = damage * ((double)defense / (double)(50 + defense));	//�˺����� 
	HP = HP - damage;		//���˺��Ѫ���仯 
	if (HP < 0)
		HP = 0;				//Ѫ�����ܵ���0 
	//��ʾ�˺� 
	cout << "���ɻ����ܵ�" << damage << "���˺���";
	cout << "���ɻ�������ֵ��Ϊ" << HP << endl;
	cout << endl;
}
//��ӡ���� 
void Oriole::show() {
	cout << "���ɻ���" << '\t' << "�ȼ���" << level << endl;
	cout << "��������" << aggressivity << '\t';
	cout << "��������" << defense << '\t';
	cout << "����ֵ��" << HPMAX << '\t';
	cout << "���������" << setprecision(2) << cooldown << endl;
	cout << endl;
}
//���õȼ� 
void Oriole::setLevel(int preLevel) {
	int gradeDif = preLevel - level;
	level = preLevel;
	//����ֱ�ӳɳ� 
	aggressivity += NPI * gradeDif;
	defense += NPI * gradeDif;
	HPMAX += NLVG * gradeDif;
	HP = HPMAX;
	//��������������������� 
	if (gradeDif >= 0)
		cooldown = cooldown * pow(NAIR, gradeDif); 
	//���ǽ����������������
	if (gradeDif < 0)
		cooldown = cooldown / pow(NAIR, (-gradeDif));
	//ע�⹥���������
	if (cooldown <= MAI)
		cooldown = MAI;
}

//�û��˺� 
class accout {
	public:
		char username[NAMELENGTH];	//�û���
		char password[NAMELENGTH];	//���� 
		int logCnt;					//��¼��¼���� 
		int onlineOrNot;			//1���� 0������ 
		int spiritcnt;				//��¼������� 
		int lV15;					//15��������� 
		
		Type my_spirit;				//�û��ľ��� 
};

int main()
{
	cout << "------------�ͻ���------------" << endl;
	
	//1 ��ʼ��
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	
	//2 �����׽���
	SOCKET clientSocket = {};
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == clientSocket) {
		cout << "�׽��ִ���ʧ�ܣ�" << endl;
	} 
	else {
		cout << "�׽��ִ����ɹ���" << endl;
	}
	
	//3 ���׽��֣�ָ���󶨵�IP��ַ�Ͷ˿ں�
	sockaddr_in socketAddr;
	socketAddr.sin_family = PF_INET;
	socketAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	socketAddr.sin_port = htons(1234);
	int cRes = connect(clientSocket, (SOCKADDR *)&socketAddr, sizeof(SOCKADDR));
	if (SOCKET_ERROR == cRes) {
		cout << "�ͻ��ˣ�\t\t�����������ʧ�ܣ�" << endl;
	} 
	else {
		cout << "�ͻ��ˣ�\t\t����������ӳɹ���" << endl;
	}
	
	while (true) {
		//4 ��������
//		char sendBuf[1024] = "from Client:     Hello service!";
//		send(clientSocket, sendBuf, strlen(sendBuf), 0);
	
//		string s;
//		cout << "���뷢�����ݣ�";
//		getline(cin, s);		//getline��������ո�Ĭ���Ի��з�����
//		send(clientSocket, (char *)s.c_str(), (int)s.length(), 0); 
		 
		//��Ϊrecv���պ�������������������Ҫ�����ж�
		//������ȷ�ҲŽ������ݣ�����Ӱ���Ҽ�������
//		if (strcmp("��ȡ�汾��Ϣ", s.c_str()) == 0) {
//			char recvBuf[4024] = {};
//			int reLen = recv(clientSocket, recvBuf, 4024, 0);	//recv�������ȴ��������� 
//			cout << endl << recvBuf << endl << endl;
//		} 
		//5 ����/���� ����
	//	char recvBuf[1024] = {};
	//	recv(clientSocket, recvBuf, 1024, 0);
	//	cout << "�ͻ��˽������ݣ�" << recvBuf << endl << endl;
	
		//���յ��˵� 
LOGMENU:
		char recvBuf[4024] = {};
		recv(clientSocket, recvBuf, 4024, 0);
		system("pause");
		system("cls");			//�����޹���� 	
		cout << recvBuf << endl << endl;
		//�û�����ѡ������
		cin >> recvBuf;
		send(clientSocket, recvBuf, 4024, 0);
		
		//1��ע��
		if (recvBuf[0] == '1') {
			//���յ������ע����ʾ
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
			//��ȡ�û�������û���
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);
			
			recv(clientSocket, recvBuf, 4024, 0);
			//����ע�� 
			if (recvBuf[0] == '0') {
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				
				cin >> recvBuf;		//�û�����
				send(clientSocket, recvBuf, 4024, 0);	//�������� 
				//ע��ɹ�
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl; 
				
				goto LOGMENU;
			}
			//�ѱ�ע��
			else {
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl << endl;
				goto LOGMENU;		//�ص���¼�˵� 
			} 
		}
		//2����¼
		if (recvBuf[0] == '2') {
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
			//��ȡ�û�������û��� 
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);	//���͸������� 
			
			recv(clientSocket, recvBuf, 4024, 0);
			//�ɵ�¼����е�¼
			if (recvBuf[0] == '1') {
				recv(clientSocket, recvBuf, 4024, 0);	//��ʾ��¼
				cout << recvBuf; 
				cin >> recvBuf;			//��ȡ�û����������
				send(clientSocket, recvBuf, 4024, 0);	//��������
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl;
				//������� 
				if (strcmp(recvBuf, "��¼�ɹ���\n") != 0) {
					goto LOGMENU;
				} 
			}
			//δע��
			else {
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl;
				
				goto LOGMENU;
			} 
		} 
		//3��ע��
		if (recvBuf[0] == '3') {
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
			//��ȡ�û�������û��� 
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);	//���͸������� 
			
			recv(clientSocket, recvBuf, 4024, 0);
			//��ע�������ע�� 
			if (recvBuf[0] == '1') {
				recv(clientSocket, recvBuf, 4024, 0);	//��ʾ�����¼����
				cout << recvBuf; 
				cin >> recvBuf;			//��ȡ�û����������
				send(clientSocket, recvBuf, 4024, 0);	//��������
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl;
				//����ע���ɹ���񣬶����ص�¼�˵� 
				goto LOGMENU; 
			} 
			//δע��
			else {
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl;
				
				goto LOGMENU;
			} 
		}  
		//4���˳�
		if (recvBuf[0] == '4') {
			break;
		} 
		
		/*�û���¼��*/
		//�û���һ�ε�½
		recv(clientSocket, recvBuf, 4024, 0);
		if (recvBuf[0] == '1') {
			cout << "��ӵ��������ֻ��ʼ���飺" << endl;
			for (int i = 0; i < 3; i++) {
				char type[4024];
				recv(clientSocket, type, 4024, 0);
				//�����������������ش� 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "������") != 0 && strcmp(type, "�����") != 0 && 
						strcmp(type, "������") != 0 && strcmp(type, "������") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//���´���
						recv(clientSocket, type, 4024, 0);
					}
					//�������� 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				char name[4024];
				recv(clientSocket, name, 4024, 0);
				//��������
				Type mirSpirit(type, name);
				mirSpirit.show();
			}
		} 
		
		
USERMENU:
		recv(clientSocket, recvBuf, 4024, 0);
		system("pause");
		system("cls");			//�����޹����
		cout << recvBuf << endl << endl;
		//�û�����ѡ������
		cin >> recvBuf;
		send(clientSocket, recvBuf, 4024, 0);
		
		//1���鿴�ҵľ���
		if (recvBuf[0] == '1') {
			//�����û�������� 
			recv(clientSocket, recvBuf, 4024, 0);
			int spiritcnt = atoi(recvBuf);
			for (int i = 0; i < spiritcnt; i++) {
				char type[4024];
				recv(clientSocket, type, 4024, 0);
				//�����������������ش� 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "������") != 0 && strcmp(type, "�����") != 0 && 
						strcmp(type, "������") != 0 && strcmp(type, "������") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//���´���
						recv(clientSocket, type, 4024, 0);
					}
					//�������� 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				//���� 
				char name[4024];
				recv(clientSocket, name, 4024, 0);
				//�ȼ� 
				recv(clientSocket, recvBuf, 4024, 0);
				int level = atoi(recvBuf);
				//��������
				Type mirSpirit(type, name);
				mirSpirit.setLevel(level);
				//��ӡ 
				mirSpirit.show();
			}
			
			goto USERMENU;
		} 
		
		//2���鿴�û�״̬
		if (recvBuf[0] == '2') {
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
			recv(clientSocket, recvBuf, 4024, 0);	//�����û��� 
			//����û� 
			while (strcmp(recvBuf, "") != 0) {
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);	//��������
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);	//����״̬
				cout << recvBuf << endl; 
				recv(clientSocket, recvBuf, 4024, 0);	//�����û��� / ��������"" 
			}
			
			cout << endl;
			goto USERMENU; 
		}
		
		//3�������û�
		if (recvBuf[0] == '3') {
			recv(clientSocket, recvBuf, 4024, 0);		//��ʾ����
			cout << recvBuf;
//			cout << "������Ҫ�������û�����"; 
			cin >> recvBuf;			//��ȡ�û�������û���
			send(clientSocket, recvBuf, 4024, 0);
			//�ж��Ƿ��ҵ�
			recv(clientSocket, recvBuf, 4024, 0);
			if (recvBuf[0] == '1') {	//���ҵ� 
				/*����û�����״̬*/
				recv(clientSocket, recvBuf, 4024, 0);	//�����û��� 
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);	//��������
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);	//����״̬
				cout << recvBuf << endl; 
				
				/*�û�����*/
				//��������
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl; 
				//����15������ 
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				recv(clientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl; 
				
				/*������û�ӵ�еľ���*/
				recv(clientSocket, recvBuf, 4024, 0);	//�жϸ��û��Ƿ��о��� 
				//�����û�û�о���
				if (recvBuf[0] == '0') {
					cout << "���û���ʱδӵ�о��顣" << endl;
				}
				//�����û�ӵ�о���
				else {
					//�����û�������� 
					recv(clientSocket, recvBuf, 4024, 0);
					int spiritcnt = atoi(recvBuf);
					for (int i = 0; i < spiritcnt; i++) {
						char type[4024];
						recv(clientSocket, type, 4024, 0);
						//�����������������ش� 
						char type_flag[4];
						strcpy(type_flag, "0");
						while (type_flag[0] != '1') {
							if (strcmp(type, "������") != 0 && strcmp(type, "�����") != 0 && 
								strcmp(type, "������") != 0 && strcmp(type, "������") != 0) {
								send(clientSocket, type_flag, 4, 0);
								
								//���´���
								recv(clientSocket, type, 4024, 0);
							}
							//�������� 
							else {
								strcpy(type_flag, "1");
								send(clientSocket, type_flag, 4, 0);
							}
						}
						char name[4024];
						recv(clientSocket, name, 4024, 0);
						//��������
						Type mirSpirit(type, name);
						mirSpirit.show();
					}
				} 
			} 
			else {
				cout << "δ���ҵ����û���" << endl;
			}	
			
			cout << endl;
			goto USERMENU; 
		} 
		
		//5���˳���¼
		if (recvBuf[0] == '5') {
			cout << "�˳���¼��" << endl;
			goto LOGMENU;
		} 
		
		//4��������� 
FIGHT:
		recv(clientSocket, recvBuf, 4024, 0);
		system("pause");
		system("cls");			//�����޹����
		cout << recvBuf << endl << endl;
		//�û�����ѡ������
		cin >> recvBuf;
		send(clientSocket, recvBuf, 4024, 0);
		
		//������ 
		if (recvBuf[0] == '1') {
			//������ս�����б� - �ͻ������ 
			system("pause");
			system("cls");
			cout << "************************************************************" << endl;
			cout << "1.����սè" << endl;
			cout << "2.��Ұ��ţ" << endl;
			cout << "3.����" << endl;
			cout << "4.�¶�" << endl;
			cout << "5.�Ϲ�ս��" << endl;
			cout << "6.���ܰ���" << endl;
			cout << "7.����ʨ��" << endl;
			cout << "8.���ɻ���" << endl;
			cout << "************************************************************" << endl;
			cout << "�����Ӧ���ѡ����Ķ��ְɣ�" << endl; 
			//�û�����ѡ������
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);
			//�����з�������� 
			Type allSpirits[8];
			Type s1("������", "����սè");
			allSpirits[0] = s1;
			Type s2("������", "��Ұ��ţ");
			allSpirits[1] = s2;
			Type s3("�����", "����");
			allSpirits[2] = s3;
			Type s4("�����", "�¶�");
			allSpirits[3] = s4;
			Type s5("������", "�Ϲ�ս��");
			allSpirits[4] = s5;
			Type s6("������", "���ܰ���");
			allSpirits[5] = s6;
			Type s7("������", "����ʨ��");
			allSpirits[6] = s7;
			Type s8("������", "���ɻ���");
			allSpirits[7] = s8;
			int choice = recvBuf[0] - '0';			//��ս������� 
			
			cout << "����������ս�ĵȼ���" << endl;
			//�û�����ѡ������
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);
			int enemyLv;		//��ս����ȼ�
			if (recvBuf[1] != '\0')
				enemyLv = (recvBuf[0] - '0') * 10 + recvBuf[1] - '0';
			else 
				enemyLv = recvBuf[0] - '0';
			allSpirits[choice - 1].setLevel(enemyLv);
			accout enemy;
			enemy.my_spirit = allSpirits[choice - 1];		//����з����� 
			
			/*�û���ѡ��ս����*/
			//����û����� 
			system("pause");
			system("cls");
			char type[4024];
			recv(clientSocket, type, 4024, 0);	//���յ�һֻ�����type 
			while (type[0] != '0') {
				//�����������������ش� 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "������") != 0 && strcmp(type, "�����") != 0 && 
						strcmp(type, "������") != 0 && strcmp(type, "������") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//���´���
						recv(clientSocket, type, 4024, 0);
					}
					//�������� 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				char name[4024];
				recv(clientSocket, name, 4024, 0);
				//��������
				Type mirSpirit(type, name);
				mirSpirit.show();
				
				recv(clientSocket, type, 4024, 0);	//�����û��� / ������Ϣ'0'
			}
			//�������� 
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
SELECTWARSPIRIT1:
			cin >> recvBuf;			//�û�ѡ��ĳ�ս������� 
			send(clientSocket, recvBuf, 4024, 0);
			recv(clientSocket, recvBuf, 4024, 0);	//�����жϣ��û��Ƿ�ӵ�и���ž���
			accout mine;			//�����û���ս���� 
			if (recvBuf[0] == '1') {		//�û�ӵ�иþ��� 
				char type[4024];
				recv(clientSocket, type, 4024, 0);
				//�����������������ش� 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "������") != 0 && strcmp(type, "�����") != 0 && 
						strcmp(type, "������") != 0 && strcmp(type, "������") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//���´���
						recv(clientSocket, type, 4024, 0);
					}
					//�������� 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				char name[4024];
				recv(clientSocket, name, 4024, 0);	//���� 
				char level[4024];
				int mirLevel;
				recv(clientSocket, level, 4024, 0);	//�ȼ�
				if (level[1] != '\0')
					mirLevel = (level[0] - '0') * 10 + level[1] - '0';
				else 
					mirLevel = level[0] - '0';
				//��������
				Type mirSpirit(type, name);
				mirSpirit.setLevel(mirLevel);
				mine.my_spirit = mirSpirit;
				mirSpirit.show();
			} 
			else {
				cout << "����δӵ�и�����ϵľ��飬������������ţ�";
				goto SELECTWARSPIRIT1;
			}
			
			//ս���׶�
			system("pause");
				system("cls");
				cout << "************************************************************" << endl;
				cout << "*                        ս����ʼ��                        *" << endl;
				cout << "*     �ҷ���" << mine.my_spirit.name;
				cout << "                    �з���" << enemy.my_spirit.name;
				cout << "     *" << endl;
				cout << "************************************************************" << endl;
				//���ھ�����Ķ����ԣ������ж�Ӧ�����õľ��鹥������ 
				
				//��������ս�����ҷ����� 
				Cat Vcat("������", "����սè");
				Ox Wox("������", "��Ұ��ţ");
				Luoyin LY("�����", "����");
				Odin OD("�����", "�¶�");
				Dragon ABdragon("������", "�Ϲ�ս��");
				Abu SPabu("������", "���ܰ���");
				Lion SWlion("������", "����ʨ��");
				Oriole Foriole("������", "���ɻ���");
				//��������ս���ĵз����� 
				Cat EVcat("������", "����սè");
				Ox EWox("������", "��Ұ��ţ");
				Luoyin ELY("�����", "����");
				Odin EOD("�����", "�¶�");
				Dragon EABdragon("������", "�Ϲ�ս��");
				Abu ESPabu("������", "���ܰ���");
				Lion ESWlion("������", "����ʨ��");
				Oriole EForiole("������", "���ɻ���");
				
				int Mdamage = 0, Edamage = 0;		//�ֱ�����ҷ�������˺����з�������˺� 
				/*�ж�����*/
				//�ҷ����� 
				if (enemy.my_spirit.cooldown >= mine.my_spirit.cooldown) {	
					/*�ҷ�����Ĺ���*/
					if (enemy.my_spirit.name == "����սè") {
						Vcat.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = Vcat.attack();
						
						//�з������ܻ�����
						allSpirits[choice].injury(Mdamage); 
					}
					if (mine.my_spirit.name == "��Ұ��ţ") {
						Wox.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = Wox.attack();
						
						//�з������ܻ�����
						allSpirits[choice].injury(Mdamage); 
					}
					if (mine.my_spirit.name == "����") {
						LY.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = LY.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "�¶�") {
						OD.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = OD.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "�Ϲ�ս��") {
						ABdragon.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = ABdragon.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "���ܰ���") {
						SPabu.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = SPabu.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "����ʨ��") {
						SWlion.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = SWlion.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "���ɻ���") {
						Foriole.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = Foriole.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					
					/*�з�����Ĺ���*/
					if (enemy.my_spirit.name == "����սè") {
						EVcat.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = EVcat.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "��Ұ��ţ") {
						EWox.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = EWox.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "����") {
						ELY.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = ELY.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "�¶�") {
						EOD.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = EOD.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "�Ϲ�ս��") {
						EABdragon.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = EABdragon.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "���ܰ���") {
						ESPabu.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = ESPabu.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "����ʨ��") {
						ESWlion.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = ESWlion.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "���ɻ���") {
						EForiole.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = EForiole.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
				}
				//�з�����
				if (enemy.my_spirit.cooldown < mine.my_spirit.cooldown) {	
					/*�з�����Ĺ���*/
					if (enemy.my_spirit.name == "����սè") {
						EVcat.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = EVcat.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "��Ұ��ţ") {
						EWox.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = EWox.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "����") {
						ELY.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = ELY.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "�¶�") {
						EOD.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = EOD.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "�Ϲ�ս��") {
						EABdragon.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = ABdragon.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "���ܰ���") {
						ESPabu.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = ESPabu.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "����ʨ��") {
						ESWlion.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = ESWlion.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "���ɻ���") {
						EForiole.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = EForiole.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					
					/*�ҷ�����Ĺ���*/
					if (mine.my_spirit.name == "����սè") {
						Vcat.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = Vcat.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "��Ұ��ţ") {
						Wox.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = Wox.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "����") {
						LY.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						LY.injury(Edamage);			//�˺����� 
						//�ҷ�����Ĺ������� 
						Mdamage = LY.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "�¶�") {
						OD.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = OD.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "�Ϲ�ս��") {
						ABdragon.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = ABdragon.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "���ܰ���") {
						SPabu.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = SPabu.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "����ʨ��") {
						SWlion.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = SWlion.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "���ɻ���") {
						Foriole.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = Foriole.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
				}
				
				//����ս�� 
				DWORD time_start, time_end;			//��ȡʱ�� 
				/* ��ȡ��ʼʱ�� */
				time_start = GetTickCount(); 		//�Ӳ���ϵͳ���������ĺ�����
				double Time = 0;	//˫����ȴʱ�� 
				double pre_mineTime = 0, pre_enemyTime = 0;	//������һ�ι�����ʱ�� 
				while (enemy.my_spirit.HP != 0 && mine.my_spirit.HP != 0) {
					/*��ʱ���֣������жϹ���ʱ�� */
					time_end = GetTickCount();				//��ȡ����ʱ��
					Time = (time_end - time_start) / 1000.0;//�õ�������ʱ��	
		//			cout << time_end - time_start << endl;
		//			cout << Time << endl;
		//			cout << pre_mineTime << "��" << Time << endl;
		//			cout << pre_enemyTime << "��" << Time << endl;
					 
					//�ҷ���ȴ��ϣ�������һ�ι��� 
					if (mine.my_spirit.HP != 0) {
						if ((int)Time != 0 && (int)Time != (int)pre_mineTime
								&& ((int)Time % (int)mine.my_spirit.cooldown) == 0) {
							/*�ҷ�����Ĺ���*/
							//���乥��ʱ�� 
							pre_mineTime = Time;
							if (mine.my_spirit.name == "����սè") {
								Vcat.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = Vcat.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "��Ұ��ţ") {
								Wox.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = Wox.attack();			//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "����") {
								LY.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = LY.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "�¶�") {
								OD.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = OD.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "�Ϲ�ս��") {
								ABdragon.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = ABdragon.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "���ܰ���") {
								SPabu.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = SPabu.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "����ʨ��") {
								SWlion.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = SWlion.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "���ɻ���") {
								Foriole.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = Foriole.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
						}
					}
					//�з���ȴ��ϣ�������һ�ι��� 
					if (enemy.my_spirit.HP != 0) {
						if ((int)Time != 0 && (int)Time != (int)pre_enemyTime 
								&& ((int)Time % (int)enemy.my_spirit.cooldown) == 0) {
							/*�з�����Ĺ���*/
							//���乥��ʱ�� 
							pre_enemyTime = Time;
							if (enemy.my_spirit.name == "����սè") {
								EVcat.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = EVcat.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "��Ұ��ţ") {
								EWox.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = EWox.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "����") {
								ELY.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = ELY.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "�¶�") {
								EOD.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = EOD.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "�Ϲ�ս��") {
								EABdragon.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = EABdragon.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "���ܰ���") {
								ESPabu.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = ESPabu.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "����ʨ��") {
								ESWlion.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = ESWlion.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "���ɻ���") {
								EForiole.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = EForiole.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
						}
					}
					
				}
				int wOl = mine.my_spirit.battleOver(enemy.my_spirit.level, enemy.my_spirit.HP);	//��¼ʤ�� 
				mine.my_spirit.levelUp(); 
				if (wOl == 1) {		//ս��ʤ�� 
					strcpy(recvBuf, "1");
					send(clientSocket, recvBuf, 4024, 0);
					itoa(enemyLv, recvBuf, 10);
					send(clientSocket, recvBuf, 4024, 0);	//�з��ȼ���ȥ������
				}
				else {				//ս��ʧ�� 
					strcpy(recvBuf, "0");	
					send(clientSocket, recvBuf, 4024, 0);
					itoa(enemyLv, recvBuf, 10);
					send(clientSocket, recvBuf, 4024, 0);	//�з��ȼ����� 
				}
				
				goto FIGHT;
		}
		
		//������ 
		if (recvBuf[0] == '2') {
			//������ս�����б� - �ͻ������ 
			system("pause");
			system("cls");
			cout << "************************************************************" << endl;
			cout << "1.����սè" << endl;
			cout << "2.��Ұ��ţ" << endl;
			cout << "3.����" << endl;
			cout << "4.�¶�" << endl;
			cout << "5.�Ϲ�ս��" << endl;
			cout << "6.���ܰ���" << endl;
			cout << "7.����ʨ��" << endl;
			cout << "8.���ɻ���" << endl;
			cout << "************************************************************" << endl;
			cout << "�����Ӧ���ѡ����Ķ��ְɣ�" << endl; 
			//�û�����ѡ������
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);
			//�����з�������� 
			Type allSpirits[8];
			Type s1("������", "����սè");
			allSpirits[0] = s1;
			Type s2("������", "��Ұ��ţ");
			allSpirits[1] = s2;
			Type s3("�����", "����");
			allSpirits[2] = s3;
			Type s4("�����", "�¶�");
			allSpirits[3] = s4;
			Type s5("������", "�Ϲ�ս��");
			allSpirits[4] = s5;
			Type s6("������", "���ܰ���");
			allSpirits[5] = s6;
			Type s7("������", "����ʨ��");
			allSpirits[6] = s7;
			Type s8("������", "���ɻ���");
			allSpirits[7] = s8;
			int choice = recvBuf[0] - '0';			//��ս������� 
			
			cout << "����������ս�ĵȼ���" << endl;
			//�û�����ѡ������
			cin >> recvBuf;
			send(clientSocket, recvBuf, 4024, 0);
			int enemyLv;		//��ս����ȼ�
			if (recvBuf[1] != '\0')
				enemyLv = (recvBuf[0] - '0') * 10 + recvBuf[1] - '0';
			else 
				enemyLv = recvBuf[0] - '0';
			allSpirits[choice - 1].setLevel(enemyLv);
			accout enemy;
			enemy.my_spirit = allSpirits[choice - 1];		//����з����� 
			
			/*�û���ѡ��ս����*/
			//����û����� 
			system("pause");
			system("cls");
			char type[4024];
			recv(clientSocket, type, 4024, 0);	//���յ�һֻ�����type 
			while (type[0] != '0') {
				//�����������������ش� 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "������") != 0 && strcmp(type, "�����") != 0 && 
						strcmp(type, "������") != 0 && strcmp(type, "������") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//���´���
						recv(clientSocket, type, 4024, 0);
					}
					//�������� 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				char name[4024];
				recv(clientSocket, name, 4024, 0);
				//��������
				Type mirSpirit(type, name);
				mirSpirit.show();
				
				recv(clientSocket, type, 4024, 0);	//�����û��� / ������Ϣ'0'
			}
			//�������� 
			recv(clientSocket, recvBuf, 4024, 0);
			cout << recvBuf;
SELECTWARSPIRIT2:
			cin >> recvBuf;			//�û�ѡ��ĳ�ս������� 
			send(clientSocket, recvBuf, 4024, 0);
			recv(clientSocket, recvBuf, 4024, 0);	//�����жϣ��û��Ƿ�ӵ�и���ž���
			accout mine;			//�����û���ս���� 
			if (recvBuf[0] == '1') {		//�û�ӵ�иþ��� 
				char type[4024];
				recv(clientSocket, type, 4024, 0);
				//�����������������ش� 
				char type_flag[4];
				strcpy(type_flag, "0");
				while (type_flag[0] != '1') {
					if (strcmp(type, "������") != 0 && strcmp(type, "�����") != 0 && 
						strcmp(type, "������") != 0 && strcmp(type, "������") != 0) {
						send(clientSocket, type_flag, 4, 0);
						
						//���´���
						recv(clientSocket, type, 4024, 0);
					}
					//�������� 
					else {
						strcpy(type_flag, "1");
						send(clientSocket, type_flag, 4, 0);
					}
				}
				char name[4024];
				recv(clientSocket, name, 4024, 0);	//���� 
				char level[4024];
				int mirLevel;
				recv(clientSocket, level, 4024, 0);	//�ȼ�
				if (level[1] != '\0')
					mirLevel = (level[0] - '0') * 10 + level[1] - '0';
				else 
					mirLevel = level[0] - '0';
				//��������
				Type mirSpirit(type, name);
				mirSpirit.setLevel(mirLevel);
				mine.my_spirit = mirSpirit;
				mirSpirit.show();
			} 
			else {
				cout << "����δӵ�и�����ϵľ��飬������������ţ�";
				goto SELECTWARSPIRIT2;
			}
			
			//ս���׶�
			system("pause");
				system("cls");
				cout << "************************************************************" << endl;
				cout << "*                        ս����ʼ��                        *" << endl;
				cout << "*     �ҷ���" << mine.my_spirit.name;
				cout << "                    �з���" << enemy.my_spirit.name;
				cout << "     *" << endl;
				cout << "************************************************************" << endl;
				//���ھ�����Ķ����ԣ������ж�Ӧ�����õľ��鹥������ 
				
				//��������ս�����ҷ����� 
				Cat Vcat("������", "����սè");
				Ox Wox("������", "��Ұ��ţ");
				Luoyin LY("�����", "����");
				Odin OD("�����", "�¶�");
				Dragon ABdragon("������", "�Ϲ�ս��");
				Abu SPabu("������", "���ܰ���");
				Lion SWlion("������", "����ʨ��");
				Oriole Foriole("������", "���ɻ���");
				//��������ս���ĵз����� 
				Cat EVcat("������", "����սè");
				Ox EWox("������", "��Ұ��ţ");
				Luoyin ELY("�����", "����");
				Odin EOD("�����", "�¶�");
				Dragon EABdragon("������", "�Ϲ�ս��");
				Abu ESPabu("������", "���ܰ���");
				Lion ESWlion("������", "����ʨ��");
				Oriole EForiole("������", "���ɻ���");
				
				int Mdamage = 0, Edamage = 0;		//�ֱ�����ҷ�������˺����з�������˺� 
				/*�ж�����*/
				//�ҷ����� 
				if (enemy.my_spirit.cooldown >= mine.my_spirit.cooldown) {	
					/*�ҷ�����Ĺ���*/
					if (enemy.my_spirit.name == "����սè") {
						Vcat.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = Vcat.attack();
						
						//�з������ܻ�����
						allSpirits[choice].injury(Mdamage); 
					}
					if (mine.my_spirit.name == "��Ұ��ţ") {
						Wox.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = Wox.attack();
						
						//�з������ܻ�����
						allSpirits[choice].injury(Mdamage); 
					}
					if (mine.my_spirit.name == "����") {
						LY.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = LY.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "�¶�") {
						OD.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = OD.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "�Ϲ�ս��") {
						ABdragon.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = ABdragon.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "���ܰ���") {
						SPabu.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = SPabu.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "����ʨ��") {
						SWlion.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = SWlion.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					if (mine.my_spirit.name == "���ɻ���") {
						Foriole.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						Mdamage = Foriole.attack();
						
						//�з������ܻ�����
						enemy.my_spirit.injury(Mdamage); 
					}
					
					/*�з�����Ĺ���*/
					if (enemy.my_spirit.name == "����սè") {
						EVcat.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = EVcat.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "��Ұ��ţ") {
						EWox.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = EWox.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "����") {
						ELY.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = ELY.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "�¶�") {
						EOD.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = EOD.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "�Ϲ�ս��") {
						EABdragon.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = EABdragon.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "���ܰ���") {
						ESPabu.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = ESPabu.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "����ʨ��") {
						ESWlion.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = ESWlion.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "���ɻ���") {
						EForiole.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						//�з�����Ĺ������� 
						Edamage = EForiole.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
				}
				//�з�����
				if (enemy.my_spirit.cooldown < mine.my_spirit.cooldown) {	
					/*�з�����Ĺ���*/
					if (enemy.my_spirit.name == "����սè") {
						EVcat.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = EVcat.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "��Ұ��ţ") {
						EWox.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = EWox.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "����") {
						ELY.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = ELY.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "�¶�") {
						EOD.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = EOD.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "�Ϲ�ս��") {
						EABdragon.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = ABdragon.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "���ܰ���") {
						ESPabu.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = ESPabu.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "����ʨ��") {
						ESWlion.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = ESWlion.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					if (enemy.my_spirit.name == "���ɻ���") {
						EForiole.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
						Edamage = EForiole.attack();
						
						//�ҷ�������ܻ�����
						mine.my_spirit.injury(Edamage);  
					}
					
					/*�ҷ�����Ĺ���*/
					if (mine.my_spirit.name == "����սè") {
						Vcat.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = Vcat.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "��Ұ��ţ") {
						Wox.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = Wox.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "����") {
						LY.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						LY.injury(Edamage);			//�˺����� 
						//�ҷ�����Ĺ������� 
						Mdamage = LY.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "�¶�") {
						OD.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = OD.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "�Ϲ�ս��") {
						ABdragon.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = ABdragon.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "���ܰ���") {
						SPabu.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = SPabu.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "����ʨ��") {
						SWlion.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = SWlion.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
					if (mine.my_spirit.name == "���ɻ���") {
						Foriole.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
						//�ҷ�����Ĺ������� 
						Mdamage = Foriole.attack();
						
						//�з�������ܻ�����
						enemy.my_spirit.injury(Mdamage);  
					}
				}
				
				//����ս�� 
				DWORD time_start, time_end;			//��ȡʱ�� 
				/* ��ȡ��ʼʱ�� */
				time_start = GetTickCount(); 		//�Ӳ���ϵͳ���������ĺ�����
				double Time = 0;	//˫����ȴʱ�� 
				double pre_mineTime = 0, pre_enemyTime = 0;	//������һ�ι�����ʱ�� 
				while (enemy.my_spirit.HP != 0 && mine.my_spirit.HP != 0) {
					/*��ʱ���֣������жϹ���ʱ�� */
					time_end = GetTickCount();				//��ȡ����ʱ��
					Time = (time_end - time_start) / 1000.0;//�õ�������ʱ��	
		//			cout << time_end - time_start << endl;
		//			cout << Time << endl;
		//			cout << pre_mineTime << "��" << Time << endl;
		//			cout << pre_enemyTime << "��" << Time << endl;
					 
					//�ҷ���ȴ��ϣ�������һ�ι��� 
					if (mine.my_spirit.HP != 0) {
						if ((int)Time != 0 && (int)Time != (int)pre_mineTime
								&& ((int)Time % (int)mine.my_spirit.cooldown) == 0) {
							/*�ҷ�����Ĺ���*/
							//���乥��ʱ�� 
							pre_mineTime = Time;
							if (mine.my_spirit.name == "����սè") {
								Vcat.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = Vcat.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "��Ұ��ţ") {
								Wox.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = Wox.attack();			//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "����") {
								LY.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = LY.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "�¶�") {
								OD.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = OD.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "�Ϲ�ս��") {
								ABdragon.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = ABdragon.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "���ܰ���") {
								SPabu.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = SPabu.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "����ʨ��") {
								SWlion.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = SWlion.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
							if (mine.my_spirit.name == "���ɻ���") {
								Foriole.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
								Mdamage = Foriole.attack();		//�ҷ����鹥�� 
							
								//�з�������ܻ�����
								enemy.my_spirit.injury(Mdamage);  
							}
						}
					}
					//�з���ȴ��ϣ�������һ�ι��� 
					if (enemy.my_spirit.HP != 0) {
						if ((int)Time != 0 && (int)Time != (int)pre_enemyTime 
								&& ((int)Time % (int)enemy.my_spirit.cooldown) == 0) {
							/*�з�����Ĺ���*/
							//���乥��ʱ�� 
							pre_enemyTime = Time;
							if (enemy.my_spirit.name == "����սè") {
								EVcat.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = EVcat.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "��Ұ��ţ") {
								EWox.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = EWox.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "����") {
								ELY.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = ELY.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "�¶�") {
								EOD.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = EOD.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "�Ϲ�ս��") {
								EABdragon.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = EABdragon.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "���ܰ���") {
								ESPabu.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = ESPabu.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "����ʨ��") {
								ESWlion.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = ESWlion.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
							if (enemy.my_spirit.name == "���ɻ���") {
								EForiole.setLevel(enemy.my_spirit.level);		//�ȼ�����һ��
								Edamage = EForiole.attack();		//�з����鹥�� 
							
								//�ҷ�������ܻ�����
								mine.my_spirit.injury(Edamage);  
							}
						}
					}
					
				}
				int wOl = mine.my_spirit.battleOver(enemy.my_spirit.level, enemy.my_spirit.HP);	//��¼ʤ�� 
				mine.my_spirit.levelUp(); 
				if (wOl == 1) {		//ս��ʤ�� 
					strcpy(recvBuf, "1");
					send(clientSocket, recvBuf, 4024, 0);
					itoa(enemyLv, recvBuf, 10);
					send(clientSocket, recvBuf, 4024, 0);	//�з��ȼ���ȥ������
					//��õз����飬���з�������Ϣ���������
					send(clientSocket, enemy.my_spirit.type.c_str(), 4024, 0);	//�������� 
					send(clientSocket, enemy.my_spirit.name.c_str(), 4024, 0);	//�������� 
					
					cout << "��þ��飺" << enemy.my_spirit.name << endl;
					enemy.my_spirit.show();
					
//					recv(clientSocket, name, 4024, 0);	//���� 
				}
				else {
					strcpy(recvBuf, "0");
					send(clientSocket, recvBuf, 4024, 0);
					itoa(enemyLv, recvBuf, 10);
					send(clientSocket, recvBuf, 4024, 0);	//�з��ȼ����� 
					
					//��ӡ�ҷ���ֻ���飬���û�ѡ��ʧȥ
					cout << "\n************************************\n";
					cout << "��������¾���ѡ��һֻʧȥ��" << endl;
					recv(clientSocket, recvBuf, 4024, 0);	//ѭ������
					int ret = 1;
					int loopcnt = recvBuf[0] - '0';
					for (int i = 0; i < loopcnt; i++) {
						char type[4024];
						recv(clientSocket, type, 4024, 0);
						//�����������������ش� 
						char type_flag[4];
						strcpy(type_flag, "0");
						while (type_flag[0] != '1') {
							if (strcmp(type, "������") != 0 && strcmp(type, "�����") != 0 && 
								strcmp(type, "������") != 0 && strcmp(type, "������") != 0) {
								send(clientSocket, type_flag, 4, 0);
								
								//���´���
								recv(clientSocket, type, 4024, 0);
							}
							//�������� 
							else {
								strcpy(type_flag, "1");
								send(clientSocket, type_flag, 4, 0);
							}
						}
						char name[4024];
						recv(clientSocket, name, 4024, 0);	//���� 
						cout << ret++ << "��" << name << endl;
					} 
					cout << "\n************************************\n";
					
					cout << "��������Ҫʧȥ�ľ������ţ�";
					cin >> recvBuf;
					send(clientSocket, recvBuf, 4024, 0);		//��Ŵ�������� 
					recv(clientSocket, recvBuf, 4024, 0);		//���վ�������
					cout << "\n************************************\n";
					cout << "��ʧȥ�˾��飺" << recvBuf << endl;
				}
				
				//���û�û�о�������ϵͳ�������һֻ���� 
				recv(clientSocket, recvBuf, 4024, 0);	//�ж��û��Ƿ�ӵ�о����� 
				if (recvBuf[0] == '0') {
					cout << "ϵͳ����һֻ��ʼ���飺" << endl;
					char type[4024];
					recv(clientSocket, type, 4024, 0);
					//�����������������ش� 
					char type_flag[4];
					strcpy(type_flag, "0");
					while (type_flag[0] != '1') {
						if (strcmp(type, "������") != 0 && strcmp(type, "�����") != 0 && 
							strcmp(type, "������") != 0 && strcmp(type, "������") != 0) {
							send(clientSocket, type_flag, 4, 0);
							
							//���´���
							recv(clientSocket, type, 4024, 0);
						}
						//�������� 
						else {
							strcpy(type_flag, "1");
							send(clientSocket, type_flag, 4, 0);
						}
					}
					char name[4024];
					recv(clientSocket, name, 4024, 0);
					//��������
					Type mirSpirit(type, name);
					mirSpirit.show();
				}
				
				goto FIGHT;
		}

		//�˳����� 
		if (recvBuf[0] == '3') {
			goto USERMENU;
		}		
	}
	
		
	
	//6 �ر�socket
	closesocket(clientSocket);
	
	//7 ��ֹ
	WSACleanup();
	
	cout << "�ͻ����˳�" << endl;
	cin.get();
	return 0; 
} 
