#include <iostream>
#include <fstream>
using namespace std;

const int max_len = 100;

typedef struct {
    char brand[max_len]; // Марка ЛА
    char tail_number[max_len]; // Бортовой номер
    int flight_number; // Номер рейса
    unsigned int boarding_hour; // Час посадки
    unsigned int boarding_min; // Время посадки
} Aircraft;

// Функция читает файл, контролируя, что он существует и не пуст. Подсчитывает
//количество ЛА в файле, далее - начинает построчную обработку данных и запись в массив ЛА.
int Open_file(const char* file_name, Aircraft *&arr, int &N);
// Функция обрабатывает строку, контролируя, что она не пустая и данные полные.
int Parse_line(char* line, Aircraft &arr);

// Функция проверяет коректность бортового номера.
int Check_tail_number(char* num);
//Функция обрабатывает цифровую часть бортового номера.
int Number(char *extra_s);
//Функция обрабатывает время и контролирует корректность времени посадки.
int Check_time(char* extra_s, unsigned int &hour, unsigned int &min);
//Функция выводит текст ошибки
void Print_error(int type);
//Функция выводит массив ЛА
void Print(Aircraft *&arr, int N, int *&B);
//Индексная сортировка
void IndexSort(Aircraft *&arr, int N, int *&B);
//Функция считает длину строки
int Length_of(char* str);
//Функция считает длину числа
int Length_of_number(int num);

int main(){
    const char* test_files[] = {" ",
        "program.dSYM/Contents/test0.txt",
        "program.dSYM/Contents/test1.txt",
        "program.dSYM/Contents/test4.txt",
        "program.dSYM/Contents/test7.txt",
        "program.dSYM/Contents/test3.txt",
        "program.dSYM/Contents/test5.txt",
        "program.dSYM/Contents/test6.txt",
        "program.dSYM/Contents/test2.txt",};

    char* descriptions[] = {"Отсутствие файла",
        "Пустой файл",
        "Неверный бортовой номер (Б-ВУ5)",
        "Отстуствие бортового номера",
        "Отсутствие номера рейса",
        "Некорректное время (50:15)",
        "Время посадки отсутствует",
        "Пустая строка",
        "Корректный файл"};

    for (int test = 0; test < sizeof(test_files) / 8; test++) { //Размер одного указателя - 8 байт
        cout << descriptions[test] << ":" << endl;

        Aircraft* arr = nullptr;
        int N = 0;
        int error = Open_file(test_files[test], arr, N);

        if (error != 0) {
            Print_error(error);
            cout << endl;
        }
        else {
            int* B = new int[N];
            int i = 0;
            for (i = 0; i < N; i++)
            {
                B[i] = i;
            }
            Print(arr, N, B); // Вывод исходных данных
            IndexSort(arr, N, B);
            cout << endl;
            Print(arr, N, B); // Вывод отсортированных данных
        }
        delete[] arr;
    }
    return 0;
}

int Open_file(const char* file_name, Aircraft *&arr, int &N) {
    ifstream file_0(file_name);
    if (!file_0.is_open()) return 2;
    if (file_0.peek() == EOF) return 3;

    char extra_s[max_len];
    while (file_0.getline(extra_s, max_len)) {
        if (extra_s[0] != '\0') N++; // Считываем не пустые строки
    }
    file_0.close();

    ifstream file(file_name);
    arr = new Aircraft[N];

    for (int i = 0; i < N; i++) {
        if (!file.getline(extra_s, max_len)) return 5;
        int line_error = Parse_line(extra_s, arr[i]);
        if (line_error !=0) return line_error;
    }
    return 0;
}

int Parse_line(char* line, Aircraft &arr) {
    if (line[0] == '\0') return 5;

    int pos = 0;
    //Считываем марку
    int i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len-1) {
        arr.brand[i++] = line[pos++];
    }
    arr.brand[i] = '\0';
    if (line[pos] == '\0') return 6;
    pos++; // Пропускаем пробел

    // Считываем бортовой номер
    i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len-1) {
        arr.tail_number[i++] = line[pos++];
    }
    arr.tail_number[i] = '\0';
    if (Check_tail_number(arr.tail_number)!=0) return Check_tail_number(arr.tail_number);
    if (line[pos] == '\0') return 8;
    pos++;

    //Считываем номер рейса
    char flight_str[max_len] = {0};
    i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len-1) {
        flight_str[i++] = line[pos++];
    }
    flight_str[i] = '\0';
    arr.flight_number = Number(flight_str);
    if (line[pos] == '\0') return 7;
    pos++;

    //Считываем время
    char time_str[max_len] = {0};
    i = 0;
    while (line[pos] != ' ' && line[pos] != '\0' && i < max_len-1) {
        time_str[i++] = line[pos++];
    }
    time_str[i] = '\0';
    unsigned int h = 0, m = 0;
    if (Check_time(time_str, h, m)!=0) return Check_time(time_str, h, m);

    arr.boarding_hour = h;
    arr.boarding_min = m;

    return 0;
}

