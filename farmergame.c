#include <stdio.h>
#define inventory_size 10

int main() // главаная функция
{
  // все переменные
  int current_day = 1;
  int current_hour = 8;
  int userAction;
  int work_hours;
  int work_time;

  static int inventory[inventory_size] = {
      0, 1, 2, 3, 4,
      5, 6, 7, 8, 9}; // создаем инвентарь(массив) с предметами(цифры я взял как
                                          // id, для отличия предметов, ведь массив это нолики)
  /*0 – пусто,
  1 – дерево,
  2 – камень,
  3 – семена,
  4 – картошка,
  5 – кирка,
  6 – пшено,
  7 – грибы,
  8 – седло,
  9 – морковь*/
  while (1) {
    printf("Выберите пункт:\n");
    if (scanf("%d", &userAction) != 1) {
        printf("Ошибка ввода! Введите число.\n");
        while (getchar() != '\n'); // чистка буфера 
        continue;
    }
    switch (userAction) // действия в зависимости от выбора
    {
    case 0: // завершение работы проги
    {
      printf("Пока-пока!\n");
      return 0;
      break;
    }
    case 1: // вывести время
      printf("Текущее время: %d день %d час\n", current_day, current_hour);
      break;

    case 2: // поработать
      printf("Сколько работаем?\n");
      scanf("%d", &work_time);
      current_hour += work_time;
      printf("Работаем...\n");
      if (current_hour >= 24) {
        current_day += current_hour / 24;
        current_hour = current_hour % 24;
      }
      break;
    
    case 3: {
        printf("Инвентарь:\n");
        for (int i = 0; i < inventory_size; i++) {
        printf("Слот %d: [%d]\n", i, inventory[i]);
        }
        break;
    }
    default: // если ввод неправильный
    {
        printf("Неверный ввод\n");
        break;
    }
    }
  }
}