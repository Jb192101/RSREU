#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lab_12.h"

Vehicle* createVehicle(VType t) {
	Vehicle* v = (Vehicle*) malloc(sizeof(Vehicle));
	
	if (v == NULL) {
		printf("Error: no memory allocated!\n");
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

void addVehicle(Vehicle** arr, int* size, Vehicle* v) {
	*arr = (Vehicle*) realloc(*arr, (*size+1) * sizeof(Vehicle));
	
	if(arr == NULL) {
		printf("Error: no memory allocated!\n");
		exit(1);
	}
	
	(*arr)[*size] = *v;
	(*size)++;
}

void deleteVehicle(Vehicle** arr, int* size, const int ind) {
	int i = ind;
	for(i; i < *size - 1; i++) {
		(*arr)[i] = (*arr)[i + 1];
	}
	
	*arr = (Vehicle*) realloc(*arr, (*size - 1) * sizeof(Vehicle));
    (*size)--;
}

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

void infoAllVehicles(Vehicle* arr, const int size) {
	printf("All vehicles: \n");
	int i = 0;
	for(i; i < size; i++) {
		infoCertainVehicle(arr + i);
		printf("\n");
	}
}

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

