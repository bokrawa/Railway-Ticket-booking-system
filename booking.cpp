// RAILWAY TICKET BOOKING SYSTEM
// Author: Claude
// Date: April 2025
// Description: OOP-based Railway Ticket Booking System with MySQL integration

// ============= DATABASE SCHEMA (MySQL) =============
/*
-- Create database
CREATE DATABASE railway_booking_system;
USE railway_booking_system;

-- Create tables
CREATE TABLE trains (
    train_id INT PRIMARY KEY AUTO_INCREMENT,
    train_name VARCHAR(100) NOT NULL,
    train_number VARCHAR(20) UNIQUE NOT NULL,
    source VARCHAR(100) NOT NULL,
    destination VARCHAR(100) NOT NULL,
    departure_time TIME NOT NULL,
    arrival_time TIME NOT NULL,
    total_seats INT NOT NULL
);

CREATE TABLE users (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    full_name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    phone VARCHAR(15) NOT NULL,
    registration_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE bookings (
    booking_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    train_id INT NOT NULL,
    booking_date DATE NOT NULL,
    journey_date DATE NOT NULL,
    num_passengers INT NOT NULL,
    total_fare DECIMAL(10,2) NOT NULL,
    booking_status ENUM('Confirmed', 'Waiting', 'Cancelled') DEFAULT 'Confirmed',
    payment_status ENUM('Paid', 'Pending') DEFAULT 'Pending',
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (train_id) REFERENCES trains(train_id)
);

CREATE TABLE passengers (
    passenger_id INT PRIMARY KEY AUTO_INCREMENT,
    booking_id INT NOT NULL,
    passenger_name VARCHAR(100) NOT NULL,
    age INT NOT NULL,
    gender ENUM('Male', 'Female', 'Other') NOT NULL,
    seat_number VARCHAR(10),
    FOREIGN KEY (booking_id) REFERENCES bookings(booking_id) ON DELETE CASCADE
);

-- Sample data
INSERT INTO trains (train_name, train_number, source, destination, departure_time, arrival_time, total_seats) VALUES
('Rajdhani Express', 'RAJ2025', 'Delhi', 'Mumbai', '16:00:00', '08:00:00', 500),
('Shatabdi Express', 'SHT1050', 'Chennai', 'Bangalore', '06:00:00', '10:30:00', 400),
('Duronto Express', 'DUR2210', 'Kolkata', 'Delhi', '23:00:00', '14:00:00', 450);
*/

// ============= HEADER FILES =============
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <limits>
#include <memory>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

// ============= UTILITY FUNCTIONS =============
class Utility {
public:
    static string getCurrentDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        
        string year = to_string(1900 + ltm->tm_year);
        string month = to_string(1 + ltm->tm_mon);
        if (month.length() == 1) month = "0" + month;
        
        string day = to_string(ltm->tm_mday);
        if (day.length() == 1) day = "0" + day;
        
        return year + "-" + month + "-" + day;
    }
    
    static void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    static void pressEnterToContinue() {
        cout << "\nPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    static string getInput(const string& prompt) {
        string input;
        cout << prompt;
        getline(cin, input);
        return input;
    }
    
    static int getIntInput(const string& prompt) {
        int input;
        string str_input;
        bool valid = false;
        
        while (!valid) {
            cout << prompt;
            getline(cin, str_input);
            
            try {
                input = stoi(str_input);
                valid = true;
            } catch (const exception& e) {
                cout << "Invalid input. Please enter a number.\n";
            }
        }
        
        return input;
    }
    
    static double getDoubleInput(const string& prompt) {
        double input;
        string str_input;
        bool valid = false;
        
        while (!valid) {
            cout << prompt;
            getline(cin, str_input);
            
            try {
                input = stod(str_input);
                valid = true;
            } catch (const exception& e) {
                cout << "Invalid input. Please enter a number.\n";
            }
        }
        
        return input;
    }
};

// ============= DATABASE CONNECTION =============
class DatabaseConnector {
private:
    sql::Driver* driver;
    sql::Connection* con;
    const string server = "tcp://127.0.0.1:3306";
    const string username = "root";
    const string password = "password"; // Change to your MySQL password

public:
    DatabaseConnector() {
        try {
            driver = get_driver_instance();
            con = driver->connect(server, username, password);
            con->setSchema("railway_booking_system");
            cout << "Database connection established successfully.\n";
        } catch (sql::SQLException &e) {
            cout << "# ERR: SQLException in " << __FILE__;
            cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
            cout << "# ERR: " << e.what();
            cout << " (MySQL error code: " << e.getErrorCode();
            cout << ", SQLState: " << e.getSQLState() << " )" << endl;
            exit(1);
        }
    }
    
    ~DatabaseConnector() {
        delete con;
    }
    
    sql::Connection* getConnection() {
        return con;
    }
};

// ============= BASE CLASSES =============
class Person {
protected:
    string name;
    string email;
    string phone;

public:
    Person() : name(""), email(""), phone("") {}
    Person(string name, string email, string phone) 
        : name(name), email(email), phone(phone) {}
    
    virtual ~Person() {}
    
