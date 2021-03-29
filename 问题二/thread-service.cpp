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
		virtual void battleOver(int enemyLv, int enemyHP);
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
	damage = damage * ((double)defense / (double)(50 + defense));	//�˺����� 
	HP = HP - damage;		//���˺��Ѫ���仯 
	if (HP < 0)
		HP = 0;				//Ѫ�����ܵ���0 
	//��ʾ�˺� 
	cout << this -> name << "�ܵ�" << damage << "���˺���";
	cout << this -> name << "����ֵ��Ϊ" << HP << endl;
	cout << endl;
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
void Type::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "************************************************" << endl;
		cout << "               ս��ʤ�����������ӣ�" << VEX * enemyLv << endl;
		cout << "************************************************" << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "************************************************" << endl;
		cout << "               ս��ʧ�ܣ��������ӣ�" << FEX * enemyLv << endl;
		cout << "************************************************" << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		cout << endl;
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
		virtual void battleOver(int enemyLv, int enemyHP);	
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
void Cat::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		cout << endl;
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
		virtual void battleOver(int enemyLv, int enemyHP);	 
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
void Ox::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		cout << endl;
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
		virtual void battleOver(int enemyLv, int enemyHP);	 
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
void Luoyin::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		cout << endl;
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
		virtual void battleOver(int enemyLv, int enemyHP);	 
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
void Odin::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		cout << endl;
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
		virtual void battleOver(int enemyLv, int enemyHP);	 
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
void Dragon::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		cout << endl;
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
		virtual void battleOver(int enemyLv, int enemyHP);	 
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
void Abu::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		cout << endl;
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
		virtual void battleOver(int enemyLv, int enemyHP);	 
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
void Lion::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
		cout << endl;
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
		cout << endl;
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
		virtual void battleOver(int enemyLv, int enemyHP);	 
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
void Oriole::battleOver(int enemyLv, int enemyHP) {
	if (HP != 0 && enemyHP == 0) {		//ʤ�� 
		cout << "ս��ʤ������������" << VEX * enemyLv << endl;
		EX += VEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ� 
	}
	else if (HP == 0 && enemyHP != 0) {	//ʧ�� 
		cout << "ս��ʧ�ܣ���������" << FEX * enemyLv << endl;
		EX += FEX * enemyLv;
		HP = HPMAX;				//ս�������������ֵ�ظ�
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
typedef struct accout {
	char username[NAMELENGTH];	//�û���
	char password[NAMELENGTH];	//���� 
	int logCnt;					//��¼��¼���� 
	int onlineOrNot;			//1���� 0������ 
	
	Type my_spirit[USERNUM];	//�û��ľ��� 
}accout;
accout users[USERNUM];	//�û��ṹ����
int usercnt = 0;		//��¼�û����� 

//�����߳�
DWORD WINAPI ServerThread(LPVOID lpParameter){
	SOCKET *ClientSocket=(SOCKET*)lpParameter;
	int receByt=0;
	char RecvBuf[MaxBufSize];
	char SendBuf[MaxBufSize];
	//��¼�û�ע����� 
	int logSrNo = 0;
	while(1){
		/*��¼����˵�*/ 
LOGMENU:
		char recvBuf[4024] = {};
		char logMenu[4024] = {"\n************************************\n\
*             1��ע��              *\n\
*             2����¼              *\n\
*             3��ע��              *\n\
*             4���˳�              *\n\
************************************\n\
          ���������ѡ��\n"};
		int sLen = send(*ClientSocket, logMenu, strlen(logMenu), 0);
		cout << logMenu << endl; 
		//�û�����ѡ��
		int reLen = recv(*ClientSocket, recvBuf, 4024, 0);	//�����������ȴ���������
		
		if (SOCKET_ERROR == sLen) {
			cout << "��������������ʧ�ܣ�" << endl;
		} 
		else {
			cout << recvBuf << endl;
			//1��ע�� 
			if (recvBuf[0] == '1') {
				strcpy(recvBuf, "***************ע��*****************\n������Ҫע����û�����");
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				
				recv(*ClientSocket, recvBuf, 4024, 0);		//�յ��ͻ��˷������û���
				cout << recvBuf << endl;		//��������û��� 
				int username_flag = 0;			//0����δ��ע�ᣬ1�����ѱ�ע�� 
				for (int i = 0; i < usercnt; i++) {
					if (strcmp(recvBuf, users[i].username) == 0)
						username_flag = 1;
				}
				//���߿ͻ��˸��û���ע����� 
				char flagBuf[4];
				if (username_flag == 1) {
					strcpy(flagBuf, "1");
				}
				else {
					strcpy(flagBuf, "0");
				}
				send(*ClientSocket, flagBuf, 4, 0);
				//δ��ע�������ע�� 
				if (username_flag == 0) {
					strcpy(users[usercnt].username, recvBuf);	//�����û��� 
					
					strcpy(recvBuf, "������������룺");
					send(*ClientSocket, recvBuf, 4024, 0);			//��ʾ�������� 
					cout << recvBuf;
					
					//�յ��ͻ��˷���������
					recv(*ClientSocket, recvBuf, 4024, 0); 
					strcpy(users[usercnt++].password, recvBuf);	//�������� 
					
//					for (int i = 0; i < usercnt; i++) {
//						cout << users[i].username;
//						cout << "��" << users[i].password;
//					}
					
					cout << recvBuf << endl;		//����������� 
					
					strcpy(recvBuf, "ע��ɹ���\n");
					users[usercnt].logCnt = 0;		//��¼������0 
					users[usercnt].onlineOrNot = 0;	//������ 
					send(*ClientSocket, recvBuf, 4024, 0);
					cout << recvBuf << endl;
					
					goto LOGMENU;
				}
				//�ѱ�ע���򷵻ز˵�����ʾ
				else {
					strcpy(recvBuf, "�ǳ���Ǹ�����û����ѱ�ע�ᣬ������ѡ��");
					send(*ClientSocket, recvBuf, 4024, 0);		//����������ʾ
					cout << recvBuf << endl;
					
					goto LOGMENU;
				}
			}
			//2����¼
			if (recvBuf[0] == '2') {
				strcpy(recvBuf, "***************��¼*****************\n�������û�����");
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				
				recv(*ClientSocket, recvBuf, 4024, 0);		//�յ��ͻ��˷������û���
				cout << recvBuf << endl;		//��������û��� 
				int username_flag = 0;			//0����δ��ע�ᣬ1�����ѱ�ע�� 
				for (int i = 0; i < usercnt; i++) {
					if (strcmp(recvBuf, users[i].username) == 0) {
						username_flag = 1;
						logSrNo = i;			//��¼�û����������� 
						break;
					}
				}
				
				//�Ƿ�ɵ�¼���͸��ͻ��� 
				char flagBuf[4];
				if (username_flag == 1) {
					strcpy(flagBuf, "1");
				}
				else {
					strcpy(flagBuf, "0");
				}
				send(*ClientSocket, flagBuf, 4, 0);
				
				if (username_flag == 1) {
					strcpy(recvBuf, "���������룺");
					send(*ClientSocket, recvBuf, 4024, 0);			//��ʾ�������� 
					cout << recvBuf;
					
					//�յ��ͻ��˷���������
					recv(*ClientSocket, recvBuf, 4024, 0); 
					cout << recvBuf << endl;
					
					//������ȷ 
					if (strcmp(recvBuf, users[logSrNo].password) == 0) {
						//Ŀǰ���������¼�ɹ� 
						if (users[logSrNo].onlineOrNot == 0) {		
							strcpy(recvBuf, "��¼�ɹ���\n");
							users[logSrNo].onlineOrNot = 1;		//��1��ʾ���� 
							cout << recvBuf << endl;
							send(*ClientSocket, recvBuf, 4024, 0);
							//��¼����+1 
							users[logSrNo].logCnt++;
						}
						//���������¼ʧ��
						else {
							strcpy(recvBuf, "��ǰ�˺��ѱ���¼�����Ժ��ٳ��ԣ�\n");
							cout << recvBuf << endl;
							send(*ClientSocket, recvBuf, 4024, 0); 
							
							goto LOGMENU;
						} 
					}
					//������� 
					else {
						strcpy(recvBuf, "�û������������������ѡ��\n");
						cout << recvBuf << endl;
						send(*ClientSocket, recvBuf, 4024, 0);
						
						goto LOGMENU;
					}
				}
				//δע���򷵻ز˵�����ʾ
				else {
					strcpy(recvBuf, "�ǳ���Ǹ�����û���δע�ᣬ������ѡ��");
					send(*ClientSocket, recvBuf, 4024, 0);		//����������ʾ
					cout << recvBuf << endl;
					
					goto LOGMENU;
				}
			}
			//3��ע��
			if (recvBuf[0] == '3') {
				strcpy(recvBuf, "������Ҫע�����û�����");
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << recvBuf << endl;
				
				recv(*ClientSocket, recvBuf, 4024, 0);	//���յ��û���
				cout << recvBuf << endl;		//��������û��� 
				int username_flag = 0;			//0����δ��ע�ᣬ1�����ѱ�ע�� 
				int srNo = 0;
				for (int i = 0; i < usercnt; i++) {
					if (strcmp(recvBuf, users[i].username) == 0) {
						username_flag = 1;
						srNo = i;			//��¼�û����������� 
						break;
					}
				}
				
				//�Ƿ��ע�����͸��ͻ��� 
				char flagBuf[4];
				if (username_flag == 1) {
					strcpy(flagBuf, "1");
				}
				else {
					strcpy(flagBuf, "0");
				}
				send(*ClientSocket, flagBuf, 4, 0);
				
				if (username_flag == 1) {
					strcpy(recvBuf, "�������¼���룺");
					send(*ClientSocket, recvBuf, 4024, 0);			//��ʾ�������� 
					cout << recvBuf;
					
					//�յ��ͻ��˷���������
					recv(*ClientSocket, recvBuf, 4024, 0); 
					cout << recvBuf << endl;
					
					//������ȷ 
					if (strcmp(recvBuf, users[srNo].password) == 0) {
						//ע���˺�
						if (srNo != usercnt) {
							for (int i = srNo; i < usercnt - 1; i++) {
								strcpy(users[srNo].password, users[srNo + 1].password);
								strcpy(users[srNo].username, users[srNo + 1].username);
							}
							usercnt--;
						}
						else 
							usercnt--;
						
						strcpy(recvBuf, "ע���ɹ���\n");
						cout << recvBuf << endl;
						send(*ClientSocket, recvBuf, 4024, 0);
						
						goto LOGMENU;		//���ص�¼�˵� 
					}
					//������� 
					else {
						strcpy(recvBuf, "�û������������������ѡ��\n");
						cout << recvBuf << endl;
						send(*ClientSocket, recvBuf, 4024, 0);
						
						goto LOGMENU;
					}
				}
				//�û���δע�� 
				else {
					strcpy(recvBuf, "�ǳ���Ǹ�����û���δע�ᣬ������ѡ��");
					cout << recvBuf << endl;
					send(*ClientSocket, recvBuf, 4024, 0);
					
					goto LOGMENU;
				}
			} 
			//4���˳� 
			if (recvBuf[0] == '4') {
				break;
			}
			
			/*�û���¼*/
			//����˸�֪�ͻ��˸��û�Ϊ�ڼ��ε�½ 
			char logCnt[4];
			if (users[logSrNo].logCnt == 1)
				strcpy(logCnt, "1");
			else 
				strcpy(logCnt, "0");
			send(*ClientSocket, logCnt, 4, 0);
			
			//�û�ע����һ�ε�½�����������û����侫��
			if (users[logSrNo].logCnt == 1) {
				
				srand((int)time(0));		//�����������
				//���ֶ�Ӧ���־��飬�Դ˷��� 
				int flag[8] = {0, 0, 0, 0, 0, 0, 0, 0};	//Ϊ��ֹ�ظ����û����飬�ж��Ƿ��ظ� 
				for (int i = 0; i < 3; i++) {
					switch ((rand() % 8 + 1)) {
						case 1:	{
								if (flag[0] == 0) {
									Type s1("������", "����սè");
									users[logSrNo].my_spirit[i] = s1;
									flag[0] = 1;
									break;	
								}
								else {
									i--;		//����ѡ��i����1 
									break;
								}
						}
				//				mine.my_spirit.show();
						case 2:	{
								if (flag[1] == 0) {
									Type s2("������", "��Ұ��ţ");
									users[logSrNo].my_spirit[i] = s2;
									flag[1] = 1;
									break;
								}
								else {
									i--;		//����ѡ��i����1 
									break;
								}
						}
				//				mine.my_spirit.show();
						case 3:	{
								if (flag[2] == 0) {
									Type s3("�����", "����");
									users[logSrNo].my_spirit[i] = s3;
									break;
								}
								else {
									i--;		//����ѡ��i����1 
									break;
								}
						}
						case 4:	{
								if (flag[3] == 0) {
									Type s4("�����", "�¶�");
									users[logSrNo].my_spirit[i] = s4;
									flag[3] = 1;
									break;
								}
								else {
									i--;		//����ѡ��i����1 
									break;
								}
						}
						case 5:	{
								if (flag[4] == 0) {
									Type s5("������", "�Ϲ�ս��");
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
									Type s6("������", "���ܰ���");
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
									Type s7("������", "����ʨ��");
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
									Type s8("������", "���ɻ���");
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
				
				//�������־��鵽�ͻ���
				for (int i = 0; i < 3; i++) {
					strcpy(recvBuf, users[logSrNo].my_spirit[i].type.c_str());
					send(*ClientSocket, recvBuf, 4024, 0);
					//����������
					char type_flag[4];
					recv(*ClientSocket, type_flag, 4, 0);		//�յ��ͻ��˷���
					while (type_flag[0] != '1') {
						strcpy(recvBuf, users[logSrNo].my_spirit[i].type.c_str());
						send(*ClientSocket, recvBuf, 4024, 0);
						recv(*ClientSocket, type_flag, 4, 0);
					}
					
					strcpy(recvBuf, users[logSrNo].my_spirit[i].name.c_str());
					send(*ClientSocket, recvBuf, 4024, 0);
				}
				//��������ӡ 
				users[logSrNo].my_spirit[0].show();
				users[logSrNo].my_spirit[1].show();
				users[logSrNo].my_spirit[2].show();
			}
			
			//�û��˵�
USERMENU:
			char userMenu[4024] = {"\n************************************\n\
*         1���鿴�ҵľ���          *\n\
*         2���鿴�����û�״̬      *\n\
*         3�������û�              *\n\
*         4���˳���¼              *\n\
************************************\n\
	    ���������ѡ��\n"};
	        send(*ClientSocket, userMenu, 4024, 0);
			cout << userMenu << endl; 
			//�û�����ѡ��
			recv(*ClientSocket, recvBuf, 4024, 0);	//�����������ȴ���������
			cout << recvBuf << endl;		//����û�ѡ��
			
			//1���鿴�ҵľ���
			if (recvBuf[0] == '1') {
				//�������־��鵽�ͻ���
				for (int i = 0; i < 3; i++) {
					strcpy(recvBuf, users[logSrNo].my_spirit[i].type.c_str());
					send(*ClientSocket, recvBuf, 4024, 0);
					//����������
					char type_flag[4];
					recv(*ClientSocket, type_flag, 4, 0);		//�յ��ͻ��˷���
					while (type_flag[0] != '1') {
						strcpy(recvBuf, users[logSrNo].my_spirit[i].type.c_str());
						send(*ClientSocket, recvBuf, 4024, 0);
						recv(*ClientSocket, type_flag, 4, 0);
					}
					strcpy(recvBuf, users[logSrNo].my_spirit[i].name.c_str());
					send(*ClientSocket, recvBuf, 4024, 0);
				}
				//��������ӡ 
				users[logSrNo].my_spirit[0].show();
				users[logSrNo].my_spirit[1].show();
				users[logSrNo].my_spirit[2].show();
				
				goto USERMENU;
			}
			
			//2���鿴�����û�״̬ 
			if (recvBuf[0] == '2') {
				strcpy(recvBuf, "�û��б�\n");	
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				for (int i = 0; i < usercnt; i++) {
					strcpy(recvBuf, users[i].username);		//�û��� 
					send(*ClientSocket, recvBuf, 4024, 0);
					cout << recvBuf << "����";
					strcpy(recvBuf, "����");
					send(*ClientSocket, recvBuf, 4024, 0);
					//����״̬ 
					if (users[i].onlineOrNot == 1) {
						strcpy(recvBuf, "����");			//������� 
						send(*ClientSocket, recvBuf, 4024, 0);
						cout << recvBuf << endl;
					}
					else {
						strcpy(recvBuf, "����");			//������� 
						send(*ClientSocket, recvBuf, 4024, 0);
						cout << recvBuf << endl;
					}
				}
				strcpy(recvBuf, "");
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << endl;
				
				goto USERMENU;
			}
			
			//3�������û�
			if (recvBuf[0] == '3') {
				strcpy(recvBuf, "������Ҫ�������û�����");
				int found_flag = 0;			//1�ҵ���0δ�ҵ� 
				send(*ClientSocket, recvBuf, 4024, 0);
				cout << recvBuf;
				recv(*ClientSocket, recvBuf, 4024, 0);		//���տͻ���������û��� 
				cout << recvBuf << endl;
				//���Ҹ��û� 
				for (int i = 0; i < usercnt; i++) {
					if (strcmp(recvBuf, users[i].username) == 0) {	//ƥ���� 
						found_flag = 1;
						strcpy(recvBuf, "1");		
						send(*ClientSocket, recvBuf, 4024, 0);		//���ҵ�����֪�ͻ��� 
						cout << recvBuf << endl;
						
						/*����û�����״̬*/
						strcpy(recvBuf, users[i].username);			//�û��� 
						send(*ClientSocket, recvBuf, 4024, 0);
						cout << recvBuf << "����";
						strcpy(recvBuf, "����");
						send(*ClientSocket, recvBuf, 4024, 0);
						//����״̬ 
						if (users[i].onlineOrNot == 1) {
							strcpy(recvBuf, "����");			//������� 
							send(*ClientSocket, recvBuf, 4024, 0);
							cout << recvBuf << endl;
						}
						else {
							strcpy(recvBuf, "����");			//������� 
							send(*ClientSocket, recvBuf, 4024, 0);
							cout << recvBuf << endl;
						}
						
						/*������û�ӵ�еľ���*/
						//�����û���ע���δ��½������û����ȡϵͳ�͵���ֻ����
						if (users[i].logCnt == 0) {
							strcpy(recvBuf, "0");
							send(*ClientSocket, recvBuf, 4024, 0);	//��֪�ͻ����޾���
						} 
						//�����û�ӵ�о��� 
						else {
							strcpy(recvBuf, "1");
							send(*ClientSocket, recvBuf, 4024, 0);	//��֪�ͻ����о���
							//�������־��鵽�ͻ���
							for (int j = 0; j < 3; j++) {
								strcpy(recvBuf, users[i].my_spirit[j].type.c_str());
								send(*ClientSocket, recvBuf, 4024, 0);
								//����������
								char type_flag[4];
								recv(*ClientSocket, type_flag, 4, 0);		//�յ��ͻ��˷���
								while (type_flag[0] != '1') {
									strcpy(recvBuf, users[i].my_spirit[j].type.c_str());
									send(*ClientSocket, recvBuf, 4024, 0);
									recv(*ClientSocket, type_flag, 4, 0);
								}
								strcpy(recvBuf, users[i].my_spirit[j].name.c_str());
								send(*ClientSocket, recvBuf, 4024, 0);
							}
							//��������ӡ 
							users[i].my_spirit[0].show();
							users[i].my_spirit[1].show();
							users[i].my_spirit[2].show();
						}
					}
				}
				//δ�ҵ� 
				if (found_flag == 0) {
					strcpy(recvBuf, "0");		
					send(*ClientSocket, recvBuf, 4024, 0);		//��δ�ҵ�����֪�ͻ��� 
				}
				
				cout << endl;
				goto USERMENU; 
			}
			
			//4���˳���¼
			if (recvBuf[0] == '4') {
				cout << "�˳���¼��" << endl;
				users[logSrNo].onlineOrNot = 0;
				goto LOGMENU;
			} 
			 
//			cout << "�������   " << endl;
//			if (strcmp("cls", recvBuf) == 0) {
//				//�����ִ������
//				system(recvBuf); 
//			}
//			else if (strcmp("��ȡ�汾��Ϣ", recvBuf) == 0) {
//				//��������
//				string verData = "Version: 1.0.1\nAuthor: Primer\nReleaseData: 2020-9-3";
//				int sLen = send(recvClientSocket, (char*)verData.c_str(), verData.length(), 0); 
//			} 
//			else if (strcmp("exit", recvBuf) == 0) {
//				cout << endl << "�˳�������" << endl;
//				break;
//			}
//			else {
//				cout << "\t����ȷ..." << endl;
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
    	printf("�Ѵ��׽���\n");            
    }
    else
    {
        //��һ�����׽���
        printf("Ƕ����δ��!");
    }//�����׽��֣�socketǰ��һЩ��鹤�����������������,ͨ��WSACleanup�ķ���ֵ��ȷ��socketЭ���Ƿ�����
	SOCKET ListenSocket = socket(AF_INET,SOCK_STREAM,0);//�����˿�ʶ���׽���
	SOCKADDR_IN ListenAddr;
	ListenAddr.sin_family=AF_INET;
	ListenAddr.sin_addr.S_un.S_addr=INADDR_ANY;//��ʾ���뱾��ip��ַ 
	ListenAddr.sin_port=htons(PORT);//�󶨶˿�
	//Ҫ�󶨵Ĳ���
	int n;
	n=bind(ListenSocket,(LPSOCKADDR)&ListenAddr,sizeof(ListenAddr));//�����
	if(n==SOCKET_ERROR){
		printf("�˿ڰ�ʧ�ܣ�\n");
		return -1;
	}
	else{
		printf("�˿ڰ󶨳ɹ���%d\n",PORT);
	}
	
	int l =listen(ListenSocket,20);//���еڶ������������ܹ����յ�����������
	printf("�����׼���������ȴ���������\n");
	
	while(1){
		//ѭ�����տͻ����������󲢴��������߳�
		SOCKET *ClientSocket = new SOCKET;
		ClientSocket=(SOCKET*)malloc(sizeof(SOCKET));
		//���տͻ�����������
		int SockAddrlen = sizeof(sockaddr);
		*ClientSocket = accept(ListenSocket,0,0);
		printf("һ���ͻ��������ӵ���������socket�ǣ�%d\n",*ClientSocket);
		CreateThread(NULL,0,&ServerThread,ClientSocket,0,NULL);
	}//while
	closesocket(ListenSocket);
	WSACleanup();
	return(0);
}//main

