#include <iostream>
#include <string>
#include <tuple>
#include <fstream>

template<class T>
class HashMap {

private:

	int size;
	int current_allocated_size;
	float max_loading_factor;

	std::tuple<std::string, T, int>* array;
	int currently_deleted_cnt;

	std::tuple<std::string, T, int>* arr_begin, * arr_end;
	int start_pos, end_pos;
	int biggest_prime_value;
	//0 neocupat -1 deleted

	long long int hash_generator(const std::string& key)
	{
		size_t hash = 0;
		hash = std::hash<std::string>()(key);
		return hash;
	}
	long long int Hashing1(const long long int& hash)
	{
		return hash;
	}
	long long int Hashing2(const long long int& hash)
	{
		return (2 * (hash / current_allocated_size) + 1);
	}


	void CheckEnds(int pos, std::tuple<std::string, T, int>*& arr)
	{
		if (start_pos > pos)
		{
			start_pos = pos;
			arr_begin = &(arr[pos]);
		}
		if (end_pos - 1 < pos)
		{
			end_pos = pos + 1;
			arr_end = &(arr[pos + 1]);
		}
		return;
	}


	void reHashing()
	{
		int new_allocated_size = current_allocated_size * 2;
		start_pos = new_allocated_size + 1;
		end_pos = 0;
		arr_begin = arr_end = nullptr;
		current_allocated_size = new_allocated_size;

		std::tuple<std::string, T, int>* aux = new std::tuple<std::string, T, int>[new_allocated_size];

		for (int i = 0; i < current_allocated_size / 2; ++i)
		{
			if (std::get<2>(array[i]) > 0)
			{
				size_t h = hash_generator(std::get<0>(array[i]));
				size_t h1 = Hashing1(h);
				size_t h2 = Hashing2(h);
				int t = 0;
				do
				{
					int pos = (h1 + t * h2) % new_allocated_size;
					if (std::get<2>(aux[pos]) == 0)
					{
						aux[pos] = std::make_tuple(std::get<0>(array[i]), std::get<1>(array[i]), pos + 1);

						CheckEnds(pos, aux);
						break;
					}
					++t;
				} while (t != current_allocated_size);
			}
		}
		delete[] array;

		array = aux;
		currently_deleted_cnt = 0;
	}

public:

	class Iterator {
	private:
		std::tuple<std::string, T, int>* m_Ptr, * arr_end, * arr_begin;
	public:

		Iterator(std::tuple<std::string, T, int>* ptr = nullptr,
			std::tuple<std::string, T, int>* ptr1 = nullptr,
			std::tuple<std::string, T, int>* ptr2 = nullptr) :
			m_Ptr(ptr), arr_begin(ptr1), arr_end(ptr2) {};

		Iterator& operator++()
		{
			++m_Ptr;
			while (std::get<2>(*m_Ptr) <= 0 && m_Ptr < arr_end)
			{
				++m_Ptr;
			}
			return *this;
		}
		Iterator operator++(int)
		{
			++m_Ptr;
			Iterator aux = *this;
			while (std::get<2>(*m_Ptr) <= 0 && m_Ptr < arr_end)
			{
				++m_Ptr;
			}
			return aux;
		}
		Iterator& operator--()
		{
			--m_Ptr;
			while (std::get<2>(*m_Ptr) <= 0 && m_Ptr != arr_begin)
			{
				--m_Ptr;
			}
			return *this;
		}
		Iterator operator--(int)
		{
			--m_Ptr;
			Iterator aux = *this;
			while (std::get<2>(*m_Ptr) <= 0 && m_Ptr != arr_begin)
			{
				--m_Ptr;
			}
			return aux;
		}

		bool operator==(const Iterator& other) const
		{
			return m_Ptr == other.m_Ptr;
		}

		bool operator!=(const Iterator& other) const
		{
			return m_Ptr != other.m_Ptr;
		}

		Iterator* operator->()
		{
			return this;
		}

		Iterator& operator*()
		{
			return *(this);
		}

		std::string& first()
		{
			return std::get<0>(*m_Ptr);
		}

		T& second()
		{
			return std::get<1>(*m_Ptr);
		}

		Iterator operator-(int b)
		{
			Iterator c;
			c.m_Ptr = m_Ptr - b;
			return c;
		}

	};



public:
	HashMap()
	{
		size = 0;
		currently_deleted_cnt = 0;
		max_loading_factor = 0.7;
		array = new std::tuple<std::string, T, int>[8];
		current_allocated_size = 8;
		arr_begin = nullptr;
		arr_end = nullptr;
		start_pos = 8;
		end_pos = 0;
		biggest_prime_value = 7;
	}