    // Getters
    string getName() const { return name; }
    string getEmail() const { return email; }
    string getPhone() const { return phone; }
    
    // Setters
    void setName(const string& name) { this->name = name; }
    void setEmail(const string& email) { this->email = email; }
    void setPhone(const string& phone) { this->phone = phone; }
    
    virtual void displayInfo() const = 0; // Pure virtual function
};

// ============= USER CLASSES =============
class User : public Person {
private:
    int userId;
    string username;
    string password;
    string registrationDate;

public:
    User() : Person(), userId(0), username(""), password(""), registrationDate("") {}
    
    User(int userId, string username, string password, string name, string email, string phone, string regDate)
        : Person(name, email, phone), userId(userId), username(username), password(password), registrationDate(regDate) {}
    
    // Getters
    int getUserId() const { return userId; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getRegistrationDate() const { return registrationDate; }
    
    // Setters
    void setUserId(int id) { userId = id; }
    void setUsername(const string& uname) { username = uname; }
    void setPassword(const string& pwd) { password = pwd; }
    void setRegistrationDate(const string& date) { registrationDate = date; }
    
    void displayInfo() const override {
        cout << "User ID: " << userId << endl;
        cout << "Username: " << username << endl;
        cout << "Name: " << name << endl;
        cout << "Email: " << email << endl;
        cout << "Phone: " << phone << endl;
        cout << "Registration Date: " << registrationDate << endl;
    }
};

class UserManager {
private:
    DatabaseConnector* dbConnector;
    
public:
    UserManager(DatabaseConnector* connector) : dbConnector(connector) {}
    
    bool registerUser(User& user) {
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "INSERT INTO users(username, password, full_name, email, phone) VALUES(?, ?, ?, ?, ?)");
            
            pstmt->setString(1, user.getUsername());
            pstmt->setString(2, user.getPassword());
            pstmt->setString(3, user.getName());
            pstmt->setString(4, user.getEmail());
            pstmt->setString(5, user.getPhone());
            
            pstmt->executeUpdate();
            
            // Get the user ID of the newly registered user
            sql::Statement* stmt = con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT LAST_INSERT_ID() as id");
            
            if (res->next()) {
                user.setUserId(res->getInt("id"));
            }
            
            delete pstmt;
            delete stmt;
            delete res;
            
            return true;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return false;
        }
    }
    
    User* loginUser(const string& username, const string& password) {
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "SELECT * FROM users WHERE username = ? AND password = ?");
            
            pstmt->setString(1, username);
            pstmt->setString(2, password);
            
            sql::ResultSet* res = pstmt->executeQuery();
            
            if (res->next()) {
                User* user = new User(
                    res->getInt("user_id"),
                    res->getString("username"),
                    res->getString("password"),
                    res->getString("full_name"),
                    res->getString("email"),
                    res->getString("phone"),
                    res->getString("registration_date")
                );
                
                delete pstmt;
                delete res;
                
                return user;
            }
            
            delete pstmt;
            delete res;
            
            return nullptr;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return nullptr;
        }
    }
    
    bool updateUserProfile(const User& user) {
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "UPDATE users SET full_name = ?, email = ?, phone = ? WHERE user_id = ?"
            );
            
            pstmt->setString(1, user.getName());
            pstmt->setString(2, user.getEmail());
            pstmt->setString(3, user.getPhone());
            pstmt->setInt(4, user.getUserId());
            
            pstmt->executeUpdate();
            delete pstmt;
            
            return true;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return false;
        }
    }
    
    bool changePassword(int userId, const string& newPassword) {
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "UPDATE users SET password = ? WHERE user_id = ?"
            );
            
            pstmt->setString(1, newPassword);
            pstmt->setInt(2, userId);
            
            pstmt->executeUpdate();
            delete pstmt;
            
            return true;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return false;
        }
    }
};

// ============= TRAIN CLASSES =============
class Train {
private:
    int trainId;
    string trainName;
    string trainNumber;
    string source;
    string destination;
    string departureTime;
    string arrivalTime;
    int totalSeats;

public:
    Train() : trainId(0), trainName(""), trainNumber(""), source(""), destination(""),
              departureTime(""), arrivalTime(""), totalSeats(0) {}
    
    Train(int id, string name, string number, string src, string dest, 
          string depTime, string arrTime, int seats)
        : trainId(id), trainName(name), trainNumber(number), source(src), destination(dest),
          departureTime(depTime), arrivalTime(arrTime), totalSeats(seats) {}
    
    // Getters
    int getTrainId() const { return trainId; }
    string getTrainName() const { return trainName; }
    string getTrainNumber() const { return trainNumber; }
    string getSource() const { return source; }
    string getDestination() const { return destination; }
    string getDepartureTime() const { return departureTime; }
    string getArrivalTime() const { return arrivalTime; }
    int getTotalSeats() const { return totalSeats; }
    
    // Setters
    void setTrainId(int id) { trainId = id; }
    void setTrainName(const string& name) { trainName = name; }
    void setTrainNumber(const string& number) { trainNumber = number; }
    void setSource(const string& src) { source = src; }
    void setDestination(const string& dest) { destination = dest; }
    void setDepartureTime(const string& depTime) { departureTime = depTime; }
    void setArrivalTime(const string& arrTime) { arrivalTime = arrTime; }
    void setTotalSeats(int seats) { totalSeats = seats; }
    
