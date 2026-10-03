#include <stdio.h>
#define inventory_size 10

const char *inventory_name[inventory_size] = {
    "пусто",
    "дерево",
    "камень",
    "семена",
    "картошка",
    "кирка",
    "пшено",
    "грибы",
    "седло",
    "морковь"};

int main() // главаная функция
{
  
  // все переменные
  int current_day = 1;
  int current_hour = 8;
  int userAction;
  int work_time;
  int item_slot;
  int item_id;

  
  static int inventory[inventory_size] = {
      0,
      1, 
      2, 
      3, 
      4,
      5, 
      6, 
      7, 
      8, 
      9};  
       
  
  while (1) 
    {
    printf("Выберите пункт:\n");
    printf("[0] Выход\n");
    printf("[1] Посмотреть на часы\n");
    printf("[2] Промотать время (Поработать)\n");
    printf("[3] Посмотреть инвентарь\n");
    printf("[4] Положить предмет в слот\n");
    printf("[5] Выбросить предмет\n");
    printf("[6] Уникальные находки\n");
    if (scanf("%d", &userAction) != 1) 
      {
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
      {
      printf("Текущее время: %d день %d час\n", current_day, current_hour);
      break;
      }
    
    case 2: // поработать
      {
      printf("Сколько работаем?\n");
      scanf("%d", &work_time);
      current_hour += work_time;
      printf("Работаем...\n");
      if (current_hour >= 24) 
        {
        current_day += current_hour / 24;
        current_hour = current_hour % 24;
        }
      break;
      }
    
    case 3: 
      {
      printf("Инвентарь:\n");
      for (int i = 0; i < inventory_size; i++) 
        {
        printf("Слот %d: %s [%d]\n", i,inventory_name[inventory[i]], inventory[i]);
        }
      break;
      }
    
    case 4: 
      {
      printf("Выберите ячейку инвентаря:\n");
      
      if (scanf("%d", &item_slot) != 1) 
        {
        printf("Ошибка ввода! Введите число от 0 до 9\n");
        while (getchar() != '\n'); // чистка буфера
        break;
        }
      
      if (item_slot > 9 || item_slot < 0) 
        {
        printf("Неверный номер слота!Введите значение от 0 до 9\n");
        break;
        } else {
        
        printf("Выберите id предмета:\n");
        
        if (scanf("%d", &item_id) != 1) 
          {
          printf("Ошибка ввода! Введите число от 0 до 9.\n");
          while (getchar() != '\n'); // чистка буфера
          break;
          }
        
        if (item_id > 9 || item_id < 0) 
          {
          printf("Такого предмета нет!Введите id предмета от 0 до 9\n");
          while (getchar() != '\n'); // чистка буфера
          break;
          } else {
          
          inventory[item_slot] = item_id;
          break;
          }
      }
    }
    case 5:
    {
      printf("Выберите ячейку инвентаря:\n");
      
      if (scanf("%d", &item_slot) != 1) 
        {
        printf("Ошибка ввода! Введите число от 0 до 9\n");
        while (getchar() != '\n'); // чистка буфера
        break;
        }
      
      if (item_slot > 9 || item_slot < 0) 
        {
        printf("Неверный номер слота!Введите значение от 0 до 9\n");
        break;
        } else {
          inventory[item_slot] = 0;
          break;
    }
    }

    case 6:
    {
      int unique_items = 0;
      int items[10] = {0};
      for (int i = 0; i < inventory_size; i++) 
        {
        int id = inventory[i];
        if (id >= 0 && id <= 9) 
          {
          items[id]++;
          }
        }
      for (int id = 1; id <= 9; id++) // начинаем с 1, чтобы проигнорировать 0
        {
        if (items[id] > 0) 
          {
          printf("Предмет %s: %d штук\n", inventory_name[id], items[id]);
          unique_items = unique_items + 1;
          }
        }
        printf("Всего уникальных предметов: %d\n", unique_items);
        if (!unique_items) 
          {
          printf("Инвнтарь пуст.\n");
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
