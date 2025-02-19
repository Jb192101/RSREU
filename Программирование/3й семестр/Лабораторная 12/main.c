#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ERROR_ARRAY printf("Error: array is empty!\n")
#define ERROR_MEMORY printf("Error: no memory allocated!\n")
#define ERROR_INDEX printf("Error: incorrect index!\n")

typedef enum {
    FLAG1 = 1,		//0x01 		// 00001
    FLAG2 = 2,		//0x02 		// 00010
    FLAG3 = 4,		//0x04 		// 00100
    FLAG4 = 8,		//0x08 		// 01000
    FLAG5 = 16	    //0x16 		// 10000
} Flags;

// Типы транспортного средства
typedef enum {
	CAR_ENUM,
	MOTOCYCLE_ENUM,
	TRUCK_ENUM
} VType;

// Структура для CAR_ENUM
struct CAR {
	int modelYear;
	char maker[50];
};

// Структура для MOTOCYCLE_ENUM
struct MOTOCYCLE {
	int cargoCapacity;
    int maxWeight;
};

// Структура для TRUCK_ENUM
struct TRUCK {
	int countOfWheels;
    int maxSpeed;
};

// Объединения для данных транспортных средств
union VData {
	struct CAR car;
	struct MOTOCYCLE motocycle;
	struct TRUCK truck;
};

// Структура транспортного средства
typedef struct {
    VType type;
    union VData data;
    unsigned int flags : 5;
} Vehicle;

// Создание структуры
Vehicle* createVehicle(VType t) {
	Vehicle* v = (Vehicle*) malloc(sizeof(Vehicle));
	
	if (v == NULL) {
		ERROR_MEMORY;
		exit(1);
	}
	
	v->type = t;
	v->flags = 0;
	
	printf("Enter data about vehicle: \n");
    switch (t) {
	    case CAR_ENUM:
	    	do {
		        printf("Year of produce: ");
		        scanf("%d", &v->data.car.modelYear);
		        printf("Maker (no more 50 chars): ");
		        scanf("%s", &v->data.car.maker);
	    	} while(v->data.car.modelYear <= 0);
	        break;
	
	    case MOTOCYCLE_ENUM:
	    	do {
		        printf("Cargo capacity: ");
		        scanf("%d", &v->data.motocycle.cargoCapacity);
		        printf("Max weight: ");
		        scanf("%d", &v->data.motocycle.maxWeight);
		    } while(v->data.motocycle.cargoCapacity <= 0 || v->data.motocycle.maxWeight <= 0);
	        break;
	
	    case TRUCK_ENUM:
	    	do {
		        printf("Count of wheels: ");
		        scanf("%d", &v->data.truck.countOfWheels);
		        printf("Max speed: ");
		        scanf("%d", &v->data.truck.maxSpeed);
	    	} while(v->data.truck.countOfWheels <= 0 || v->data.truck.maxSpeed <= 0);
	        break;
	
	    default:
	        printf("Error: Unknown type of vehicle. \n");
	        free(v);
	        return NULL;
    };
    
    // Работа с флагами
    int f = false;
    unsigned int mask = 0;
    
    printf("Enter features of vehicle: \n");
    
    printf("Is refueled? (0 - No, 1 - Yes): ");
    scanf("%d", &f);
    mask = f ? mask|FLAG5 : mask;
    
    printf("Is used? (0 - No, 1 - Yes): ");
    scanf("%d", &f);
    mask = f ? mask|FLAG4 : mask;
    
    printf("Is arended? (0 - No, 1 - Yes): ");
    scanf("%d", &f);
    mask = f ? mask|FLAG3 : mask;
    
    printf("Require a repair? (0 - No, 1 - Yes): ");
    scanf("%d", &f);
    mask = f ? mask|FLAG2 : mask;
    
    printf("In the moving now? (0 - No, 1 - Yes): ");
    scanf("%d", &f);
    mask = f ? mask|FLAG1 : mask;
    
    v->flags = mask;
    
    return v;
}

// Добавление нового транспортного средства в массив
void addVehicle(Vehicle** arr, int* size, Vehicle* v) {
	*arr = (Vehicle*) realloc(*arr, (*size+1) * sizeof(Vehicle));
	
	if(arr == NULL) {
		ERROR_MEMORY;
		exit(1);
	}
	
	(*arr)[*size] = *v;
	(*size)++;
}

