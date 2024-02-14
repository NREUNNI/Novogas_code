#include <iostream>
#include <string>
#include <random>
#include <sstream>

class SmartValve {
private:
    std::string id;
    int porcent;
    int balance;
    bool canBeUsed;
    int totalCapacity;

    // Método privado para generar un ID único aleatorio si no se proporciona uno
    std::string generateID() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1000, 9999);
        return "SV" + std::to_string(dis(gen));
    }

    // Constructor privado para asegurar que no se puedan crear instancias fuera de la clase
    SmartValve(const std::string& customID = "") {
        // Establecer el ID, generando uno aleatorio si no se proporciona uno
        id = customID.empty() ? generateID() : customID;
        // Inicializar otras variables
        porcent = 0;
        balance = 0;
        canBeUsed = true;
        totalCapacity = 0;
    }

public:
    // Método estático para obtener la única instancia del Singleton
    static SmartValve& getInstance(const std::string& customID = "") {
        static SmartValve instance(customID);
        return instance;
    }

    // Deshabilitar constructores de copia y operadores de asignación
    SmartValve(const SmartValve&) = delete;
    SmartValve& operator=(const SmartValve&) = delete;

    // Métodos para acceder y modificar las variables
    std::string getId() const {
        return id;
    }

    int getPorcent() const {
        return porcent;
    }

    void setPorcent(int value) {
        porcent = value;
    }

    int getBalance() const {
        return balance;
    }

    void setBalance(int value) {
        balance = value;
    }

    bool getCanBeUsed() const {
        return canBeUsed;
    }

    void setCanBeUsed(bool value) {
        canBeUsed = value;
    }

    int getTotalCapacity() const {
        return totalCapacity;
    }

    void setTotalCapacity(int value) {
        totalCapacity = value;
    }
    
   bool isInicialized = false;

   void init() {
      isInicialized = true;
    }
};
