#include <stdio.h>
#define INVENTORY_SIZE 10
int main() 
{   
    int slot, item_id;
    int work_hours;
    int choice; 
    int current_day = 1;
    int current_hour = 8;
    int inventory[INVENTORY_SIZE] = {
        0, //пусто
        1, //дерево
        2, //камень
        3, //семена
        4, //вилы
        5, //лопата
        6, //мотыга
        7, //топор
        8, //ведро
        9  //сено
    };

    while (1) {
        printf("[0] Выход\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Промотать время (Поработать)\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Инверсия карманов\n");
        printf("Выбор: ");

        scanf("%d", &choice);

        switch (choice) {
            
            case 0:
                printf("\nВыход.\n");
                return 0; 
            
            case 1:
                printf("\nВремя: День %d, %02d:00\n", current_day, current_hour);
                break;

            case 2: {
                printf("\nСколько часов работать? ");
                scanf("%d", &work_hours);

                if (work_hours < 0) {
                    printf("[Ошибка]: Неправильный ввод!\n");
                    break;
                }
                current_hour = current_hour + work_hours;

                if (current_hour >= 24) {
                    current_day = current_day + (current_hour / 24); 
                    current_hour = current_hour % 24;                
                }

                printf("Вы поработали.\n");
                break;
            }

            case 3:
                printf("\n--- РЮКЗАК: ---\n");
                for (int i = 0; i < INVENTORY_SIZE; i++) {
                    printf("Слот %d: [%d] ", i, inventory[i]);

                    switch (inventory[i]) {
                        case 0: printf("(Пусто)\n"); break;
                        case 1: printf("(Дерево)\n"); break;
                        case 2: printf("(Камень)\n"); break;
                        case 3: printf("(Семена)\n"); break;
                        case 4: printf("(Вилы)\n"); break;
                        case 5: printf("(Лопата)\n"); break;
                        case 6: printf("(Мотыга)\n"); break;
                        case 7: printf("(Топор)\n"); break;
                        case 8: printf("(Ведро)\n"); break;
                        case 9: printf("(Сено)\n"); break;
                        default: printf("(Неизвестный предмет)\n"); break;
                    }
                }
                printf("---------------------------\n");
                break;

            case 4: {
                printf("\nВведите индекс слота (от 0 до 9): ");
                scanf("%d", &slot);
                printf("Введите ID предмета: ");
                scanf("%d", &item_id);

                if (slot < 0 || slot >= INVENTORY_SIZE) {
                    printf("[Ошибка]: Слот %d не существует! Вводите от 0 до %d.\n", slot, INVENTORY_SIZE - 1);
                } else {
                    inventory[slot] = item_id; // Записываем ID в массив
                    printf("Предмет с ID %d успешно положен в слот %d.\n", item_id, slot);
                }
                break;
            }

            case 5: {
                printf("\nВведите индекс слота для очистки (от 0 до 9): ");
                scanf("%d", &slot);

                if (slot < 0 || slot >= INVENTORY_SIZE) {
                    printf("[Ошибка]: Слот %d не существует!\n", slot);
                } else {
                    inventory[slot] = 0; // Обнуляем ячейку (по ТЗ 0 — это пусто)
                    printf("Слот %d успешно очищен.\n", slot);
                }
                break;
            }
            case 6: {
                for (int i = 0; i < INVENTORY_SIZE; i++) {
                    printf("[%d] ", inventory[i]);
                }
                printf("\n");
                break;
            }

            default:
                printf("\n[Ошибка]: Неверный пункт меню! Введите число от 0 до 5.\n");
                break;
        }
    }

    return 0;
}  
