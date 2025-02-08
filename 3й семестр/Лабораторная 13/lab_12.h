#ifndef LAB12_HEADER
#define LAB12_HEADER

typedef enum {
    FLAG1 = 1,		//0x01 		// 00001
    FLAG2 = 2,		//0x02 		// 00010
    FLAG3 = 4,		//0x04 		// 00100
    FLAG4 = 8,		//0x08 		// 01000
    FLAG5 = 16	    //0x16 		// 10000
} Flags;

typedef enum {
	CAR_ENUM,
	MOTOCYCLE_ENUM,
	TRUCK_ENUM
} VType;

struct CAR {
	int modelYear;
	char maker[50];
};

struct MOTOCYCLE {
	int cargoCapacity;
    int maxWeight;
};

struct TRUCK {
	int countOfWheels;
    int maxSpeed;
};

union VData {
	struct CAR car;
	struct MOTOCYCLE motocycle;
	struct TRUCK truck;
};

typedef struct {
    VType type;
    union VData data;
    unsigned int flags : 5;
} Vehicle;

Vehicle* createVehicle(VType t);

void addVehicle(Vehicle** arr, int* size, Vehicle* v);

void deleteVehicle(Vehicle** arr, int* size, const int ind);

void infoCertainVehicle(const Vehicle* v);

void infoAllVehicles(Vehicle* arr, const int size);

void updateVehicle(Vehicle* arr, const int size, const int index);

void infoVehicleMask(Vehicle* arr, const int size, unsigned int mask);

#endif

