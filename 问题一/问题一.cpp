#include <iostream>
#include <iomanip>
#include <cmath>
#include <windows.h>
#include <time.h>
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
		this -> crit = CRIT + 2;
		this -> dodge = DODGE;
	} 
	//����� 
	if ((name == "����") || (name == "�¶�")) {
		this -> aggressivity = 20;	
		this -> defense = 13;
		this -> HPMAX = 105;
		this -> HP = this -> HPMAX;
		this -> cooldown = 8;
		this -> crit = CRIT;
		this -> dodge = DODGE;
	}
	//������
	if ((name == "�Ϲ�ս��") || (name == "���ܰ���")) {
		this -> aggressivity = 20;	
		this -> defense = 20;
		this -> HPMAX = 90;
		this -> HP = this -> HPMAX;
		this -> cooldown = 8;
		this -> crit = CRIT;
		this -> dodge = DODGE;
	} 
	//������
	if ((name == "����ʨ��") || (name == "���ɻ���")) {
		this -> aggressivity = 30;	
		this -> defense = 5;
		this -> HPMAX = 55;
		this -> HP = this -> HPMAX;
		this -> cooldown = 4;
		this -> crit = CRIT;
		this -> dodge = DODGE + 1;
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
	if (level < 15) {
		if (EX >= EXMAX * level) {
			level += EX / (EXMAX * level);	//�ȼ����� 
			EX %= (EXMAX * level);			//�������� 
			cout << "************************************************" << endl;
			cout << this -> name << "�ȼ�������" << this -> level << "!" << endl;
			//��������
			cout << "���Ա仯��" << endl;
			
			if (type == "������") {
				//�����仯 
				cout << "��������" << aggressivity;
				aggressivity += PI;			//�������仯 
				cout << " -> " << aggressivity << endl;
				
				//�����ʱ仯
				cout << "�����ʣ�" << setprecision(2) << ((double)crit) / 100;
				crit += level * 2;
				//����������40% 
				if (crit >= CHL) 
					crit = CHL;
				cout << " -> " << setprecision(2) << ((double)crit) / 100 << endl;
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
				//��������仯 
				cout << "���������" << setprecision(2) << cooldown;
				cooldown *= NAIR;
				if (cooldown <= NMAI)	//�������;��鹥���������ΪNMAI 
					cooldown = NMAI;
				cout << " -> " << setprecision(2) << cooldown << endl;
				
				//�����ʱ仯
				cout << "�����ʣ�" << setprecision(2) << ((double)dodge) / 100;
				dodge += level;
				//����������MDL 
				if (dodge >= MDL)
					dodge = MDL;
				cout << " -> " << setprecision(2) << ((double)dodge) / 100 << endl; 
			}
			
			cout << "************************************************" << endl;
			cout << endl;
		}
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
	if (level < 15) {
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

//�û��ṹ�� 
typedef struct user {
	Type my_spirit;
}user;

//���Գ����д
int main()
{
	//�������о��� 
	Cat Vcat("������", "����սè");
	Ox Wox("������", "��Ұ��ţ");
	Luoyin LY("�����", "����");
	Odin OD("�����", "�¶�");
	Dragon ABdragon("������", "�Ϲ�ս��");
	Abu SPabu("������", "���ܰ���");
	Lion SWlion("������", "����ʨ��");
	Oriole Foriole("������", "���ɻ���");
	
	//�û���ѡ����
MYSPIRIT:
	cout << "************************************************************" << endl;
	cout << "1.";	Vcat.show();
	cout << "2.";	Wox.show();
	cout << "3.";	LY.show();
	cout << "4.";	OD.show();
	cout << "5.";	ABdragon.show();
	cout << "6.";	SPabu.show();
	cout << "7.";	SWlion.show();
	cout << "8.";	Foriole.show();
	cout << "************************************************************" << endl;
	cout << "�����Ӧ���ѡ����ľ���ɣ�" << endl; 
	int Sno;
	cin >> Sno;
	
	user mine;
	/*���û��������Ը�ֵ*/
	switch (Sno) {
		case 1:	{
					Type s1("������", "����սè");
					mine.my_spirit = s1;
					break;	
		}
//				mine.my_spirit.show();
		case 2:	{
					Type s2("������", "��Ұ��ţ");
					mine.my_spirit = s2;
					break;
		}
//				mine.my_spirit.show();
		case 3:	{
					Type s3("�����", "����");
					mine.my_spirit = s3;
					break;
		}
		case 4:	{
					Type s4("�����", "�¶�");
					mine.my_spirit = s4;
					break;
		}
		case 5:	{
					Type s5("������", "�Ϲ�ս��");
					mine.my_spirit = s5;
					break;
		}
		case 6:	{
					Type s6("������", "���ܰ���");
					mine.my_spirit = s6;
					break;
		}
		case 7:	{
					Type s7("������", "����ʨ��");
					mine.my_spirit = s7;
					break;
		}
		case 8:	{
					Type s8("������", "���ɻ���");
					mine.my_spirit = s8;
					break;
		}
		default:	cout << "������ȷ��Ų��ܻ�þ���Ŷ��" << endl;
					goto MYSPIRIT;
	} 
	cout << "���" << mine.my_spirit.name << "һֻ����ʼս���ɣ�" << endl;
	system("pause");
	
	
	//�˵�
MENU:
	system("cls");
	cout << "************************************************" << endl;
	cout << "*         1.��ʼս��       2.�鿴�ҵľ���      *" << endl;
	cout << "*         3.�˳���Ϸ       4.�ۿ�����ͼ��      *" << endl;
	cout << "************************************************" << endl;
	int choice;
	cin >> choice;
	if (choice == 1)
		goto FIGHT;
	else if (choice == 2) {
		//չʾ�û����� 
		mine.my_spirit.show();
		system("pause");
		goto MENU;
	}
	else if (choice == 3) 
		return 0;
	else {
		//չʾ���о��� 
		Vcat.show();
		Wox.show();
		LY.show();
		OD.show();
		ABdragon.show();
		SPabu.show();
		SWlion.show();
		Foriole.show();
		system("pause");
		//���ز˵� 
		goto MENU;
	}
	
	//��ѡ���ֽ׶� 
FIGHT:
	//��������ս���ľ��� 
	Cat EVcat("������", "����սè");
	Ox EWox("������", "��Ұ��ţ");
	Luoyin ELY("�����", "����");
	Odin EOD("�����", "�¶�");
	Dragon EABdragon("������", "�Ϲ�ս��");
	Abu ESPabu("������", "���ܰ���");
	Lion ESWlion("������", "����ʨ��");
	Oriole EForiole("������", "���ɻ���");
	//�û���ѡ��ս����
ENEMY:
	cout << "************************************************************" << endl;
	cout << "1.";	EVcat.show();
	cout << "2.";	EWox.show();
	cout << "3.";	ELY.show();
	cout << "4.";	EOD.show();
	cout << "5.";	EABdragon.show();
	cout << "6.";	ESPabu.show();
	cout << "7.";	ESWlion.show();
	cout << "8.";	EForiole.show();
	cout << "************************************************************" << endl;
	cout << "�����Ӧ�������ѡ��Ķ��ְɣ�" << endl;
	int ESno;
	cin >> ESno;
	while ( 1 ) {
		if (ESno < 1 || ESno > 8) {
			cout << "Ҫ������ȷ�Ķ�ս�������Ŷ��" << endl;
			goto ENEMY;
		}
		//�Եз��������Եĸ�ֵ 
		user enemy;				//�����û��ṹ�� 
		cout << "��Ϊ��ƥ����ʵĶ��֣�" << endl;
		system("pause");
		/*�Եз��������Ը�ֵ*/
		switch (ESno) {
			case 1:	{
						Type s1("������", "����սè");
						enemy.my_spirit = s1;
						break;	
			}
			case 2:	{
						Type s2("������", "��Ұ��ţ");
						enemy.my_spirit = s2;
						break;
			}
			case 3:	{
						Type s3("�����", "����");
						enemy.my_spirit = s3;
						break;
			}
			case 4:	{
						Type s4("�����", "�¶�");
						enemy.my_spirit = s4;
						break;
			}
			case 5:	{
						Type s5("������", "�Ϲ�ս��");
						enemy.my_spirit = s5;
						break;
			}
			case 6:	{
						Type s6("������", "���ܰ���");
						enemy.my_spirit = s6;
						break;
			}
			case 7:	{
						Type s7("������", "����ʨ��");
						enemy.my_spirit = s7;
						break;
			}
			case 8:	{
						Type s8("������", "���ɻ���");
						enemy.my_spirit = s8;
						break;
			}
		} 
		cout << endl;
		
		//��ս��ʼ
		system("cls");
		cout << "************************************************************" << endl;
		cout << "*                        ս����ʼ��                        *" << endl;
		cout << "*     �ҷ���" << mine.my_spirit.name;
		cout << "                    �з���" << enemy.my_spirit.name;
		cout << "     *" << endl;
		cout << "************************************************************" << endl;
		//���ھ�����Ķ����ԣ������ж�Ӧ�����õľ��鹥������ 
		
		int Mdamage = 0, Edamage = 0;		//�ֱ�����ҷ�������˺����з�������˺� 
		/*�ж�����*/
		//�ҷ����� 
		if (enemy.my_spirit.cooldown >= mine.my_spirit.cooldown) {	
			/*�ҷ�����Ĺ���*/
			if (mine.my_spirit.name == "����սè") {
				Vcat.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
				Mdamage = Vcat.attack();
				
				//�з������ܻ�����
				enemy.my_spirit.injury(Mdamage); 
			}
			if (mine.my_spirit.name == "��Ұ��ţ") {
				Wox.setLevel(mine.my_spirit.level);		//�ȼ�����һ��
				Mdamage = Wox.attack();
				
				//�з������ܻ�����
				enemy.my_spirit.injury(Mdamage); 
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
		mine.my_spirit.battleOver(enemy.my_spirit.level, enemy.my_spirit.HP);
		mine.my_spirit.levelUp();
		
		//���ز˵�
		system("pause");
		goto MENU; 
	}
}