    void displayInfo() const {
        cout << left << setw(5) << trainId
             << setw(20) << trainName
             << setw(12) << trainNumber
             << setw(15) << source
             << setw(15) << destination
             << setw(12) << departureTime
             << setw(12) << arrivalTime
             << setw(8) << totalSeats << endl;
    }
    
    static void displayHeader() {
        cout << left << setw(5) << "ID"
             << setw(20) << "Train Name"
             << setw(12) << "Number"
             << setw(15) << "Source"
             << setw(15) << "Destination"
             << setw(12) << "Departure"
             << setw(12) << "Arrival"
             << setw(8) << "Seats" << endl;
        cout << string(99, '-') << endl;
    }
};

class TrainManager {
private:
    DatabaseConnector* dbConnector;
    
public:
    TrainManager(DatabaseConnector* connector) : dbConnector(connector) {}
    
    vector<Train> searchTrains(const string& source, const string& destination) {
        vector<Train> trains;
        
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "SELECT * FROM trains WHERE source LIKE ? AND destination LIKE ?");
            
            pstmt->setString(1, "%" + source + "%");
            pstmt->setString(2, "%" + destination + "%");
            
            sql::ResultSet* res = pstmt->executeQuery();
            
            while (res->next()) {
                Train train(
                    res->getInt("train_id"),
                    res->getString("train_name"),
                    res->getString("train_number"),
                    res->getString("source"),
                    res->getString("destination"),
                    res->getString("departure_time"),
                    res->getString("arrival_time"),
                    res->getInt("total_seats")
                );
                
                trains.push_back(train);
            }
            
            delete pstmt;
            delete res;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
        }
        
        return trains;
    }
    
    vector<Train> getAllTrains() {
        vector<Train> trains;
        
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::Statement* stmt = con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT * FROM trains");
            
            while (res->next()) {
                Train train(
                    res->getInt("train_id"),
                    res->getString("train_name"),
                    res->getString("train_number"),
                    res->getString("source"),
                    res->getString("destination"),
                    res->getString("departure_time"),
                    res->getString("arrival_time"),
                    res->getInt("total_seats")
                );
                
                trains.push_back(train);
            }
            
            delete stmt;
            delete res;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
        }
        
        return trains;
    }
    
    Train* getTrainById(int trainId) {
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM trains WHERE train_id = ?");
            
            pstmt->setInt(1, trainId);
            sql::ResultSet* res = pstmt->executeQuery();
            
            if (res->next()) {
                Train* train = new Train(
                    res->getInt("train_id"),
                    res->getString("train_name"),
                    res->getString("train_number"),
                    res->getString("source"),
                    res->getString("destination"),
                    res->getString("departure_time"),
                    res->getString("arrival_time"),
                    res->getInt("total_seats")
                );
                
                delete pstmt;
                delete res;
                
                return train;
            }
            
            delete pstmt;
            delete res;
            
            return nullptr;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return nullptr;
        }
    }
    
    int getAvailableSeats(int trainId, const string& journeyDate) {
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "SELECT t.total_seats - COALESCE(SUM(b.num_passengers), 0) AS available_seats "
                "FROM trains t LEFT JOIN bookings b ON t.train_id = b.train_id AND b.journey_date = ? AND b.booking_status = 'Confirmed' "
                "WHERE t.train_id = ? GROUP BY t.train_id");
            
            pstmt->setString(1, journeyDate);
            pstmt->setInt(2, trainId);
            
            sql::ResultSet* res = pstmt->executeQuery();
            
            int availableSeats = 0;
            if (res->next()) {
                availableSeats = res->getInt("available_seats");
            } else {
                // If no bookings found, get total seats from train
                delete pstmt;
                delete res;
                
                pstmt = con->prepareStatement("SELECT total_seats FROM trains WHERE train_id = ?");
                pstmt->setInt(1, trainId);
                res = pstmt->executeQuery();
                
                if (res->next()) {
                    availableSeats = res->getInt("total_seats");
                }
            }
            
            delete pstmt;
            delete res;
            
            return availableSeats;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return 0;
        }
    }
};

// ============= BOOKING CLASSES =============
class Passenger {
private:
    int passengerId;
    string passengerName;
    int age;
    string gender;
    string seatNumber;

public:
    Passenger() : passengerId(0), passengerName(""), age(0), gender(""), seatNumber("") {}
    
    Passenger(int id, string name, int age, string gender, string seat = "")
        : passengerId(id), passengerName(name), age(age), gender(gender), seatNumber(seat) {}
    
    // Getters
    int getPassengerId() const { return passengerId; }
    string getPassengerName() const { return passengerName; }
    int getAge() const { return age; }
    string getGender() const { return gender; }
    string getSeatNumber() const { return seatNumber; }
    
    // Setters
    void setPassengerId(int id) { passengerId = id; }
    void setPassengerName(const string& name) { passengerName = name; }
    void setAge(int a) { age = a; }
    void setGender(const string& g) { gender = g; }
    void setSeatNumber(const string& seat) { seatNumber = seat; }
    