	T& insert(std::string key, T info)
	{
		float loading_factor = 1. * (size + 1) / current_allocated_size;
		if (loading_factor >= max_loading_factor)
		{
			reHashing();
		}
		size_t h = hash_generator(key);
		size_t h1 = Hashing1(h);
		size_t h2 = Hashing2(h);
		int pos = 0;
		int t = 0;
		do
		{
			pos = (h1 + t * h2) % current_allocated_size;
			if (std::get<2>(array[pos]) == 0)
			{

				array[pos] = std::make_tuple(key, info, pos + 1);

				++size;
				CheckEnds(pos, array);
				return std::get<1>(array[pos]);
			}
			else if (std::get<2>(array[pos]) == -1)
			{

				array[pos] = std::make_tuple(key, info, pos + 1);

				++size;
				CheckEnds(pos, array);
				return std::get<1>(array[pos]);
			}
			else if (std::get<0>(array[pos]) == key)
			{
				return std::get<1>(array[pos]);
			}
			++t;
		} while (t != current_allocated_size);
		std::cout << key << '\n';

	}

	void remove(std::string key)
	{
		float corruption_factor = 1. * currently_deleted_cnt / current_allocated_size;
		if (corruption_factor > 0.5)
		{
			reHashing();
		}
		int t = 0;
		size_t h = hash_generator(key);
		size_t h1 = Hashing1(h);
		size_t h2 = Hashing2(h);
		do
		{
			int pos = (h1 + t * h2) % current_allocated_size;
			if (std::get<0>(array[pos]) == key)
			{
				std::get<2>(array[pos]) = -1;
				--size;
				if (&(array[pos]) == arr_begin)
				{
					bool found = false;
					for (int i = pos; i < current_allocated_size; ++i)
					{
						if (std::get<2>(array[i]) > 0)
						{
							arr_begin = &(array[i]);
							found = true;
							break;
						}
					}
					if (!found)
						arr_begin = arr_end = nullptr;
				}
				else if (&(array[pos]) == arr_end)
				{
					bool found = false;

					for (int j = pos; j >= 0; --j)
					{
						if (std::get<2>(array[j]) > 0)
						{
							arr_end = &(array[j]);
							found = true;
							break;
						}
					}
					if (!found)
						arr_begin = arr_end = nullptr;
				}

				return;
			}
			++t;
		} while (t != current_allocated_size);

		return;
	}

	std::tuple<std::string, T, int>* find(std::string key)
	{
		int t = 0;
		size_t h = hash_generator(key);
		size_t h1 = Hashing1(h);
		size_t h2 = Hashing2(h);
		do
		{
			int pos = (h1 + t * h2) % current_allocated_size;
			if (std::get<0>(array[pos]) == key && std::get<2>(array[pos]) != -1)
			{
				return &(array[pos]);
			}
			++t;
		} while (t != current_allocated_size);
		return nullptr;
	}



	T& operator[](std::string key)
	{
		T ignore;
		return insert(key, ignore);
	}

	Iterator begin()
	{
		return Iterator(arr_begin, arr_begin, arr_end);
	}

	Iterator end()
	{
		return Iterator(arr_end, arr_begin, arr_end);
	}

	int dimension()
	{
		return size;
	}
};

void menu()
{
	std::cout << "1 key value - Insert a value in HashTable with the specified key;\n";
	std::cout << "2 key - Check if the key exists, and if so, print the value;\n";
	std::cout << "3 key - Remove the element that has the specified key;\n";
	std::cout << "4 - Print the HashTable;\n";
	std::cout << "5 - Clear the screen;\n";
	std::cout << "6 - Size;\n";
	std::cout << "0 - Exit.\n";
}

int main()
{
	HashMap<std::string> map;
	int n;
	std::ifstream in("file.in");

	in >> n;
	for (int i = 0; i < n; ++i)
	{
		std::string s;
		std::string info;
		in >> s >> info;
		//map.insert(s, info);
		map[s] = info;
		//std::cout << i << '\n';
	}
	//std::cout << map.dimension();
	int x;
	std::string value;
	std::string key;
	std::tuple<std::string, std::string, int>* aux;
	menu();
	while (true)
	{

		std::cin >> x;
		switch (x)
		{
		case 0:
			return 0;
			break;
		case 1:
			std::cin >> key >> value;
			map[key] = value;
			break;
		case 2:
			std::cin >> key;
			aux = map.find(key);
			if (!aux)
				std::cout << "The key does not exist!\n";
			else
				std::cout << std::get<0>(*aux) << " - " << std::get<1>(*aux) << '\n';
			break;
		case 3:
			std::cin >> key;
			map.remove(key);
			break;
		case 4:
			for (auto it : map)
			{
				std::cout << "key: " << it.first() << ", value: " << it.second() << '\n';
			}
			break;
		case 5:
			system("CLS");
			menu();
			break;
		case 6:
			std::cout << map.dimension() << '\n';
			break;
		default:
			continue;
		}
	}
	return 0;
}