// Удаление транспортного средства
void deleteVehicle(Vehicle** arr, int* size, const int ind) {
	int i = ind;
	for(i; i < *size - 1; i++) {
		(*arr)[i] = (*arr)[i + 1];
	}
	
	*arr = (Vehicle*) realloc(*arr, (*size - 1) * sizeof(Vehicle));
    (*size)--;
}

// Вывод информации о конкретном транспортном средстве
void infoCertainVehicle(const Vehicle* v) {
	VType t = v->type;
	char* type_string = NULL;
	switch (t)
    {
    case CAR_ENUM:
        type_string = "Car";
        break;

    case MOTOCYCLE_ENUM:
        type_string = "Motocycle";
        break;

    case TRUCK_ENUM:
        type_string = "Truck";
        break;

    default:
        type_string = "Undefined vehicle";
        return;
    };
    
	printf("Type of vehicle: %s\n", type_string);
	
	// Вывод основных св-в
	switch(t) {
		case CAR_ENUM:
		    printf("Year of produce: %d\n", v->data.car.modelYear);
		    printf("Maker: %s\n", v->data.car.maker);
	        break;
	
	    case MOTOCYCLE_ENUM:
		    printf("Cargo capacity: %d\n", v->data.motocycle.cargoCapacity);
		    printf("Max weight: %d\n", v->data.motocycle.maxWeight);
	        break;
	
	    case TRUCK_ENUM:
		    printf("Count of wheels: %d\n", v->data.truck.countOfWheels);
		    printf("Max speed: %d\n", v->data.truck.maxSpeed);
	        break;
	
	    default:
	        printf("Error: Unknown type of vehicle. \n");
	    	return;
	}
	printf("\n");
	
	// Вывод флагов
	printf("Specific features of vehicle. \n");
    if (v->flags & FLAG5)
        printf("Refueled.\n");
    if (v->flags & FLAG4)
        printf("Used.\n");
    if (v->flags & FLAG3)
        printf("Arended.\n");
    if (v->flags & FLAG2)
        printf("Require a repair.\n");
    if (v->flags & FLAG1)
        printf("At moving.\n");
}

// Вывод информации обо всех транспортных средствах
void infoAllVehicles(Vehicle* arr, const int size) {
	printf("All vehicles: \n");
	int i = 0;
	for(i; i < size; i++) {
		infoCertainVehicle(arr + i);
		printf("\n");
	}
}

// Обновление данных транспортного средства
void updateVehicle(Vehicle* arr, const int size, const int index) {
	Vehicle* cur_v = &arr[index];
	
	printf("New data for vehicle: \n");
    switch (cur_v->type) {
	    case CAR_ENUM:
		    	do {
			        printf("Year of produce: ");
			        scanf("%d", &cur_v->data.car.modelYear);
			        printf("Maker: ");
			        scanf("%s", &cur_v->data.car.maker);
		    	} while(cur_v->data.car.modelYear <= 0);
		        break;
		
		    case MOTOCYCLE_ENUM:
		    	do {
			        printf("Cargo capacity: ");
			        scanf("%d", &cur_v->data.motocycle.cargoCapacity);
			        printf("Max weight: ");
			        scanf("%d", &cur_v->data.motocycle.maxWeight);
			    } while(cur_v->data.motocycle.cargoCapacity <= 0 || cur_v->data.motocycle.maxWeight <= 0);
		        break;
		
		    case TRUCK_ENUM:
		    	do {
			        printf("Count of wheels: ");
			        scanf("%d", &cur_v->data.truck.countOfWheels);
			        printf("Max speed: ");
			        scanf("%d", &cur_v->data.truck.maxSpeed);
		    	} while(cur_v->data.truck.countOfWheels <= 0 || cur_v->data.truck.maxSpeed <= 0);
		        break;
		
		    default:
		        printf("Error: Unknown type of vehicle. \n");
        return;
    };
    
    // Работа с флагами
    int f = false;
    unsigned int mask = 0;
    
    printf("Enter features of vehicle: \n");
    
    printf("Is refueled? (0 - No, 1 - Yes): ");
    scanf("%d", &f);
    mask = f ? mask|FLAG5 : mask;
    
    printf("Is used? (0 - No, 1 - Yes): ");
    scanf("%d", &f);
    mask = f ? mask|FLAG4 : mask;
    
    printf("Is arended? (0 - No, 1 - Yes): ");
    scanf("%d", &f);
    mask = f ? mask|FLAG3 : mask;
    
    printf("Require a repair? (0 - No, 1 - Yes): ");
    scanf("%d", &f);
    mask = f ? mask|FLAG2 : mask;
    
    printf("In the moving now? (0 - No, 1 - Yes): ");
    scanf("%d", &f);
    mask = f ? mask|FLAG1 : mask;
    
    cur_v->flags = mask;
}