    void displayInfo() const {
        cout << left << setw(5) << passengerId
             << setw(25) << passengerName
             << setw(5) << age
             << setw(10) << gender
             << setw(10) << seatNumber << endl;
    }
    
    static void displayHeader() {
        cout << left << setw(5) << "ID"
             << setw(25) << "Name"
             << setw(5) << "Age"
             << setw(10) << "Gender"
             << setw(10) << "Seat" << endl;
        cout << string(55, '-') << endl;
    }
};

class Booking {
private:
    int bookingId;
    int userId;
    int trainId;
    string bookingDate;
    string journeyDate;
    int numPassengers;
    double totalFare;
    string bookingStatus;
    string paymentStatus;
    vector<Passenger> passengers;

public:
    Booking() : bookingId(0), userId(0), trainId(0), bookingDate(""), journeyDate(""),
                numPassengers(0), totalFare(0.0), bookingStatus(""), paymentStatus("") {}
    
    Booking(int bookId, int usrId, int trnId, string bookDate, string jrnyDate,
            int numPass, double fare, string bookStatus, string payStatus)
        : bookingId(bookId), userId(usrId), trainId(trnId), bookingDate(bookDate),
          journeyDate(jrnyDate), numPassengers(numPass), totalFare(fare),
          bookingStatus(bookStatus), paymentStatus(payStatus) {}
    
    // Getters
    int getBookingId() const { return bookingId; }
    int getUserId() const { return userId; }
    int getTrainId() const { return trainId; }
    string getBookingDate() const { return bookingDate; }
    string getJourneyDate() const { return journeyDate; }
    int getNumPassengers() const { return numPassengers; }
    double getTotalFare() const { return totalFare; }
    string getBookingStatus() const { return bookingStatus; }
    string getPaymentStatus() const { return paymentStatus; }
    vector<Passenger> getPassengers() const { return passengers; }
    
    // Setters
    void setBookingId(int id) { bookingId = id; }
    void setUserId(int id) { userId = id; }
    void setTrainId(int id) { trainId = id; }
    void setBookingDate(const string& date) { bookingDate = date; }
    void setJourneyDate(const string& date) { journeyDate = date; }
    void setNumPassengers(int num) { numPassengers = num; }
    void setTotalFare(double fare) { totalFare = fare; }
    void setBookingStatus(const string& status) { bookingStatus = status; }
    void setPaymentStatus(const string& status) { paymentStatus = status; }
    
    void addPassenger(const Passenger& passenger) {
        passengers.push_back(passenger);
    }
    
    void displayInfo(const Train& train) const {
        cout << "\n====== Booking Details ======\n";
        cout << "Booking ID: " << bookingId << endl;
        cout << "Booking Date: " << bookingDate << endl;
        cout << "Journey Date: " << journeyDate << endl;
        cout << "Train: " << train.getTrainName() << " (" << train.getTrainNumber() << ")" << endl;
        cout << "From: " << train.getSource() << " To: " << train.getDestination() << endl;
        cout << "Departure: " << train.getDepartureTime() << " Arrival: " << train.getArrivalTime() << endl;
        cout << "Number of Passengers: " << numPassengers << endl;
        cout << "Total Fare: $" << fixed << setprecision(2) << totalFare << endl;
        cout << "Booking Status: " << bookingStatus << endl;
        cout << "Payment Status: " << paymentStatus << endl;
        
        cout << "\n------ Passenger Details ------\n";
        Passenger::displayHeader();
        for (const auto& passenger : passengers) {
            passenger.displayInfo();
        }
    }
};

class BookingManager {
private:
    DatabaseConnector* dbConnector;
    TrainManager* trainManager;
    
    // Calculate fare based on distance, train type, etc.
    double calculateFare(int trainId, int numPassengers) {
        // Simple fare calculation (can be made more complex)
        double baseFare = 50.0; // Base fare per passenger
        return baseFare * numPassengers;
    }
    
    bool addPassengers(int bookingId, const vector<Passenger>& passengers) {
        try {
            sql::Connection* con = dbConnector->getConnection();
            
            for (const auto& passenger : passengers) {
                sql::PreparedStatement* pstmt = con->prepareStatement(
                    "INSERT INTO passengers(booking_id, passenger_name, age, gender, seat_number) VALUES(?, ?, ?, ?, ?)");
                
                pstmt->setInt(1, bookingId);
                pstmt->setString(2, passenger.getPassengerName());
                pstmt->setInt(3, passenger.getAge());
                pstmt->setString(4, passenger.getGender());
                pstmt->setString(5, passenger.getSeatNumber());
                
                pstmt->executeUpdate();
                delete pstmt;
            }
            
            return true;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return false;
        }
    }
    
public:
    BookingManager(DatabaseConnector* connector, TrainManager* trainMgr) 
        : dbConnector(connector), trainManager(trainMgr) {}
    
