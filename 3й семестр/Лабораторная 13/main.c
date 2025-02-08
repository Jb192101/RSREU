#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "lab_11.h"
#include "lab_12.h"

#define ERROR_ARRAY printf("Error: array is empty! \n")
#define ERROR_INDEX printf("Error: incorrect index!\n")
#define ERROR_VECTOR printf("Error: Vector is not created!\n")
#define ERROR_MEMORY printf("Error: no memory allocated!\n")

int main() {
	float x, y, z;
	float x2, y2, z2;
	bool isCreated = false;
	int choice = -1;
	int choice1 = -1;
	Vector3D v, v1, v2;
	
	Vehicle *vehicles = NULL;
    int size = 0;
	int command;
	int type;
	int index;
	int f2 = 0;
	unsigned int mask = 0;
	
	while(true)
	{
		printf("\nMenu:\n");
        printf("1. Lab 11: Vector3D\n");
        printf("2. Lab 12: Vehicle\n");
        printf("3. Exit\n");
        printf("Enter choice: ");

        scanf("%d", &choice1);
        
        switch(choice1)
        {
        	case 1:
    			while (choice != 9)
				{
					printf("Whats function u want to testing? \n");
					printf("1. Creating \n");
					printf("2. Adding \n");
					printf("3. Substraction \n");
					printf("4. Degree \n");
					printf("5. Normalization \n");
					printf("6. Production\n");
					printf("7. Projection \n");
					printf("8. Print vector \n");
					printf("9. Exit from loop \n");
					scanf("%d", &choice);
					
					switch(choice)
					{
						case 1:
							printf("Enter values of x, y, z: \n");
							scanf("%f %f %f", &x, &y, &z);
				
							v = createVector(x, y, z);
							printVector(&v);
							printf("\n");
							isCreated = true;
							break;
						case 2:
							if (!isCreated) {
								ERROR_VECTOR;
								continue;
							}
							printf("Enter values of x2, y2, z2: \n");
							scanf("%f %f %f", &x2, &y2, &z2);
							
							v2 = createVector(x2, y2, z2);
							v = addVectors(&v, &v2);
							printVector(&v);
							printf("\n");
							break;
						case 3:
							if (!isCreated) {
								ERROR_VECTOR;
								continue;
							}
							printf("Enter values of x2, y2, z2: \n");
							scanf("%f %f %f", &x2, &y2, &z2);
							
							v2 = createVector(x2, y2, z2);
							v = subVectors(&v, &v2);
							printVector(&v);
							printf("\n");
							break;
						case 4:
							if (!isCreated) {
								ERROR_VECTOR;
								continue;
							}
							printf("Enter values of x2, y2, z2: \n");
							scanf("%f %f %f", &x2, &y2, &z2);
							
							v2 = createVector(x2, y2, z2);
							double degree = degreeVectors(&v, &v2);
							printf("%lf", degree);
							printf("\n");
							break;
						case 5:
							if (!isCreated) {
								ERROR_VECTOR;
								continue;
							}
							v = createVector(x, y, z);
							normVector(&v);
							printVector(&v);
							printf("\n");
							break;
						case 6:
							if (!isCreated) {
								ERROR_VECTOR;
								continue;
							}
							printf("Enter values of x2, y2, z2: \n");
							scanf("%f %f %f", &x2, &y2, &z2);
							
							v2 = createVector(x2, y2, z2);
							v = prodVectors(&v, &v2);
							printVector(&v);
							printf("\n");
							break;
						case 7:
							if (!isCreated) {
								ERROR_VECTOR;
								continue;
							}
							printf("Enter values of x2, y2, z2: \n");
							scanf("%f %f %f", &x2, &y2, &z2);
							
							v2 = createVector(x2, y2, z2);
							v = projectionVector(&v, &v2);
							printVector(&v);
							printf("\n");
							break;
						case 8:
							if (!isCreated) {
								ERROR_VECTOR;
								continue;
							}
							printVector(&v);
							printf("\n");
							break;
						case 9:
							printf("Exit from loop!\n");
							printf("\n");
							return 0;
							break;
						default:
							printf("Error: Value of choice is uncorrect!");
							printf("\n");
							break;
					}
				}
        		break;
        	case 2:
        		while (true)
			    {
			        printf("\nMenu:\n");
			        printf("1. Create vehicle\n");
			        printf("2. Delete vehicle\n");
			        printf("3. Update data about vehicle\n");
			        printf("4. Print info about all vehicles\n");
			        printf("5. Print info about vehicle by mask\n");
			        printf("6. Printf info about specific vehicle\n");
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
			            return 0;
			
			        default:
			            printf("Error: unknown command!\n");
			        }
			    }
        		break;
        	case 3:
        		printf("You succesfully exited the program!");
        		return 0;
        	default:
        		printf("Error: Incorrect number of choice!");
        		break;
		}
	}
	#undef ERROR_ARRAY
    #undef ERROR_MEMORY
    #undef ERROR_INDEX
    #undef ERROR_VECTOR
	
	return 0;
}

