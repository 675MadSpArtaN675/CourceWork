#define _CRT_SECURE_NO_WARNINGS
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

//Размер игрового поля головоломки
#define N 9 

// Прототипы функций
//====================================================================================//

int Game(char* tb[][N], char* nw_file, char* save_file, const char* const unc_tb);

void PrintGame(const char* s[][N]);
void ReadGame(char* s[][N], char* filename);
void EnterValue(char* s[][N], int row, int column, int answer, const char* const unc_tb);

int isBlack(char* s[][N], int row, int column);
int AccessEnter(char* s[][N], int row, int column, char* answer);

int isFileHas(char* filename);

int SaveGame(char* s[][N], char* filename);

int hasInStr(char* str, char f);

void InicializeCheckArray(int* arr);
void addElementInCheckArr(int* arr, int value);
int compare(const void* x1, const void* x2);
int CheckArray(int arr[]);

int check_str(char* s[][N], int row, int OnColumns);
int ConstCheck(const char* const un_s[][N], int row, int column);

int game_end(char* s[][N]);

void help(int skipAnimation);

//====================================================================================//


int main()
{
	setlocale(LC_ALL, "RUS");

	char* unch_table[N][N]; // Таблица с константными клетками
	char* table[N][N]; // Основная игровая таблица

	char nw_file[] = "game.txt";
	char save_file[] = "saved.txt";
	char consts_save_file[] = "nw_saved.txt";

	// Главное меню игры
	while (1)
	{
		int act;

		printf("\tГОЛОВОЛОМКА 'СТРИТ'\n\nВыберите действие: \n\t1 - Новая игра; %s\n\t3 - Помощь; \n\t4 - Выход.\n\nВвод действия: ",
			(isFileHas(save_file)) ? "\n\t2 - Загрузить игру;" : "");

		scanf("%i", &act);


		switch (act)
		{
		case 1: // Новая игра
			if (isFileHas(nw_file))
			{
				ReadGame(unch_table, nw_file);
				SaveGame(unch_table, consts_save_file);

				Game(table, nw_file, save_file, unch_table);
				return 0;
			}
			else
			{
				system("cls");
				puts("Файл игрового поля game.txt не найден.");
				return 0;
			}
			break;

		case 2: // Загрузка игры(если есть файлы сохранения)
			if (isFileHas(save_file) && isFileHas(consts_save_file))
			{
				ReadGame(unch_table, consts_save_file);

				Game(table, save_file, save_file, unch_table);
				return 0;
			}
			break;

		case 3: // Помощь
			system("cls");
			help(0);
			break;

		case 32: // Помощь(Без анимации)
			system("cls");
			help(1);
			break;

		case 4: // Выход
			system("cls");
			return 0;

		default:
			system("cls");
			puts("Неизвестное действие. Повторите попытку");

			Sleep(1500);

			system("cls");
			break;
		}
	}
}

/*

Тело игры*/
int Game(char* tb[][N], char* nw_file, char* save_file, const char *const unc_tb)
{
	ReadGame(tb, nw_file);

	while (1)
	{
		int row, column, answer;

		PrintGame(tb);

		scanf("%ix%i:%i", &row, &column, &answer);

		EnterValue(tb, row, column, answer, unc_tb);
		SaveGame(tb, save_file);
		
		int end = game_end(tb);
		
		if (end)
			break;
	}
}

/*
Вывод игрового поля*/
void PrintGame(const char* s[][N])
{
	system("cls");
	printf("\n---------------------------\n");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			char* st = s[i][j];
			printf("|%2s", st);
		}
		printf("|\n---------------------------\n");
	}
	printf("\nОтвет: ");
}