    bool createBooking(Booking& booking) {
        try {
            // Check if seats are available
            int availableSeats = trainManager->getAvailableSeats(booking.getTrainId(), booking.getJourneyDate());
            
            if (availableSeats < booking.getNumPassengers()) {
                cout << "Sorry, only " << availableSeats << " seats are available for this train on the selected date.\n";
                return false;
            }
            
            // Calculate fare
            double fare = calculateFare(booking.getTrainId(), booking.getNumPassengers());
            booking.setTotalFare(fare);
            
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "INSERT INTO bookings(user_id, train_id, booking_date, journey_date, num_passengers, total_fare, booking_status, payment_status) "
                "VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
            
            pstmt->setInt(1, booking.getUserId());
            pstmt->setInt(2, booking.getTrainId());
            pstmt->setString(3, booking.getBookingDate());
            pstmt->setString(4, booking.getJourneyDate());
            pstmt->setInt(5, booking.getNumPassengers());
            pstmt->setDouble(6, booking.getTotalFare());
            pstmt->setString(7, booking.getBookingStatus());
            pstmt->setString(8, booking.getPaymentStatus());
            
            pstmt->executeUpdate();
            
            // Get the booking ID of the newly created booking
            sql::Statement* stmt = con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT LAST_INSERT_ID() as id");
            
            if (res->next()) {
                booking.setBookingId(res->getInt("id"));
            }
            
            delete pstmt;
            delete stmt;
            delete res;
            
            // Add passengers
            addPassengers(booking.getBookingId(), booking.getPassengers());
            
            return true;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return false;
        }
    }
    
    bool cancelBooking(int bookingId) {
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "UPDATE bookings SET booking_status = 'Cancelled' WHERE booking_id = ?");
            
            pstmt->setInt(1, bookingId);
            pstmt->executeUpdate();
            delete pstmt;
            
            return true;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return false;
        }
    }
    
    bool updatePaymentStatus(int bookingId, const string& status) {
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "UPDATE bookings SET payment_status = ? WHERE booking_id = ?");
            
            pstmt->setString(1, status);
            pstmt->setInt(2, bookingId);
            pstmt->executeUpdate();
            delete pstmt;
            
            return true;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return false;
        }
    }
    
    vector<Booking> getUserBookings(int userId) {
        vector<Booking> bookings;
        
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "SELECT * FROM bookings WHERE user_id = ? ORDER BY booking_date DESC");
            
            pstmt->setInt(1, userId);
            sql::ResultSet* res = pstmt->executeQuery();
            
            while (res->next()) {
                Booking booking(
                    res->getInt("booking_id"),
                    res->getInt("user_id"),
                    res->getInt("train_id"),
                    res->getString("booking_date"),
                    res->getString("journey_date"),
                    res->getInt("num_passengers"),
                    res->getDouble("total_fare"),
                    res->getString("booking_status"),
                    res->getString("payment_status")
                );
                
                // Get passengers for this booking
                sql::PreparedStatement* pstmt2 = con->prepareStatement(
                    "SELECT * FROM passengers WHERE booking_id = ?");
                
                pstmt2->setInt(1, booking.getBookingId());
                sql::ResultSet* passengerRes = pstmt2->executeQuery();
                
                while (passengerRes->next()) {
                    Passenger passenger(
                        passengerRes->getInt("passenger_id"),
                        passengerRes->getString("passenger_name"),
                        passengerRes->getInt("age"),
                        passengerRes->getString("gender"),
                        passengerRes->getString("seat_number")
                    );
                    
                    booking.addPassenger(passenger);
                }
                
                delete pstmt2;
                delete passengerRes;
                
                bookings.push_back(booking);
            }
            
            delete pstmt;
            delete res;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
        }
        
        return bookings;
    }
    
    Booking* getBookingById(int bookingId) {
        try {
            sql::Connection* con = dbConnector->getConnection();
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "SELECT * FROM bookings WHERE booking_id = ?");
            
            pstmt->setInt(1, bookingId);
            sql::ResultSet* res = pstmt->executeQuery();
            
            if (res->next()) {
                Booking* booking = new Booking(
                    res->getInt("booking_id"),
                    res->getInt("user_id"),
                    res->getInt("train_id"),
                    res->getString("booking_date"),
                    res->getString("journey_date"),
                    res->getInt("num_passengers"),
                    res->getDouble("total_fare"),
                    res->getString("booking_status"),
                    res->getString("payment_status")
                );
                
                // Get passengers for this booking
                sql::PreparedStatement* pstmt2 = con->prepareStatement(
                    "SELECT * FROM passengers WHERE booking_id = ?");
                
                pstmt2->setInt(1, booking->getBookingId());
                sql::ResultSet* passengerRes = pstmt2->executeQuery();
                
                while (passengerRes->next()) {
                    Passenger passenger(
                        passengerRes->getInt("passenger_id"),
                        passengerRes->getString("passenger_name"),
                        passengerRes->getInt("age"),
                        passengerRes->getString("gender"),
                        passengerRes->getString("seat_number")
                    );
                    
                    booking->addPassenger(passenger);
                }
                
                delete pstmt2;
                delete passengerRes;
                delete pstmt;
                delete res;
                
                return booking;
            }
            
            delete pstmt;
            delete res;
            
            return nullptr;
        } catch (sql::SQLException &e) {
            cout << "SQL Error: " << e.what() << endl;
            return nullptr;
        }
    }
};