// Вывод транспортного средства по маске
void infoVehicleMask(Vehicle* arr, const int size, unsigned int mask) {
	int i = 0;
    for (i; i < size; i++)
    {
        if (arr[i].flags & mask)
        {
            printf("Number of vehicle: %d\n", i + 1);
        	infoCertainVehicle(&arr[i]);
        	printf("\n\n");
        }
    }
}

int main() {
	Vehicle *vehicles = NULL;
    int size = 0;
	int command;
	int type;
	int index;
	int f2 = 0;
	unsigned int mask = 0;

    while (true)
    {
        printf("\nMenu:\n");
        printf("1. Create vehicle\n");
        printf("2. Delete vehicle\n");
        printf("3. Update data about vehicle\n");
        printf("4. Print info about all vehicles\n");
        printf("5. Print info about vehicle by mask\n");
        printf("6. Print info about specific vehicle\n");
        printf("7. Exit program.\n");
        printf("Choice a command: ");

        scanf("%d", &command);

        switch (command)
        {
        case 1:
            printf("Enter type of vehicle \n");
            printf("0 - Car\n");
            printf("1 - Motocycle\n");
            printf("2 - Truck\n");
            printf("Your choice: ");
            scanf("%d", &type);
            VType new_v_t = (VType)type;
            Vehicle *new_v = createVehicle(new_v_t);
            if (!new_v) {
            	ERROR_MEMORY;
                break;
			}         
			   
            addVehicle(&vehicles, &size, new_v);
            break;

        case 2:
            if (size == 0)
            {
                ERROR_ARRAY;
                break;
            }

            printf("Enter number of element for deleting: ");
            scanf("%d", &index);
            
            if (index < 1 || index > size) {
            	printf("Error: incorrect index!\n");
            	break;
			}
            
            deleteVehicle(&vehicles, &size, index - 1);
            break;

        case 3:
            if (size == 0)
            {
                ERROR_ARRAY;
                break;
            }

            printf("Enter index of element for updating: ");
            scanf("%d", &index);
            
            if (index < 1 || index > size) {
            	ERROR_INDEX;
            	break;
			}
			
            updateVehicle(vehicles, size, index - 1);
            break;

        case 4:
            if (size == 0)
            {
                ERROR_ARRAY;
                break;
            }
            infoAllVehicles(vehicles, size);
            break;
        
        case 5:
		    printf("Enter features of vehicle: \n");
		    
		    printf("Is refueled? (0 - No, 1 - Yes): ");
		    scanf("%d", &f2);
		    mask = f2 ? mask|FLAG5 : mask;
		    
		    printf("Is used? (0 - No, 1 - Yes): ");
		    scanf("%d", &f2);
		    mask = f2 ? mask|FLAG4 : mask;
		    
		    printf("Is arended? (0 - No, 1 - Yes): ");
		    scanf("%d", &f2);
		    mask = f2 ? mask|FLAG3 : mask;
		    
		    printf("Require a repair? (0 - No, 1 - Yes): ");
		    scanf("%d", &f2);
		    mask = f2 ? mask|FLAG2 : mask;
		    
		    printf("In the moving now? (0 - No, 1 - Yes): ");
		    scanf("%d", &f2);
		    mask = f2 ? mask|FLAG1 : mask;
		    
		    infoVehicleMask(vehicles, size, mask);
			break;

        case 6:
            if (size == 0)
            {
                ERROR_ARRAY;
                break;
            }
            
            printf("Enter index of element for updating: ");
            scanf("%d", &index);
            
            if (index < 1 || index > size) {
            	ERROR_INDEX;
            	break;
			}

            infoCertainVehicle(vehicles+index-1);
            break;
        
        case 7:
            printf("Exit from program is success!\n");
            free(vehicles);
            vehicles = NULL;
            return 0;

        default:
            printf("Error: unknown command!\n");
        }
    }
    #undef ERROR_ARRAY
    #undef ERROR_MEMORY
    #undef ERROR_INDEX
	
	return 0;
}