/* Чтение поля игры из файла*/
void ReadGame(char* s[][N], char* filename)
{
	FILE* to_read = fopen(filename, "rt");
	int f, i = 0;
	char str[5] = "";

	while (((f = fgetc(to_read)) != EOF) || i < 81)
	{
		// Подсчитываем строку и столбец массива и сохраняем текущий символ в переменную r_char
		int row = i / 9;
		int column = i % 9;
		char r_char = (char)f;

		// Если не найден переход на следующую строку, не найден пробел и конец файла, то текущий символ добавляем в строку str
		if ((r_char != ' ') && (r_char != '\n') && (r_char != EOF))
			str[strlen(str)] = r_char;
		else
		{
			// Выделяем новую ячейку памяти для полученной строки
			char* str_n = (char*)calloc(strlen(str), sizeof(char));
			// Превращаем "e" в пробел
			for (int x = 0; x < 5; x++)
			{
				char sym = str[x];
				if (sym == '\0')
					break;
				if (sym == 'e')
					str[x] = ' ';

			}

			strcpy(str_n, str);

			// Добавляем в игровое поле и очищаем str
			s[row][column] = str_n;

			for (int j = 0; j < 5; j++)
				str[j] = '\0';

			i++;
		}
	}

	fclose(to_read);
}

/* Ввод значения в ячейку*/
void EnterValue(char* s[][N], int row, int column, int answer, const char* const unc_tb)
{
	if ((0 < row <= 9) && (0 < column <= 9) && (answer > 0))
	{
		// Преобразование ответа из числа в строку
		char aswr = (char)(answer + 48);
		char* st = (char*)calloc(5, sizeof(char));
		st[0] = '\0';

		strncat(st, &aswr, 1);

		//Преобразование к строкам и столбцам массива
		row -= 1;
		column -= 1;

		// Проверка допустимости ввода
		if(AccessEnter(s,row, column, st) && !(isBlack(s, row, column)) && (ConstCheck(unc_tb, row, column)))
			s[row][column] = st;
	}
}

/* Проверка: чёрная ли ячейка(0 - не черная, 1 - черная)*/
int isBlack(char* s[][N], int row, int column)
{
	char* str = s[row][column];
	int str_len = strlen(str);

	//Проверяем ячейку на наличие 0
	for (int k = 0; k < str_len; k++)
		if (str[k] == '0')
			return 1;

	return 0;
}

/* Проверка допустимости ввода(0 - нельзя вводить, 1 - можно вводить)*/
int AccessEnter(char* s[][N], int row, int column, char* answer)
{
	for (int i = 0; i < N; i++)
	{
		char* x = s[row][i];
		char* y = s[i][column];

		// Проверка строки row на наличие значения answer
		for(int k = 0; k < strlen(x); k++)
			if (x[k] == answer[0])
				return 0;

		// Проверка столбца column на наличие значения answer
		for (int k = 0; k < strlen(y); k++)
			if (y[k] == answer[0])
				return 0;
	}

	return 1;
}

/* Проверка существует ли файл(0 - не существует, 1 - существует)*/
int isFileHas(char* filename)
{
	FILE* out = fopen(filename, "rt");

	if (out == NULL)
		return 0;
	else
	{
		fclose(out);
		return 1;
	}
}

/* Сохранение игры (1 - удалось сохранить, 0 - не удалось сохранить)*/
int SaveGame(char* s[][N], char* filename)
{
	FILE* out = fopen(filename, "wt");

	if (out == NULL)
	{
		puts("Ошибка открытия файла сохранения");
		return 0;
	}

	//Строка таблицы игрового поля
	char full_str[300] = "";

	//Преобразование таблицы игрового поля в одну строку
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			char str[5] = "";

			//Копируем содержимое ячейки
			strcpy(str,s[i][j]);

			//Пробелы(или пустые ячейки) заменяем на символ "e"
			for (int k = 0; k < 5; k++)
				if (str[k] == ' ')
					str[k] = 'e';

			//Добавляем пробел между символами
			strcat(str, " ");

			strcat(full_str, str);
		}
		//Самый последный пробел в строке меняем на знак перехода на новую строку
		full_str[strlen(full_str)-1] = '\n';
	}

	//Удаляем самый последный знак перехода на новую строку в последней строчке
	full_str[strlen(full_str) - 1] = '\0';

	//Записываем в файл, и закрываем его
	fputs(full_str, out);
	fclose(out);

	return 1;
}

/* Есть ли в строке str символ f (1 - в строке есть символ f, 0 - в строке нет символа f)*/
int hasInStr(char* str, char f)
{
	for (int i = 0; i < strlen(str); i++)
		if (str[i] == f)
			return 1;

	return 0;
}

