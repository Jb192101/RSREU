class Airline:
    def __init__(self):
        print("Авиакомпания создана")
        self.name = "Airline"
        self.count_of_airplanes = 300
        self.count_of_staff = 50000

    def getInfo(self):
        print(f"Название авиакомпании: {self.name}")
        print(f"Кол-во самолётов во владении компании: {self.count_of_airplanes}")
        print(f"Кол-во сотрудников компании: {self.count_of_staff}")

    def changeName(self, newName):
        self.name = newName

    def changeCoA(self, newCoA):
        self.count_of_airplanes = newCoA

    def changeCoS(self, newCoS):
        self.count_of_staff = newCoS

    def CompanyIsExist(self):
        print("Авиакомпания существует")

# Основная программа
Company1 = Airline()
Company2 = Airline()
Company3 = Airline()
Company4 = Airline()
Company5 = Airline()

Company1.getInfo()

Company2.changeName("Аэрофлот")
print(Company2.name)

Company3.changeCoA(250)
print(Company3.count_of_airplanes)

Company4.changeCoS(45000)
print(Company4.count_of_staff)

Company5.CompanyIsExist()
