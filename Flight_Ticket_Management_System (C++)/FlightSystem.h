#pragma once

#include <string>

inline constexpr int SIZE = 100;

extern std::string currentUser;

int readIntInRange(const std::string& prompt, int minimum, int maximum);
void title();
void menu();
void FlightSchedule();
void registration();
void readUser(std::string[], std::string[], std::string[], std::string[], std::string[], std::string[], int&);
int login(std::string[], std::string[], int);
void performBooking();
void readBooking(std::string[], std::string[], int[], std::string[], int[], int[], std::string[], int[], int&);
void editBooking(std::string[], std::string[], int[], std::string[], int[], int[], std::string[], int[], int);
void payment();
void readPaymentCheckIn(int&, std::string&, std::string[], std::string[], std::string[], std::string[], std::string[], std::string[], std::string&);
void checkIn(int&, std::string&, std::string[], std::string[], std::string[], std::string[], std::string[], std::string[], std::string&);
void printInvoice(std::string[], std::string[], std::string[], std::string[], std::string[], std::string[], int[], std::string[], int[], int[], std::string[], int[]);
