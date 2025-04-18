#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}

void insertMap(HashMap * map, char * key, void * value) {
    //Funcion Hash y da pos
    long pos = hash(key, map->capacity);
    if(((float)map->size/map->capacity) >= 0.7) //deve ser al menos 1 float
        enlarge(map);
    long posInicial = pos;

    //Mejor estructura que la que tenia creo
    while(map->buckets[pos] != NULL)
    {
        if(map->buckets[pos]->key != NULL && strcmp(map->buckets[pos]->key, key) == 0)
            return;

        //Se suma 1 para ir buscando otra pos donde poder insertar
        pos = (pos + 1) % map->capacity;
        if (pos == posInicial) return; //Significa que volvio a incial el ciclo

    }

    //Creamos tupla y actualizamos el mapa
    Pair * nuevaTupla = createPair(key, value);
    map->buckets[pos] = nuevaTupla;
    map->current = pos;
    map->size++;
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    Pair ** antiguoArreglo = map->buckets; //es el arreglo en si
    long capacidadAntigua = map->capacity;

    //Duplicamos la capacidad y redimencionamos el arreglo (map->bucket)
    map->capacity *=2;
    map->buckets = (Pair**) malloc(map->capacity * sizeof(Pair*)); //Preguntar porque el realloc me daba error

    if(map->buckets == NULL) //No c pudo redimencionar el arreglo
    {
        map->buckets = antiguoArreglo;
        map->capacity = capacidadAntigua;
        return;
    }

    //Inicializamos todo de nuevo
    for(int i = 0 ; i < map->capacity ; i++)
    {
        map->buckets[i] = NULL;
    }
    map->size = 0;

    for(int k = 0 ; k < capacidadAntigua ; k++)
    {
        if((antiguoArreglo[k] != NULL) && (antiguoArreglo[k]->key != NULL))
        insertMap(map, antiguoArreglo[k]->key, antiguoArreglo[k]->value);
    }

    free(antiguoArreglo);
}

HashMap * createMap(long capacity) {

    //Creo y reservo memoria para el mapa en sí
    HashMap * mapa = (HashMap*) malloc(sizeof(HashMap));
    if(mapa == NULL) return NULL; 

    //Luego reservo memoria para cada bucket // Pair como es doble puntero se pone como un puntero simple aquí
    mapa->buckets = (Pair**) malloc(sizeof(Pair*) * capacity); //resevamos para toda la capacidad;
    if(mapa->buckets == NULL)
    {
        free(mapa); //liberamos la memoria del mapa si no se pudo resevar memoria para los buckets
        return NULL;
    }

    //El readme dice que hay que inicializar los buckets en NULL
    for(long i = 0 ; i < capacity ; i++)
    {
        mapa->buckets[i] = NULL;
    }

    //Inicializamos los demás campos
    mapa->capacity = capacity;
    mapa->size = 0;
    mapa->current = -1;

    return mapa;
}

void eraseMap(HashMap * map,  char * key) {   
    //Como no se me ocurrio antes u_u
    Pair * parBuscar = searchMap(map, key);

    if(parBuscar != NULL)
    {
        parBuscar->key = NULL;
        map->size--;
    }

}

Pair * searchMap(HashMap * map,  char * key) {   

    long posiblePos = hash(key, map->capacity);
    long posInicial = posiblePos;

    while(map->buckets[posiblePos] != NULL)
    { 
        if(map->buckets[posiblePos]->key != NULL && (strcmp(map->buckets[posiblePos]->key, key) == 0))
        {   
            map->current = posiblePos;
            return map->buckets[posiblePos];
        }

        posiblePos = (posiblePos + 1) % map->capacity;
        
        if(posiblePos == posInicial) return NULL;
    }

    return NULL;
}

Pair * firstMap(HashMap * map) {

    for(long k = 0 ; k < map->capacity ; k++)
    {
        if((map->buckets[k] != NULL) && (map->buckets[k]->key != NULL))
        {   
            map->current = k; //se actualiza la pos
            return map->buckets[k];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {

    long * actual = (map->current) + 1; //Mejor inicializar en 1 más de inmediato para evitar errores

    for(long k = actual ; k < map->capacity ; k++)
    {
        if((map->buckets[k] != NULL) && (map->buckets[k]->key != NULL))
        {
            map->current = k ; //actualizamos el current
            return map->buckets[k];
        }
    }

    return NULL;
}
