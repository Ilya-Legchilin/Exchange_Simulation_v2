#include <stdio.h>
#include <stdlib.h>


typedef enum {
    B,
    S
} Side_TypeDef;


typedef struct {
    int ID;
    Side_TypeDef side;
    int Qty;
    double Price;
} Order_TypeDef;


typedef struct list
{
    Order_TypeDef field;  // поле данных
    struct list *next;    // указатель на следующий элемент
    struct list *prev;    // указатель на предыдущий элемент
} list;


list *lst;
int list_counter = 0;
int trade_counter = 0;


FILE * fpinput;
FILE * fpoutput;


void swap_orders(Order_TypeDef * a, Order_TypeDef * b);
void trade(Order_TypeDef * buy_order, Order_TypeDef * sell_order);
void deletetail(void);
void deletehead(void);
void trade(Order_TypeDef * buy_order, Order_TypeDef * sell_order);
void trade_show(int buy_id, int sell_id, int quantity, double price);
void cancel(int order_id);
int string_to_order(char * buffer, Order_TypeDef * order);
int delete_via_id(int id);


void init_list(Order_TypeDef a)  // а- значение первого узла
{
    if (lst == NULL) {
        lst = (list *)malloc(sizeof(list));
    }
    lst->field = a;   // выделение памяти под корень списка
    lst->next = NULL; // указатель на следующий узел
    lst->prev = NULL; // указатель на предыдущий узел
    list_counter = 1;
}


void sort(void)
{
    list * temp = lst;
    for (int i = 0; i < list_counter - 1; i++) {
        if (temp->field.side == B && temp->prev->field.side == B) {
            if (temp->field.Price > temp->prev->field.Price)
                swap_orders(&(temp->field), &(temp->prev->field));
            else if (temp->field.Price == temp->prev->field.Price)
                if (temp->field.ID < temp->prev->field.ID)
                    swap_orders(&(temp->field), &(temp->prev->field));
        }
        if (temp->field.side == S && temp->prev->field.side == B)
            swap_orders(&(temp->field), &(temp->prev->field));
        if (temp->field.side == S && temp->prev->field.side == S) {
            if (temp->field.Price < temp->prev->field.Price)
                swap_orders(&(temp->field), &(temp->prev->field));
            else if (temp->field.Price == temp->prev->field.Price)
                if (temp->field.ID < temp->prev->field.ID)
                    swap_orders(&(temp->field), &(temp->prev->field));
        }
        temp = temp->prev;
    }
    while (temp->next != NULL)
        temp = temp->next;
    lst = temp;
}


list * take_from_list(int id)
{
    list * temp = (list*)malloc(sizeof(list));
    temp = lst;
    while (1){
        if (temp->field.ID == id)
            return temp;
        temp = temp->prev;
    }
}


int addelem_to_list(Order_TypeDef order)
{
    if (lst != NULL) {
        list_counter++;
        list *temp, *p;
        temp = (list *) malloc(sizeof(list));
        list * current_element = (list *) malloc(sizeof(list));
        p = (list *) malloc(sizeof(list));
        p = lst->next;                            // storing a pointer to the next node
        lst->next = temp;                         // the previous node points to the newly created
        temp->field = order;                      // saving the data field of the added node
        temp->next = p;                           // the created node points to the next node
        temp->prev = lst;                         // the created node points to the previous node
        int i = 0;
        if (p != NULL) {
            i++;
            p->prev = temp;
        }
        lst = temp;
        int id = temp->field.ID;
        sort();
        current_element = take_from_list(id);
        temp = lst;
        while (temp->prev != NULL)
            temp = temp->prev;
        if (current_element->field.side == B){
            while (temp != NULL && temp->field.side == S){
                if ((temp->field.ID < current_element->field.ID) && (temp->field.Price <= current_element->field.Price))
                    trade(&(temp->field), &(current_element->field));
                else if (temp->field.ID > current_element->field.ID)
                    trade(&(temp->field), &(current_element->field));
                if (temp->field.Qty == 0) {
                    delete_via_id(temp->field.ID);
                    //return 0;
                }
                if (current_element->field.Qty == 0){
                    delete_via_id(current_element->field.ID);
                    return 0;
                }
                temp = temp->next;
            }
        } else {
            while (temp != NULL && temp->field.side != B)
                temp = temp->next;
            while (temp != NULL && temp->field.side == B){
                if ((temp->field.ID < current_element->field.ID) && (temp->field.Price >= current_element->field.Price))
                    trade(&(temp->field), &(current_element->field));
                else if (temp->field.ID > current_element->field.ID)
                    trade(&(temp->field), &(current_element->field));
                if (temp->field.Qty == 0) {
                    delete_via_id(temp->field.ID);
                    //return 0;
                }
                if (current_element->field.Qty == 0) {
                    delete_via_id(current_element->field.ID);
                    return 0;
                }
                temp = temp->next;
            }
        }
    } else
        init_list(order);
    return 0;
}

void swap_orders(Order_TypeDef * a, Order_TypeDef * b){
    Order_TypeDef temp;
    temp.ID = a->ID;
    temp.Price = a->Price;
    temp.Qty = a->Qty;
    temp.side = a->side;
    a->ID = b->ID;
    a->Price = b->Price;
    a->Qty = b->Qty;
    a->side = b->side;
    b->ID = temp.ID;
    b->Price = temp.Price;
    b->Qty = temp.Qty;
    b->side = temp.side;
}


