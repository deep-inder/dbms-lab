#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "contact.h"

void somedemo(void(*f)(struct Contact *), struct Contact *c){
    f(c);
}

void print1 (struct Contact *c){
    printf("%d\n", c->contact_id);
}

void print2(struct Contact *c){
    printf("%s\n", c->contact_name);
}

int main(){
    struct Contact demo;
    demo.contact_id = 100;
    strcpy(demo.contact_name, "Adele");
    somedemo(print1, &demo);
    somedemo(print2, &demo);
    return 0;
}