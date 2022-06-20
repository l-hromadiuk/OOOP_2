#pragma once

#include "RationalNum.h"
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;


//Генератор випадкового беззнакового цілого в проміжку [min, max]
unsigned get_random_uint(unsigned min, unsigned max) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(min, max);
	return dis(gen);
}

string concat(RationalNum num) {
	return (decToHexa(num.getNumerator()) + decToHexa(num.getDenominator()));

}
//Конст
//переведення числа в 16 систему числення
string decToHexa(int num)
{


	map<int, char> m;

	char digit = '0';
	char c = 'A';

	for (int i = 0; i <= 15; i++) {
		if (i < 10) {
			m[i] = digit++;
		}
		else {
			m[i] = c++;
		}
	}


	string res = "";


	if (!num) {
		return "0";
	}

	if (num > 0) {
		while (num) {
			res = m[num % 16] + res;
			num /= 16;
		}
	}

	else {

		unsigned int n = num;


		while (n) {
			res = m[n % 16] + res;
			n /= 16;
		}
	}

	return res;
}

//анти для хешування, P - достатньо велике просте число
namespace constants {
	const int P =199999;
	const int STIRNG_MOD = 53;
}

//Знаходження ключа для string
long long compute_hash(string const& str) {
	unsigned int hash_value = 0;
	long long p_pow = 1;
	for (char ch : str) {
		hash_value = (hash_value + (ch -'A' + 1) * p_pow) % constants::P;
		p_pow = (p_pow * constants::STIRNG_MOD) % constants::P;
	}
	return hash_value;
}

//Знаходження ключа для залізничної станції (за іменем)
long long compute_hash(RationalNum const& num) {
	return compute_hash(num.getNumerator()+num.getDenominator());
}
 

//Клас для вторинної хеш-таблиці
template <typename T>
class secondary_hash_table {
private:
	//коефіцієнти універсальної хеш-функції
	unsigned a;
	unsigned b;
	unsigned m;
	vector<T> data;
	//хеш-функція для знаходження ключа шаблонного параметру, задається користувачем в конструкторі
	long long (*hash_func)(T const&);

protected:
	//знаходить індекс за ключем (реалізація універсальної хеш-функції з параметрами a, b, P, m)
	unsigned get_index(long long const& key) const {
		return ((a * key + b) % constants::P) % m;
	}

public:
	secondary_hash_table(vector<T> const& _data, long long (*_hash_func)(T const&)) {
		//резервуємо пам'ять розміру <кількість значень, що потрапили в одну комірку первинної хеш-функції> в квадраті
		m = pow(_data.size(), 2);
		data.resize(m);
		//задаємо початкові значення коефіцієнтів хешування
		a = 2;
		b = get_random_uint(constants::P / 10, constants::P);

		//запам'ятовуємо ключі
		vector<long long> keys(_data.size());
		for (unsigned i = 0; i < _data.size(); i++) {
			keys[i] = _hash_func(_data[i]);
		}
		//масив-індикатор колізії, повертає true, якщо комірка вільна
		vector<bool> free(m, true);

		while (true) {
			unsigned index, i = 0;

			for (; i < keys.size(); i++) {
				//знаходимо індекс для і-го ключа
				index = get_index(keys[i]);
				//якщо комірка вільна, записуємо в неї шаблонний елемент
				if (free[index]) {
					data[index] = _data[i];
				}
				//інакше підбираємо інші коефіцієнти хешування
				else {
					a++;
					//якщо а перевищує 9, присвоюємо b інше випадкове значення в зазначеному проміжку
					if (a > 9) {
						a = 2;
						b = get_random_uint(constants::P / 10, constants::P);
					}

					//очищуємо масив-індикатор колізії та масив записаних елементів
					free.clear();
					free.resize(m, true);
					data.clear();
					data.resize(m);

					break;
				}
			}

			//якщо всі ключі захешувалися без колізій, завершуємо підбір коефіцієнтів
			if (i == keys.size()) {
				break;
			}
		}
	}

	unsigned get_multiplier() const {
		return a;
	}

	unsigned get_term() const {
		return b;
	}

	unsigned get_first_mod() const {
		return constants::P;
	}

	unsigned get_second_mod() const {
		return m;
	}

	//отримати елемент за ключем
	//якщо елемента з таким ключем в таблиці немає, повертаємо дефолтне значення шаблонного параметра
	T get(long long const& key) const {
		if (data.empty()) {
			return T();
		}
		return data[get_index(key)];
	}
};

//Клас, що описує первинну хеш-таблицю
template <typename T>
class perfect_hash_table {
private:
	unsigned a;
	unsigned b;
	unsigned m;
	vector<secondary_hash_table<T>> tables;
	long long (*hash_func)(T const&);

protected:
	unsigned get_index(long long const& key) const {
		return ((a * key + b) % constants::P) % m;
	}

public:
	perfect_hash_table(vector<T> const& data, long long (*_hash_func)(T const&)) {
		//перевіряємо вхідні дані на повторюваність, у випадку наявності двох однакових значень надсилаємо exception
		for (unsigned i = 0; i < data.size(); i++) {
			for (unsigned j = i + 1; j < data.size(); j++) {
				if (data[i] == data[j]) {
					throw std::invalid_argument("Values in hash-table repeat.");
				}
			}
		}

		vector<long long> keys;
		unsigned key, num_of_unique_keys = 0;
		a = 7;
		b = get_random_uint(constants::P / 10, constants::P);

		//шукаємо кількість унікальних ключів
		for (unsigned i = 0; i < data.size(); i++) {
			key = _hash_func(data[i]);
			for (unsigned j = 0; j != i && j < keys.size(); j++) {
				if (keys[j] == key) {
					num_of_unique_keys--;
					break;
				}
			}
			num_of_unique_keys++;
		}

		//резервуємо пам'ять для зберігання всіх унікальних ключів, поки що в тимчасових векторах
		m = num_of_unique_keys;
		unsigned index;
		vector<vector<T>> temp_cache(m);

		//розподіляємо ключі по тичасових векторах з відповідними індексами
		for (T const& item : data) {
			key = _hash_func(item);
			index = get_index(key);
			temp_cache[index].push_back(item);
		}

		//створємо m вторинних хеш-таблиць, що побудуються за відповідними тимчасовими векторами значень
		//також передаємо показчик на фунцію хешування для шаблонного елемента
		for (unsigned i = 0; i < m; i++) {
			tables.push_back(secondary_hash_table<T>{ temp_cache[i], _hash_func });
		}
	}

	unsigned get_multiplier() const {
		return a;
	}

	unsigned get_term() const {
		return b;
	}

	unsigned get_first_mod() const {
		return constants::P;
	}

	unsigned get_second_mod() const {
		return m;
	}

	//шукаємо значення з певним ключем у певній вторинній хеш-таблиці
	T get(long long const& key) const {
		return tables[get_index(key)].get(key);
	}
};