// ============= PAYMENT SYSTEM =============
class PaymentSystem {
private:
    DatabaseConnector* dbConnector;
    BookingManager* bookingManager;
    
public:
    PaymentSystem(DatabaseConnector* connector, BookingManager* bookingMgr)
        : dbConnector(connector), bookingManager(bookingMgr) {}
    
    bool processPayment(int bookingId, const string& paymentMethod) {
        // Simulate payment processing
        cout << "Processing payment for booking #" << bookingId << " using " << paymentMethod << "...\n";
        
        // In a real system, we would integrate with payment gateways
        // For simulation, we'll just update the payment status
        if (bookingManager->updatePaymentStatus(bookingId, "Paid")) {
            cout << "Payment successful!\n";
            return true;
        } else {
            cout << "Payment failed. Please try again.\n";
            return false;
        }
    }
    
    void displayPaymentOptions() {
        cout << "\n------ Payment Options ------\n";
        cout << "1. Credit Card\n";
        cout << "2. Debit Card\n";
        cout << "3. Net Banking\n";
        cout << "4. UPI Payment\n";
        cout << "5. Cancel Payment\n";
    }
};

// ============= MENU SYSTEM =============
class Menu {
private:
    DatabaseConnector* dbConnector;
    UserManager* userManager;
    TrainManager* trainManager;
    BookingManager* bookingManager;
    PaymentSystem* paymentSystem;
    User* currentUser;
    
    void displayMainMenu() const {
        Utility::clearScreen();
        cout << "\n===== RAILWAY TICKET BOOKING SYSTEM =====\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Exit\n";
        cout << "Choose an option: ";
    }
    
    void displayUserMenu() const {
        Utility::clearScreen();
        cout << "\n===== Welcome, " << currentUser->getName() << "! =====\n";
        cout << "1. Search Trains\n";
        cout << "2. View All Trains\n";
        cout << "3. Book Ticket\n";
        cout << "4. View My Bookings\n";
        cout << "5. Cancel Booking\n";
        cout << "6. Update Profile\n";
        cout << "7. Change Password\n";
        cout << "8. Logout\n";
        cout << "Choose an option: ";
    }
    
    void registerUser() {
        Utility::clearScreen();
        cout << "\n===== USER REGISTRATION =====\n";
        
        string username = Utility::getInput("Enter username: ");
        string password = Utility::getInput("Enter password: ");
        string fullName = Utility::getInput("Enter full name: ");
        string email = Utility::getInput("Enter email: ");
        string phone = Utility::getInput("Enter phone number: ");
        
        User newUser(0, username, password, fullName, email, phone, Utility::getCurrentDate());
        
        if (userManager->registerUser(newUser)) {
            cout << "Registration successful! You can now login.\n";
        } else {
            cout << "Registration failed. Username or email might already exist.\n";
        }
        
        Utility::pressEnterToContinue();
    }
    
    bool loginUser() {
        Utility::clearScreen();
        cout << "\n===== USER LOGIN =====\n";
        
        string username = Utility::getInput("Enter username: ");
        string password = Utility::getInput("Enter password: ");
        
        currentUser = userManager->loginUser(username, password);
        
        if (currentUser) {
            cout << "Login successful! Welcome, " << currentUser->getName() << "!\n";
            Utility::pressEnterToContinue();
            return true;
        } else {
            cout << "Login failed. Invalid username or password.\n";
            Utility::pressEnterToContinue();
            return false;
        }
    }
    
    void searchTrains() {
        Utility::clearScreen();
        cout << "\n===== SEARCH TRAINS =====\n";
        
        string source = Utility::getInput("Enter source station (or part of name): ");
        string destination = Utility::getInput("Enter destination station (or part of name): ");
        
        vector<Train> trains = trainManager->searchTrains(source, destination);
        
        if (trains.empty()) {
            cout << "No trains found matching your criteria.\n";
        } else {
            cout << "\nFound " << trains.size() << " train(s):\n";
            Train::displayHeader();
            
            for (const auto& train : trains) {
                train.displayInfo();
            }
        }
        
        Utility::pressEnterToContinue();
    }
    
    void viewAllTrains() {
        Utility::clearScreen();
        cout << "\n===== ALL AVAILABLE TRAINS =====\n";
        
        vector<Train> trains = trainManager->getAllTrains();
        
        if (trains.empty()) {
            cout << "No trains available in the system.\n";
        } else {
            cout << "\nTotal " << trains.size() << " train(s):\n";
            Train::displayHeader();
            
            for (const auto& train : trains) {
                train.displayInfo();
            }
        }
        
        Utility::pressEnterToContinue();
    }
    