int Check_tail_number(char* num) {
    if (num == nullptr || num[0] == '\0') return 6;
    int i = 0;
    if ((unsigned char)num[0] == 0xD0 && (unsigned char)num[1] == 0x91){
        i = 2;
    }
    else{
        return 1;
    }
    if (num[i]!= '-'){
        return 1;
    }
    i++;

    if (num[i] == '\0') {
        return 1;
    }
    while (num[i] != '\0') {
        if (num[i] < '0' || num[i] > '9') {
            return 1;
        }
        i++;
    }
    return 0;
}

int Number(char *extra_s) {
    int sum = 0;
    bool flag = false;
    for (int h = 0 ; extra_s[h] != '\0'; ) {
        if (extra_s[h] == '-')
        {
            flag = true;
            h++;
        }
        else if (extra_s[h] >= '0' && extra_s[h] <= '9') {
            sum = sum * 10 + (extra_s[h] - '0');
            h++;
        }
        else {
            do {
                h++;
            } while (extra_s[h] != '\0' && (extra_s[h] & 0xC0) == 0x80);
        }
    }
    if (flag == true) {
        sum *= -1;
    }
    return sum;
}

int Check_time(char* extra_s, unsigned int &hour, unsigned int &min){
    if (extra_s == nullptr || extra_s[0] == '\0') return 7;
    int i = 0;
    bool has_colon;

    while (extra_s[i] != '\0' && extra_s[i] != ':') {
        if (extra_s[i] >= '0' && extra_s[i] <= '9') {
            hour = hour * 10 + (extra_s[i] - '0');
        }
        else {
            return 4;
        }
        i++;
    }
    if (extra_s[i] != ':') return 4;

    has_colon = true;
    i++;

    int min_digits = 0;
    while (extra_s[i] != '\0') {
        if (extra_s[i] >= '0' && extra_s[i] <= '9') {
            min = min * 10 + (extra_s[i] - '0');
            min_digits++;
        }
        else return 4;
        i++;
    }

    if (!has_colon || min_digits == 0) return 4;

    if (hour > 23 || min > 59) return 4;

    return 0;
}

void Print_error(int type) {
    if (type == 1) cout << "Ошибка! Неверный бортовой номер" << endl;
    if (type == 2) cout << "Ошибка! Файл не найден" << endl;
    if (type == 3) cout << "Ошибка! Файл пустой" << endl;
    if (type == 4) cout << "Ошибка! Некорректное время (должен быть формат ЧЧ:ММ)" << endl;
    if (type == 5) cout << "Ошибка! Строка не считана" << endl;
    if (type == 6) cout << "Ошибка! Бортовой номер отсутствует" << endl;
    if (type == 7) cout << "Ошибка! Время посадки отсутствует" << endl;
    if (type == 8) cout << "Ошибка! Номер рейса отсутствует" << endl;
}

int Length_of(char* str) {
    if (str == nullptr) return 0;
    int len = 0;
    for (int i = 0; str[i] != '\0'; ) {
        unsigned char c = (unsigned char)str[i];
        if (c >= 0xC0) {
            len++;
            i+=2;
        }
        else {
            len++;
            i++;
        }
    }
    return len;
}

int Length_of_number(int num) {
    if (num == 0) return 1;
    int len = 0;
    if (num < 0) {
        len = 1;
        num = -num;
    }
    while (num > 0) {
        len++;
        num /= 10;
    }
    return len;
}

void Print(Aircraft *&arr, int N, int *&B) {
    cout << " ----------------------------------------------------------------" << endl;
    cout << "| № | Марка ЛА    | Бортовой номер | Номер рейса | Время посадки |" << endl;
    cout << " ----------------------------------------------------------------" << endl;
    for (int i = 0; i < N; i++) {
        cout << "|";

        if (i + 1 < 10) cout << " " << (i + 1) << " ";
        else cout << (i + 1) << " ";

        cout << "| ";

        int brand_len = Length_of(arr[B[i]].brand);
        cout << arr[B[i]].brand;

        for (int k = 0; k < 12 - brand_len; k++) cout << " ";

        cout << "| ";
        cout << arr[B[i]].tail_number;
        for (int k = 0; k < 15 - Length_of(arr[B[i]].tail_number); k++) cout << " ";
        cout << "| ";
        cout << arr[B[i]].flight_number;
        for (int k = 0; k < 12 - Length_of_number(arr[B[i]].flight_number); k++) cout << " ";
        cout << "| ";

        // Время посадки
        if (Length_of_number(arr[B[i]].boarding_hour) == 1) cout << "0";
        cout << arr[B[i]].boarding_hour << ":";
        if (Length_of_number(arr[B[i]].boarding_min) == 1) cout << "0";
        cout << arr[B[i]].boarding_min << "         |" << endl;
    }
    cout << " ----------------------------------------------------------------" << endl;
}

void IndexSort(Aircraft *&arr, int N, int *&B){
    int i, j, t = 0;
    for (i = 0; i < N - 1; i++)
    {
        for (j = i + 1; j < N; ++j)
        {
            if (arr[B[i]].flight_number > arr[B[j]].flight_number)
            {
                t = B[i];
                B[i] = B[j];
                B[j] = t;
            }
        }
    }
}