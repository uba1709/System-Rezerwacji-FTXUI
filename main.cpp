#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

// Obsługa FTXUI
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
using namespace ftxui;
using namespace std;

enum class Role{
    CLIENT,
    ADMIN
};

//Przypisanie statusu rezeracji
enum class ReservationStatus {
    PENDING,
    CONFIRMED,
    CANCELLED,
    COMPLETED
};

class Reservation{
    private:
        static unsigned int nextAutoIncrementId; // static unsigned int. Widoczna tylko dla class Reservation. unsigned sprawa zmienną tylko dodatnią 
        unsigned int id; // ID rezerwacji
        unsigned int userId; // ID uzytkownia w rezerwacji
        unsigned int resourceId; // ID zasobu
        time_t startTimestamp; // Data początkowa
        time_t endTimestamp; // Data zakonczenia
        unsigned int reservedSeats; // Stawka za osobe
        ReservationStatus status; // Status rezerwacji (enum class ReservationStatus)
        double totalPrice; // Cena calkowita

        // Obliczanie ceny rezeracji
        void calculateTotalPrice(double pricePerSlot){
            // Jeśli data zakonczenia jest taka sama lub wczesniejsza niz data startowa cena calkowita jest 0 
            if(endTimestamp <= startTimestamp){
                totalPrice = 0.0;
                return;
            }
            // Obliczanie ceny rezerwacji w godzinach przez czas trwania i stawke 
            double durationHours = difftime(endTimestamp, startTimestamp) / 3600;
            totalPrice = durationHours * reservedSeats * pricePerSlot;
        }
    public:
        //Tworzenie nowej rezerwacji
        //Przypisanie zmiennych do informacji o nowej rezerwacjii
        Reservation(unsigned int uId, unsigned rId, time_t start, time_t end, unsigned int seats, double pricePerSlot)
                // nextAutoIncrementId++ (Za kazdym razem o + 1)
                // ID Uzytkownika 
                // ID Rezerwacji
                // Data początkowa
                // Data zakonczenia
                // Stawka za osobe
                // Ustawienie statusu rezerwacji na PENDING
            : id(nextAutoIncrementId++), userId(uId), resourceId(rId), startTimestamp(start), endTimestamp(end),
                 reservedSeats(seats), status(ReservationStatus::PENDING){
                // Obliczenie ceny calkowitej
                calculateTotalPrice(pricePerSlot);
            }

        // Tylko do odczytu danych (const)
        unsigned int getId()            const {return id;}
        unsigned int getUserId()        const {return userId;}
        unsigned int getResourceId()    const {return resourceId;}
        time_t getStartTimestamp()      const {return startTimestamp;}
        time_t getEndTimestamp()        const {return endTimestamp;}
        unsigned int getReservedSeats() const {return reservedSeats;}
        ReservationStatus getStatus()   const {return status;}
        double getTotalPrice()          const {return totalPrice;}
        
        // Ustawienie nowego statusu dla rezerwacji
        void setStatus(ReservationStatus newStatus){status = newStatus;}
        // sprawdzenia czy status rezerwacji jest COMPLETED jesli tak -> Ustawienie statusu rezerwacji na CANCELLED 
        bool setCancel(){
            if(status == ReservationStatus::COMPLETED) return false;
            status = ReservationStatus::CANCELLED;
            return true;
        }
};

class User{
    private:
        static unsigned int nextAutoIncrementId; // static unsigned int. Widoczna tylko dla class Reservation. unsigned sprawa zmienną tylko dodatnią
        unsigned int id; // ID uzytkownika
        string fullName; // Imie i Nazwisko uzytkownika
        string phoneNumber; // Number telefonu uzytkownika
        Role role; // Przypisana rola (Admin lub Client) [enum class Role]
    public:
        // Tworzenie nowego uzytkownika 
        // Podanie imie i nazwisko
        // Number telfonu
        // Przypisana rola 
        User(string name, string phone, Role r) 
            // nextAutoIncrementId++ (Za kazdym razem o + 1)
            // Imie i nazwisko
            // Numer telefonu
            // Rola 
            : id(nextAutoIncrementId++), fullName(name), phoneNumber(phone), role(r){}

        // Tylko do odczytu danych (const)
        unsigned getIdUser()        const {return id;}
        string getFullNameUser()    const {return fullName;}
        string getPhoneNumberUser() const {return phoneNumber;}
        Role getRole()              const {return role;}
};