    void bookTicket() {
        Utility::clearScreen();
        cout << "\n===== BOOK TRAIN TICKET =====\n";
        
        // First, show all available trains
        vector<Train> trains = trainManager->getAllTrains();
        
        if (trains.empty()) {
            cout << "No trains available for booking.\n";
            Utility::pressEnterToContinue();
            return;
        }
        
        cout << "\nAvailable Trains:\n";
        Train::displayHeader();
        
        for (const auto& train : trains) {
            train.displayInfo();
        }
        
        int trainId = Utility::getIntInput("\nEnter Train ID to book: ");
        
        // Check if train exists
        Train* selectedTrain = trainManager->getTrainById(trainId);
        if (!selectedTrain) {
            cout << "Invalid Train ID. Please try again.\n";
            Utility::pressEnterToContinue();
            return;
        }
        
        string journeyDate = Utility::getInput("Enter journey date (YYYY-MM-DD): ");
        
        // Check available seats
        int availableSeats = trainManager->getAvailableSeats(trainId, journeyDate);
        cout << "Available seats: " << availableSeats << endl;
        
        if (availableSeats <= 0) {
            cout << "Sorry, no seats available for this train on the selected date.\n";
            Utility::pressEnterToContinue();
            delete selectedTrain;
            return;
        }
        
        int numPassengers = Utility::getIntInput("Enter number of passengers: ");
        
        if (numPassengers <= 0 || numPassengers > availableSeats) {
            cout << "Invalid number of passengers. Please try again.\n";
            Utility::pressEnterToContinue();
            delete selectedTrain;
            return;
        }
        
        // Create booking
        Booking newBooking(
            0, currentUser->getUserId(), trainId, Utility::getCurrentDate(), journeyDate,
            numPassengers, 0.0, "Confirmed", "Pending"
        );
        
        // Add passenger details
        cout << "\nEnter passenger details:\n";
        
        for (int i = 0; i < numPassengers; i++) {
            cout << "\nPassenger " << (i + 1) << ":\n";
            string name = Utility::getInput("Name: ");
            int age = Utility::getIntInput("Age: ");
            string gender = Utility::getInput("Gender (Male/Female/Other): ");
            
            // Assign seat number (simple sequential assignment)
            string seatNumber = "A" + to_string(i + 1);
            
            Passenger passenger(0, name, age, gender, seatNumber);
            newBooking.addPassenger(passenger);
        }
        
        // Save booking
        if (bookingManager->createBooking(newBooking)) {
            cout << "\nBooking created successfully! Booking ID: " << newBooking.getBookingId() << endl;
            cout << "Total fare: $" << fixed << setprecision(2) << newBooking.getTotalFare() << endl;
            
            // Process payment
            cout << "\nProceed to payment? (y/n): ";
            string choice;
            getline(cin, choice);
            
            if (choice == "y" || choice == "Y") {
                paymentSystem->displayPaymentOptions();
                int paymentOption = Utility::getIntInput("Select payment method: ");
                
                string paymentMethod;
                switch (paymentOption) {
                    case 1: paymentMethod = "Credit Card"; break;
                    case 2: paymentMethod = "Debit Card"; break;
                    case 3: paymentMethod = "Net Banking"; break;
                    case 4: paymentMethod = "UPI Payment"; break;
                    default: 
                        cout << "Payment cancelled.\n";
                        Utility::pressEnterToContinue();
                        delete selectedTrain;
                        return;
                }
                
                paymentSystem->processPayment(newBooking.getBookingId(), paymentMethod);
            }
        } else {
            cout << "Booking failed. Please try again.\n";
        }
        
        delete selectedTrain;
        Utility::pressEnterToContinue();
    }
    
    void viewMyBookings() {
        Utility::clearScreen();
        cout << "\n===== MY BOOKINGS =====\n";
        
        vector<Booking> bookings = bookingManager->getUserBookings(currentUser->getUserId());
        
        if (bookings.empty()) {
            cout << "You don't have any bookings yet.\n";
        } else {
            cout << "You have " << bookings.size() << " booking(s):\n\n";
            
            for (const auto& booking : bookings) {
                Train* train = trainManager->getTrainById(booking.getTrainId());
                if (train) {
                    booking.displayInfo(*train);
                    delete train;
                }
                cout << "\n" << string(40, '-') << "\n";
            }
        }
        
        Utility::pressEnterToContinue();
    }
    
