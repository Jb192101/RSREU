// Персоны
#include "Person.h"
#include "Minister.h"
#include "Parlamentman.h"
#include "MemberOfMonarchFamily.h"

// Государства
#include "State.h"
#include "Monarchy.h"
#include "PresidentRepublic.h"
#include "Republic.h"
#include "Confederation.h"

// Остальное
#include "World.h"

int main() {
    Parlamentman parm = Parlamentman(50, "Шиз", speaker, RadicalLeft);
    parm.getAge();
}