class Resource{
    private:
        static unsigned int nextAutoIncrementId; // static unsigned int. Widoczna tylko dla class Reservation. unsigned sprawa zmienną tylko dodatnią 
        unsigned int id; // ID zasobu
        string name; // Nazwa zasobu
        unsigned int capacity; // Ilosc miejsc 
        double pricePerSlot; // cena za miejsce
        bool isActive; // Status aktywnosci (Dostepne / Niedostepne)
    public:
        // Podanie nazwy zasobu
        // Ilosc miejsc 
        // cena za osobe
        Resource(string n, unsigned int cap, double price)
            // nextAutoIncrementId++ (Za kazdym razem o + 1)
            // Nazwa zasobu
            // Ilosc miejsc 
            // Cena za miejsce
            // Danie statusu Dosetpny (isActive(true))
            : id(nextAutoIncrementId++), name(n), capacity(cap), pricePerSlot(price), isActive(true) {} 
        unsigned int getIdResource() const {return id;}
        string getNameResource()     const {return name;}
        unsigned int getCapacity()   const {return capacity;}
        double getPricePerSlot()     const {return pricePerSlot;}
        bool getIsActive()           const {return isActive;}
        
       
        void setIsActive(bool active){isActive = active;}  // Zmana aktywnosci na Dostepny
        void setNewName(string newName){name = newName;}   // Ustawienie nowej nazwy dla zaosbu
        void setPricePerSlot(double newPrice){
            if(newPrice >= 0) pricePerSlot = newPrice; // Ustawienie nowej ceny za osobe
        }
};

// Przypisanie dla kazdej class zmiennej nextAutoIncrementId liczbe 1
unsigned int Reservation::nextAutoIncrementId = 1;
unsigned int User::nextAutoIncrementId        = 1;
unsigned int Resource::nextAutoIncrementId    = 1;

void AdminPanel(){
  int selected_option = 0;
  vector<string> options = {
    "[1] Lista wszystkich zasobow",
    "[2] Dodaj nowy zasob",
    "[3] Zmien cenę / dostepnosc zasobu",
    "[4] Zmien nazwe zasobu",
    "[5] Pelny harmonogram rezerwacji (Wszystkie)",
    "[6] Wszyscy uzytkownicy"
  };

  auto screen = ScreenInteractive::TerminalOutput();
  auto admin_menu = Menu(&options, &selected_option);
  string info_msg = "Zalogowano jako Administrator";
  auto base_component = Container::Vertical({
    admin_menu,
    Button("Zatwierdź", [&] {
      info_msg = "Wybrano: " + options[selected_option];
    }),
    Button("Wróć", [&] {
      screen.Exit();
    })
  });
  auto component = CatchEvent(base_component, [&](Event event) {
    if (event == Event::Escape) {
      screen.Exit();
      return true;
    }
    if (event == Event::Return) {
      info_msg = "Wybrano: " + options[selected_option];
      return true;
    }
    return false;
  });

  auto renderer = Renderer(component, [&] {
    return vbox({
      text("=== PANEL ADMINISTRATORA ===") | bold | color(Color::Blue) | center,
      separator(),
      admin_menu->Render() | border,
      separator(),
      text(info_msg),
      separator(),
      text("Wybierz opcję strzałkami, zatwierdź Enterem") | dim,
    }) | border;
  });
  screen.Loop(renderer);
}

void ClientPanel(){
  auto screen = ScreenInteractive::TerminalOutput();
  auto base_component = Container::Vertical({
    Button("Utwórz rezerwację", [&] {
      screen.Exit();
    }),
    Button("Wróć", [&] {
      screen.Exit();
    })
  });
  auto component = CatchEvent(base_component, [&](Event event) {
    if (event == Event::Escape) {
      screen.Exit();
      return true;
    }
    return false;
  });
  auto renderer = Renderer(component, [&] {
    return vbox({
      text("=== PANEL KLIENTA ===") | bold | color(Color::Blue) | center,
      separator(),
      text("Panel rezerwacji jest gotowy do rozbudowy."),
      separator(),
      text("Użyj Enter, aby wybrać przycisk.") | dim,
    }) | border;
  });
  screen.Loop(renderer);
}


int main() {
  vector<User> users;
                 users.push_back(User("Admin",        "+48 111 222 333", Role::ADMIN));
                 users.push_back(User("KlientTest",   "+48 999 888 777", Role::CLIENT));
  vector<Resource> resources;
                     resources.push_back(Resource("Sala Konferencyjna A", 20, 50.0));
  vector<Reservation> reservations;

  int role_selection = 0;
  vector<string> roles = {
    "[1] Klient (Dokonaj rezerwacji)",
    "[2] Administrator (Zarządzanie systemem)",
    "[3] Wyjdź z programu"
  };

  auto role_menu = Menu(&roles, &role_selection);
  auto screen = ScreenInteractive::TerminalOutput();

  auto role_component = Container::Vertical({
      role_menu,
      Button("Wybierz rolę", [&] {
        screen.Exit(); // Zamknij ekran wyboru ról i przejdź do akcji
      })
  });
  role_component = CatchEvent(role_component, [&](Event event) {
    if (event == Event::Return) {
      screen.Exit();
      return true;
    }
    return false;
  });

  //glowna pentla programu
  screen.Clear();
  while(true){
      screen.Clear();
      auto role_renderer = Renderer(role_component, [&] {
      return vbox({
                 text("=== SYSTEM REZERWACJI - WYBÓR ROLI ===") | bold | color(Color::Blue) | center,
                 separator(),
                 role_menu->Render() | border,
             }) | border;
    });
    screen.Loop(role_renderer);

    if(role_selection == 0){
      screen.Clear();
      ClientPanel();
    }
    else if(role_selection == 1){
      screen.Clear();
      AdminPanel();
    }
    else{
      //Wyjscie
      return 0;
    }

  }
}