    void cancelBooking() {
        Utility::clearScreen();
        cout << "\n===== CANCEL BOOKING =====\n";
        
        vector<Booking> bookings = bookingManager->getUserBookings(currentUser->getUserId());
        
        if (bookings.empty()) {
            cout << "You don't have any bookings to cancel.\n";
            Utility::pressEnterToContinue();
            return;
        }
        
        cout << "Your active bookings:\n\n";
        cout << left << setw(10) << "BookingID" 
             << setw(15) << "Journey Date" 
             << setw(10) << "Train" 
             << setw(8) << "Status" << endl;
        cout << string(43, '-') << endl;
        
        for (const auto& booking : bookings) {
            if (booking.getBookingStatus() != "Cancelled") {
                Train* train = trainManager->getTrainById(booking.getTrainId());
                
                cout << left << setw(10) << booking.getBookingId()
                     << setw(15) << booking.getJourneyDate()
                     << setw(10) << (train ? train->getTrainNumber() : "Unknown")
                     << setw(8) << booking.getBookingStatus() << endl;
                
                delete train;
            }
        }
        
        int bookingId = Utility::getIntInput("\nEnter Booking ID to cancel (0 to go back): ");
        
        if (bookingId == 0) return;
        
        bool found = false;
        for (const auto& booking : bookings) {
            if (booking.getBookingId() == bookingId && booking.getBookingStatus() != "Cancelled") {
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "Invalid Booking ID or booking already cancelled.\n";
            Utility::pressEnterToContinue();
            return;
        }
        
        cout << "Are you sure you want to cancel booking #" << bookingId << "? (y/n): ";
        string choice;
        getline(cin, choice);
        
        if (choice == "y" || choice == "Y") {
            if (bookingManager->cancelBooking(bookingId)) {
                cout << "Booking cancelled successfully.\n";
                cout << "A refund will be processed according to the cancellation policy.\n";
            } else {
                cout << "Failed to cancel booking. Please try again.\n";
            }
        } else {
            cout << "Cancellation aborted.\n";
        }
        
        Utility::pressEnterToContinue();
    }
    
    void updateProfile() {
        Utility::clearScreen();
        cout << "\n===== UPDATE PROFILE =====\n";
        
        currentUser->displayInfo();
        cout << "\nEnter new details (leave blank to keep current value):\n";
        
        string fullName = Utility::getInput("Full Name: ");
        string email = Utility::getInput("Email: ");
        string phone = Utility::getInput("Phone: ");
        
        if (!fullName.empty()) currentUser->setName(fullName);
        if (!email.empty()) currentUser->setEmail(email);
        if (!phone.empty()) currentUser->setPhone(phone);
        
        if (userManager->updateUserProfile(*currentUser)) {
            cout << "Profile updated successfully!\n";
        } else {
            cout << "Failed to update profile. Please try again.\n";
        }
        
        Utility::pressEnterToContinue();
    }
    
    void changePassword() {
        Utility::clearScreen();
        cout << "\n===== CHANGE PASSWORD =====\n";
        
        string currentPassword = Utility::getInput("Enter current password: ");
        
        if (currentPassword != currentUser->getPassword()) {
            cout << "Incorrect current password.\n";
            Utility::pressEnterToContinue();
            return;
        }
        
        string newPassword = Utility::getInput("Enter new password: ");
        string confirmPassword = Utility::getInput("Confirm new password: ");
        
        if (newPassword != confirmPassword) {
            cout << "Passwords do not match.\n";
            Utility::pressEnterToContinue();
            return;
        }
        
        if (userManager->changePassword(currentUser->getUserId(), newPassword)) {
            // Update current user's password in memory
            currentUser->setPassword(newPassword);
            cout << "Password changed successfully!\n";
        } else {
            cout << "Failed to change password. Please try again.\n";
        }
        
        Utility::pressEnterToContinue();
    }
    
public:
    Menu() {
        dbConnector = new DatabaseConnector();
        userManager = new UserManager(dbConnector);
        trainManager = new TrainManager(dbConnector);
        bookingManager = new BookingManager(dbConnector, trainManager);
        paymentSystem = new PaymentSystem(dbConnector, bookingManager);
        currentUser = nullptr;
    }
    
    ~Menu() {
        delete dbConnector;
        delete userManager;
        delete trainManager;
        delete bookingManager;
        delete paymentSystem;
        delete currentUser;
    }
    
    void run() {
        int choice;
        bool running = true;
        
        while (running) {
            if (!currentUser) {
                // Main menu (not logged in)
                displayMainMenu();
                choice = Utility::getIntInput("");
                
                switch (choice) {
                    case 1: // Login
                        if (loginUser()) {
                            runUserMenu();
                        }
                        break;
                    case 2: // Register
                        registerUser();
                        break;
                    case 3: // Exit
                        running = false;
                        cout << "Thank you for using Railway Ticket Booking System. Goodbye!\n";
                        break;
                    default:
                        cout << "Invalid choice. Please try again.\n";
                        Utility::pressEnterToContinue();
                }
            } else {
                runUserMenu();
                // User logged out
                currentUser = nullptr;
            }
        }
    }
    
    void runUserMenu() {
        int choice;
        bool userLoggedIn = true;
        
        while (userLoggedIn) {
            displayUserMenu();
            choice = Utility::getIntInput("");
            
            switch (choice) {
                case 1: // Search Trains
                    searchTrains();
                    break;
                case 2: // View All Trains
                    viewAllTrains();
                    break;
                case 3: // Book Ticket
                    bookTicket();
                    break;
                case 4: // View My Bookings
                    viewMyBookings();
                    break;
                case 5: // Cancel Booking
                    cancelBooking();
                    break;
                case 6: // Update Profile
                    updateProfile();
                    break;
                case 7: // Change Password
                    changePassword();
                    break;
                case 8: // Logout
                    userLoggedIn = false;
                    cout << "Logged out successfully.\n";
                    Utility::pressEnterToContinue();
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
                    Utility::pressEnterToContinue();
            }
        }
    }
};

// ============= MAIN FUNCTION =============
int main() {
    cout << "Initializing Railway Ticket Booking System...\n";
    
    try {
        Menu bookingSystem;
        bookingSystem.run();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}