void deletetail(void)
{
    list_counter--;
    list * tail = lst;
    while (tail->next != NULL)
        tail = tail->next;
    lst = tail->prev;
    if (lst != NULL) {
        free(lst->next);
        lst->next = NULL;
    }
}


void deletehead(void)
{
    list_counter--;
    list * root = lst;
    while (root->prev != NULL)
        root = root->prev;
    list *temp;
    temp = root->next;
    if (list_counter != 0){
        temp->prev = NULL;
        free(root);   // free memory of current root
    }
    while (temp->next != NULL)
        temp = temp->next;
    lst = temp;
}


int delete_via_id(int id)
{
    list * temp = lst;
    while (temp->field.ID != id && temp->prev != NULL)
        temp = temp->prev;
    if (temp->field.ID == id) {
        if (temp->next == NULL) {
            deletetail();
            return 0;
        }
        if (temp->prev == NULL) {
            deletehead();
            return 0;
        } else {
            list_counter--;
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;
            free(temp);
        }
    }
    return 0;
}


int min(int a, int b){
    if (a < b)
        return a;
    else
        return b;
}


int max(int a, int b){
    if  (a > b)
        return a;
    else
        return b;
}


void trade(Order_TypeDef * buy_order, Order_TypeDef * sell_order){
    if (buy_order->side == S) {
        Order_TypeDef * temp = buy_order;
        buy_order = sell_order;
        sell_order = temp;
    }
    int minimum = min(buy_order->Qty, sell_order->Qty);
    if (buy_order->ID < sell_order->ID)
        trade_show(buy_order->ID, sell_order->ID, minimum, buy_order->Price);
    else
        trade_show(buy_order->ID, sell_order->ID, minimum, sell_order->Price);
    sell_order->Qty = sell_order->Qty - minimum;
    buy_order->Qty = buy_order->Qty - minimum;
}



void trade_show(int buy_id, int sell_id, int quantity, double price){
    trade_counter++;
    char k;
    if (buy_id < sell_id)
        k = 'B';
    else
        k = 'S';
    if (((int)(price*100) % 100) == 0)
        fprintf(fpoutput, "T,%d,%c,%d,%d,%d,%.1f\n", trade_counter, k, min(buy_id, sell_id), max(buy_id, sell_id), quantity, price);
    else
        fprintf(fpoutput, "T,%d,%c,%d,%d,%d,%g\n", trade_counter, k, min(buy_id, sell_id), max(buy_id, sell_id), quantity, price);
}


void cancel(int order_id){
    fprintf(fpoutput, "X,%d\n", order_id);
}


int string_to_order(char * buffer, Order_TypeDef * order){
    int i = 0;
    int id = 0;
    if (buffer[0] == 'C'){
        i = 2;
        while (buffer[i] != '\0')   {
            id = id*10 + buffer[i] - '0';
            i++;
        }
        return id;
    }
    Side_TypeDef side;
    int quantity = 0;
    char price_buffer[32];
    double price = 0;
    int counter_of_commas = 0;
    int j = 0;
    while (buffer[i] != '\0'){
        if (buffer[i] == ','){
            counter_of_commas++;
            i++;
            continue;
        }
        if (counter_of_commas == 1)
            id = id*10 + buffer[i] - '0';
        if (counter_of_commas == 2)
            if (buffer[i] == 'B')
                side = B;
            else
                side = S;
        if (counter_of_commas == 3)
            quantity = quantity*10 + buffer[i] - '0';
        if (counter_of_commas == 4)
            price_buffer[j++] = buffer[i];
        i++;
    }
    price = atof(price_buffer);
    order->ID = id;
    order->Price = price;
    order->Qty = quantity;
    order->side = side;
    return 0;
}


void listprintr(void)
{
    list *p;
    p = lst;
    if (lst == NULL)
        printf("LIST IS EMPTY\n");
    else {
        while (p->prev != NULL) {
            p = p->prev;
        }
        while (p != NULL) {
            char c;
            if (p->field.side == B)
                c = 'B';
            else
                c = 'S';
            printf("O,%d,%c,%d,%.2f\n", p->field.ID, c, p->field.Qty, p->field.Price);
            p = p->next;
        }
    }
}

int is_in_list(int id)
{
    list * temp = lst;
    int found = 0;
    while (temp != NULL) {
        if (temp->field.ID == id)
            found = 1;
        temp = temp->prev;
    }
    return found;
}


int main()
{
    fpinput = fopen("input.txt", "r");
    fpoutput = fopen("output.txt", "w");
    lst = (list*)malloc(sizeof(list));
    char line[64];
    fscanf(fpinput, "%s", line);
    Order_TypeDef * order = (Order_TypeDef *)malloc(sizeof(Order_TypeDef));
    string_to_order(line, order);
    init_list(*order);
    int i = 0;
    int id = 0;
    while (!feof(fpinput)) {
        fscanf(fpinput, "%s", line);
        id = string_to_order(line, order);
        if (id == 0)
            addelem_to_list(*order);
        else{
            if (is_in_list(id)) {
                cancel(id);
                delete_via_id(id);
            }
        }
        i++;
    }
    return 0;
}
