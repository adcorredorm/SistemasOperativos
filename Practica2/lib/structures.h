#ifndef STRUCTURES_H
#define STRUCTURES_H

#define PORT 3535

typedef struct{
    int id;
    char nombre[32];
    char tipo[32];
    int edad;
    char raza[16];
    int estatura;
    float peso;
    char sexo; // H/M
} dogType;

dogType* crear_registro();
void imprimirMascota(void *p);

#endif
