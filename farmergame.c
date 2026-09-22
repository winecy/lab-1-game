#include <stdio.h>
#define inventory_size 10

int main() //главаная функция
{
    //все переменные 
    int current_day = 1;
    int current_hour = 8;
    int userAction;
    int work_hours;

    static int inventory[inventory_size] = {0,1,2,3,4,5,6,7,8,9}; //создаем инвентарь(массив) с предметами(цифры я взял как id, для отличия предметов, ведь массив это нолики)
    
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
    
    printf("Выберите пункт: ");
    scanf("%d",&userAction);
    
    switch (userAction) //действия в зависимости от выбора
    {
        case 0://завершение работы проги
        {
            printf("Пока-пока!");
	        break;
        }
        case 1://вывести время
	        printf("Текущее время: %d день %d час",current_day,current_hour);
	        break;
    
        case 2://поработать(пока просто текст)
        {
	        printf("Работаем");
            break;
        }
        default://если ввод неправильный
        {
            printf("Неверный ввод");
            break;

        }
        
    }


}