# Railway Ticket Booking System

## Overview

Railway Ticket Booking System is a C++ console application that demonstrates a comprehensive railway ticket reservation platform. This system allows users to register accounts, search for trains, book tickets, manage their bookings, and process payments through a simple interface. The application uses MySQL for data persistence.

## Features

- **User Management**
  - User registration and login
  - Profile management
  - Password changing

- **Train Management**
  - View all available trains
  - Search trains by source/destination

- **Booking System**
  - Book tickets with multiple passenger details
  - View booking history
  - Cancel existing bookings
  - Seat availability checking

- **Payment Processing**
  - Multiple payment method options
  - Payment status tracking

## System Requirements

- C++ compiler with C++11 support
- MySQL Server (version 5.7+)
- MySQL Connector/C++ library

## Installation

### Prerequisites

1. Install MySQL Server:
   ```bash
   # For Ubuntu/Debian
   sudo apt install mysql-server
   
   # For macOS
   brew install mysql
   ```

2. Install MySQL Connector/C++:
   ```bash
   # For Ubuntu/Debian
   sudo apt install libmysqlcppconn-dev
   
   # For macOS
   brew install mysql-connector-c++
   ```

### Setup Database

1. Log in to MySQL:
   ```bash
   mysql -u root -p
   ```

2. Create the database and tables:
   ```sql
   CREATE DATABASE railway_booking_system;
   USE railway_booking_system;

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
   ```

3. Add sample data:
   ```sql
   INSERT INTO trains (train_name, train_number, source, destination, departure_time, arrival_time, total_seats) VALUES
   ('Rajdhani Express', 'RAJ2025', 'Delhi', 'Mumbai', '16:00:00', '08:00:00', 500),
   ('Shatabdi Express', 'SHT1050', 'Chennai', 'Bangalore', '06:00:00', '10:30:00', 400),
   ('Duronto Express', 'DUR2210', 'Kolkata', 'Delhi', '23:00:00', '14:00:00', 450);
   ```

### Compile and Run

1. Compile the application:
   ```bash
   g++ -o railway_booking main.cpp -lmysqlcppconn
   ```

2. Run the application:
   ```bash
   ./railway_booking
   ```

## Configuration

The database connection parameters can be modified in the `DatabaseConnector` class:

```cpp
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "password"; // Change to your MySQL password
```

Update these values to match your MySQL server configuration.

## Usage Guide

### Main Menu
- **Login**: Access your account
- **Register**: Create a new account
- **Exit**: Close the application

### User Menu
- **Search Trains**: Find trains by source/destination
- **View All Trains**: See all available trains
- **Book Ticket**: Book a new train ticket
- **View My Bookings**: See your booking history
- **Cancel Booking**: Cancel an existing booking
- **Update Profile**: Update your account details
- **Change Password**: Change your account password
- **Logout**: Return to the main menu

### Booking Process
1. Select a train from the available list
2. Enter journey date and number of passengers
3. Enter passenger details
4. Review booking information and fare
5. Select payment method
6. Complete payment

## Project Structure

The project follows object-oriented design principles with the following key classes:

- **Person**: Base class for user information
- **User**: Extends Person with authentication details
- **UserManager**: Handles user operations
- **Train**: Stores train information
- **TrainManager**: Handles train operations
- **Passenger**: Stores passenger details
- **Booking**: Contains booking information
- **BookingManager**: Handles booking operations
- **PaymentSystem**: Processes payments
- **Menu**: Manages the user interface
- **Utility**: Provides helper functions
- **DatabaseConnector**: Handles database connections

## Security Notes

This is a demonstration project and has several security limitations:

- Passwords are stored in plain text (not secure for production)
- No input validation for SQL injection prevention
- No encryption for sensitive data

For a production system, implement proper security measures including password hashing, input validation, and data encryption.

## Future Enhancements

- Add user roles (admin, staff, customer)
- Implement advanced search filters
- Add seat selection feature
- Generate e-tickets
- Implement waitlist functionality
- Add email notifications

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Developed as an educational project to demonstrate OOP concepts in C++
- Inspired by real-world railway booking systems