/*Инициализируем(или очищаем) проверочный массив*/
void InicializeCheckArray(int* arr)
{
	for (int i = 0; i < 10; i++)
		arr[i] = 0;
}

/* Добавление элементов в проверочный массив*/
void addElementInCheckArr(int* arr, int value)
{
	for (int i = 0; i < 10; i++)
		if (arr[i] == 0)
		{
			arr[i] = value;
			break;
		}
}

/* Сравнение элементов массива*/
int compare(const void* x1, const void* x2)
{
	return (*(int*)x1 - *(int*)x2);
}
 
/* Проверка отсортированного массива. 
Возвращает 1 - если каждый последующий элемент массива на 1 больше предыдущего. Нули игнорируются*/
int CheckArray(int arr[])
{
	int prev_num = 0;
	for (int i = 0; i < 10; i++)
	{
		int num = arr[i];

		if (num == 0 || prev_num == 0)
			continue;
		else if (num - arr[i + 1] != 1)
			return 0;
		else if (i == 9 && num != 0 && prev_num == 0)
			return 1;

		prev_num = num;
	}
	
	return 1;
}

/*Проверка строки таблицы. Если OnColumns != 0, то делается проверка по столбцам*/
int check_str(char* s[][N], int row, int OnColumns)
{
	// В этот массив сохраняются значения от первой чёрной ячейки или первого конца строки 
	// до следующей черной ячейки или до конца строки(Столбца)

	int* arr = (int*)calloc(10, sizeof(int));
	InicializeCheckArray(arr);

	for (int i = 0; i < N + 1; i++)
	{
		char* str = "";

		if ((i < N) && OnColumns == 0)
			str = s[row][i];
		else if ((i < N) && OnColumns == 1)
			str = s[i][row];

		//Если найдена чёрная ячейка или дошли до конца строки, то
		if (hasInStr(str, '0') || (i == N))
		{
			// Сортируем массив
			qsort(arr, 10, sizeof(int), compare);

			// Проверяем его
			int a = CheckArray(arr);

			// Очищаем массив
			InicializeCheckArray(arr);

			// Если a = 0, возвращаем 0
			if (a == 0)
			{
				free(arr);
				return 0;
			}

			continue;
		}
		else if (hasInStr(str, ' '))
		{
			free(arr);
			return 0;
		}
		else
		{
			int num = (int)str[0] - 48;
			addElementInCheckArr(arr, num);
		}
	}
	
	free(arr);
	return 1;

}

/* Проверка констант в таблице*/
int ConstCheck(const char* const un_s[][N], int row, int column)
{
	char* cell = un_s[row][column];

	for (int i = 0; i < strlen(cell); i++)
	{
		if (cell[i] != ' ')
			return 0;
	}

	return 1;
}

/* Проверка конца игры*/
int game_end(char* s[][N])
{
	//Проверяем комбинации строк
	for (int i = 0; i < N; i++)
	{

		int a = check_str(s, i, 0);
		if (a == 0)
			return 0;
	}

	// Проверяем комбинации столбцов
	for (int i = 0; i < N; i++)
	{
		int a = check_str(s, i, 1);
		if (a == 0)
			return 0;
	}

	system("cls");
	printf("Поздравляем!!! Вы успешно решили головоломку.");
	return 1;
}

/* Правила игры из файла help.txt*/
void help(int skipAnimation)
{
	FILE* help = fopen("help.txt", "rt");

	if (help == NULL)
	{
		system("cls");
		puts("Ошибка открытия файла help.txt");
		return;
	}

	int c = 0;

	while ((c = fgetc(help)) != EOF)
	{
		char symbol = (char)c;
		printf("%c", symbol);

		if(skipAnimation == 0)
		{
			if ((symbol != '\t') && (symbol != ' ') && (symbol != '\n'))
				Sleep(60);

			else if (symbol == ';')
				Sleep(80);

			else if (((symbol == '.') || (symbol == ':')))
				Sleep(350);
		}
	}

	fclose(help);

	printf("Нажмите ENTER для возвращения в главное меню...");

	char xxx;
	scanf("%c",&xxx);
	getchar();

	system("cls");
	
}